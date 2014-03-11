/*
 *  Copyright (c) 2014 Kirk Scheibelhut <kjs@scheibo.com>
 *
 *  This file is free software: you may copy, redistribute and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *     Copyright (c) 2010 Greg Lonnon (greg.lonnon@gmail.com)
 *     Copyright (c) 2008 Sean C. Rhea (srhea@srhea.net),
 *     Copyright (c) 2008 J.T Conklin (jtc@acorntoolworks.com)
 *
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU General Public License as published by the
 *     Free Software Foundation; either version 2 of the License, or (at your
 *     option) any later version.
 *
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with this program; if not, write to the Free Software Foundation, Inc.,
 *     51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "mxml.h"

#include "activity.h"
#include "util.h"

/**
 * State
 *
 * Description:
 *  Structure used to maintain state between calls to the sax_cb.
 *
 * Fields:
 *  activity - the activity being constructed during parsing.
 *  metadata - whether or not we are currently inside the <metadata> tag.
 *  first_element - whether we have seen the first element yet.
 *  dp - the current datapoint which we are building up to add to `Activity`.
 */
typedef struct {
  Activity *activity;
  bool metadata;
  bool first_element;
  DataPoint dp;
} State;

/**
 * sax_cb
 *
 * Description:
 *  MXML SAX callback which processes events and nodes as a stream. We retain
 *  nodes which have data and process them on ELEMNT_CLOSE so that we know what
 *  kind of node was read.
 *
 * Parameters:
 *  node - the current node in the tree being proessed.
 *  event - the type of SAX event currently being processed.
 *  sax_data - a pointer to the `State` object used to build up the `Activity`.
 *
 * Return value:
 *  0 - continue processing.
 *  1 - stop processing the XML and clean up.
 *      Added to enable to MXML to realize we're not processing a GPX file.
 */
static int sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *sax_data) {
  const char *name, *attr, *data;
  State *state = (State *)sax_data;

  if (event == MXML_SAX_ELEMENT_OPEN) {
    if (state->metadata) return 0;

    name = mxmlGetElement(node);

    if (state->first_element) {
      if (strcmp(name, "gpx")) {
        return 1; /* stop reading the file */
      }

      /* we must retain an element so that mxmlSAXLoadFile can return it */
      mxmlRetain(node);
    }

    if (!strcmp(name, "metadata")) {
      state->metadata = true;
    } else if (!strcmp(name, "trkpt")) {
      attr = mxmlElementGetAttr(node, "lat");
      if (attr) {
        parse_field(Latitude, &(state->dp), attr);
      }
      attr = mxmlElementGetAttr(node, "lon");
      if (attr) {
        parse_field(Longitude, &(state->dp), attr);
      }
    }
    state->first_element = false;
  } else if (event == MXML_SAX_ELEMENT_CLOSE) {
    name = mxmlGetElement(node);
    data = mxmlGetOpaque(node);

    if (!strcmp(name, "metadata")) {
      state->metadata = false;
    } else if (state->metadata) {
      return 0;
    } else if (!strcmp(name, "time")) {
      parse_field(Timestamp, &(state->dp), data);
    } else if (!strcmp(name, "ele")) {
      parse_field(Altitude, &(state->dp), data);
    } else if (!strcmp(name, "gpxdata:hr") || !strcmp(name, "gpxtpx:hr")) {
      parse_field(HeartRate, &(state->dp), data);
    } else if (!strcmp(name, "gpxdata:temp") || !strcmp(name, "gpxtpx:atemp")) {
      parse_field(Temperature, &(state->dp), data);
    } else if (!strcmp(name, "gpxdata:cadence") ||
               !strcmp(name, "gpxtpx:cad")) {
      parse_field(Cadence, &(state->dp), data);
    } else if (!strcmp(name, "gpxdata:bikepower")) {
      parse_field(Power, &(state->dp), data);
    } else if (!strcmp(name, "trkpt")) {
      activity_add_point(state->activity, &(state->dp));
      unset_data_point(&(state->dp));
    }
  } else if (event == MXML_SAX_DATA) {
    mxmlRetain(node);
  }

  return 0;
}

/**
 * gpx_read
 *
 * Description:
 *  Read in the GPX file pointed to by `f` and return an `Activity`.
 *
 * Parameters:
 *  f - The file descriptor for the GPX file to read.
 *
 * Return value:
 *  NULL - unable to read in GPX or invalid GPX file.
 *  valid pointer - a valid pointer to a newly allocated Activity instance.
 *                  The caller is responsible for freeing the activity.
 */
Activity *gpx_read(FILE *f) {
  mxml_node_t *tree;
  State state = {NULL, false /* metadata */, true /* first_element */, {{0}}};
  unset_data_point(&(state.dp));

  if (!(state.activity = activity_new())) return NULL;

  if (!(tree = mxmlSAXLoadFile(NULL, f, MXML_OPAQUE_CALLBACK, sax_cb,
                               (void *)&state))) {
    activity_destroy(state.activity);
    fclose(f);
    return NULL;
  }

  state.activity->format = GPX;

  mxmlDelete(tree);
  fclose(f);
  return state.activity;
}

/**
 * to_gpx_xml
 *
 * Description:
 *  Deals with the MXML specific logic for writing a GPX file as XML.
 *
 * Parameters:
 *  a - the `Activity` to convert into XML.
 *
 * Return value:
 *  valid pointer - a pointer to the mxml_node_t root node for the xml.
 *                  The caller is responsible for freeing this node.
 */
static mxml_node_t *to_gpx_xml(Activity *a) {
  char buf[TIME_BUFSIZ]; /* we don't need to zero since all the same length */
  unsigned i;
  mxml_node_t *xml, *gpx, *metadata, *trk, *time, *name, *trkseg, *trkpt, *ele,
      *extensions, *gpxtpx, *atemp, *hr, *cad;

  xml = mxmlNewXML("1.0");

  /* create gpx root */
  gpx = mxmlNewElement(xml, "gpx");
  mxmlElementSetAttr(gpx, "creator", "fitparse");
  mxmlElementSetAttr(gpx, "version", "1.1");
  mxmlElementSetAttr(gpx, "xmlns", "http://www.topografix.com/GPX/1/1");
  mxmlElementSetAttr(gpx, "xmlns:xsi",
                     "http://www.w3.org/2001/XMLSchema-instance");
  mxmlElementSetAttr(gpx, "xmlns:gpxtpx",
                     "http://www.garmin.com/xmlschemas/TrackPointExtension/v1");
  mxmlElementSetAttr(gpx, "xmlns:gpxx",
                     "http://www.garmin.com/xmlschemas/GpxExtensions/v3");
  mxmlElementSetAttr(gpx, "xsi:schemaLocation",
                     "http://www.topografix.com/GPX/1/1 "
                     "http://www.topografix.com/GPX/1/1/gpx.xsd "
                     "http://www.garmin.com/xmlschemas/GpxExtensions/v3 "
                     "http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd "
                     "http://www.garmin.com/xmlschemas/TrackPointExtension/v1 "
                     "http://www.garmin.com/xmlschemas/"
                     "TrackPointExtensionv1.xsd");

  /* write metadata element */
  metadata = mxmlNewElement(gpx, "metadata");
  time = mxmlNewElement(metadata, "time");
  format_timestamp(buf, a->start_time);
  mxmlNewText(time, 0, buf);

  /* TODO lap waypoints?  or lap as metadata? */

  trk = mxmlNewElement(gpx, "trk");
  name = mxmlNewElement(trk, "name");
  mxmlNewText(name, 0, "Untitled");

  trkseg = mxmlNewElement(trk, "trkseg");
  for (i = 0; i < a->num_points; i++) {
    trkpt = mxmlNewElement(trkseg, "trkpt");
    mxmlElementSetAttrf(trkpt, "lat", "%.7f", a->data_points[i].data[Latitude]);
    mxmlElementSetAttrf(trkpt, "lon", "%.7f",
                        a->data_points[i].data[Longitude]);

    if (SET(a->data_points[i].data[Altitude])) {
      ele = mxmlNewElement(trkpt, "ele");
      mxmlNewTextf(ele, 0, "%.2f", a->data_points[i].data[Altitude]);
    }
    if (SET(a->data_points[i].data[Timestamp])) {
      time = mxmlNewElement(trkpt, "time");
      format_timestamp(buf, a->data_points[i].data[Timestamp]);
      mxmlNewText(time, 0, buf);
    }

    if (SET(a->data_points[i].data[HeartRate]) ||
        SET(a->data_points[i].data[Cadence]) ||
        SET(a->data_points[i].data[Temperature])) {
      extensions = mxmlNewElement(trkpt, "extensions");
      gpxtpx = mxmlNewElement(extensions, "gpxtpx:TrackPointExtension");

      if (SET(a->data_points[i].data[HeartRate])) {
        hr = mxmlNewElement(gpxtpx, "gpxtpx:hr");
        mxmlNewInteger(hr, a->data_points[i].data[HeartRate]);
      }
      if (SET(a->data_points[i].data[Cadence])) {
        cad = mxmlNewElement(gpxtpx, "gpxtpx:cad");
        mxmlNewInteger(cad, a->data_points[i].data[Cadence]);
      }
      if (SET(a->data_points[i].data[Temperature])) {
        atemp = mxmlNewElement(gpxtpx, "gpxtpx:atemp");
        mxmlNewInteger(atemp, a->data_points[i].data[Temperature]);
      }
    }
  }

  return xml;
}

/**
 * gpx_write
 *
 * Description:
 *  Write the `Activity` to `f` in GPX format.
 *
 * Parameters:
 *  f - the file descriptor for the GPX file to write to.
 *  a - the `Activity` to write.
 *
 * Return value:
 *  0 - successfully wrote GPX file.
 *  1 - unable to write GPX.
 */
int gpx_write(FILE *f, Activity *a) {
  mxml_node_t *tree;

  assert(a != NULL);

  if (!a->last[Latitude] && !a->last[Longitude]) return 1;
  if (!(tree = to_gpx_xml(a))) return 1;

  if (mxmlSaveFile(tree, f, MXML_NO_CALLBACK) < 0) {
    mxmlDelete(tree);
    fclose(f);
    return 1;
  }

  mxmlDelete(tree);
  fclose(f);
  return 0;
}
