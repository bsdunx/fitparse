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
#include "tcx.h"
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
 *  TODO
 *
 * Parameters:
 *  node - the current node in the tree being proessed.
 *  event - the type of SAX event currently being processed.
 *  sax_data - a pointer to the `State` object used to build up the `Activity`.
 *
 * Return value:
 *  0 - continue processing.
 *  1 - stop processing the XML and clean up.
 *      Added to enable to MXML to realize we're not processing a TCX file.
 */
static int sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *sax_data) {
  const char *name, *attr, *data;
  State *state = (State *)sax_data;

  if (event == MXML_SAX_ELEMENT_OPEN) {
    if (state->metadata) return 0;

    name = mxmlGetElement(node);

    if (state->first_element) {
      if (strcmp(name, "TraingCenterDatabase")) {
        return 1; /* stop reading the file */
      }

      /* we must retain an element so that mxmlSAXLoadFile can return it */
      mxmlRetain(node);
    }

    if (!strcmp(name, "Lap")) {
      /* TODO */
    } else if (!strcmp(name, "Trackpoint")) {
      /* TODO do we need to do anything? */
    }
    state->first_element = false;
  } else if (event == MXML_SAX_ELEMENT_CLOSE) {
    name = mxmlGetElement(node);
    data = mxmlGetOpaque(node);

    /* TODO validate everything is the correc tunits (distance, speed) */
    if (!strcmp(name, "Time")) {
      parse_field(Timestamp, &(state->dp), data);
    } else if (!strcmp(name, "DistanceMeters")) {
      parse_field(Distance, &(state->dp), data); /* TODO */
    } else if (!strcmp(name, "Watts") || !strcmp(name, "ns3:Watts")) {
      parse_field(Power, &(state->dp), data);
    } else if (!strcmp(name, "Speed") || !strcmp(name, "ns3:Speed")) {
      parse_field(Speed, &(state->dp), data); /* TODO */
    } else if (!strcmp(name, "Value")) {
      parse_field(HeartRate, &(state->dp), data);
    } else if (!strcmp(name, "Cadence")) {
      parse_field(Cadence, &(state->dp), data);
    } else if (!strcmp(name, "AltitudeMeters")) {
      parse_field(Altitude, &(state->dp), data);
    } else if (!strcmp(name, "LongitudeDegrees")) {
      parse_field(Longitude, &(state->dp), data); /* TODO */
    } else if (!strcmp(name, "LatitudeDegrees")) {
      parse_field(Longitude, &(state->dp), data); /* TODO */
    } else if (!strcmp(name, "Trackpoint")) {
      /* TODO */
      activity_add_point(state->activity, &(state->dp));
      unset_data_point(&(state->dp));
    }
  } else if (event == MXML_SAX_DATA) {
    mxmlRetain(node);
  }

  return 0;
}

/**
 * tcx_read
 *
 * Description:
 *  Read in the TCX file pointed to by `f` and return an `Activity`.
 *
 * Parameters:
 *  f - The file descriptor for the TCX file to read.
 *
 * Return value:
 *  NULL - unable to read in TCX or invalid TCX file.
 *  valid pointer - a valid pointer to a newly allocated Activity instance.
 *                  The caller is responsible for freeing the activity.
 */
Activity *tcx_read(FILE *f) {
  mxml_node_t *tree;
  State state = {NULL, false /* metadata */, true /* first_element */, {{0}}};
  unset_data_point(&(state.dp));

  if (!(state.activity = activity_new())) return NULL;

  if (!(tree = mxmlSAXLoadFile(NULL, f, MXML_OPAQUE_CALLBACK, sax_cb,
                               (void *)&state))) {
    activity_destroy(state.activity);
    return NULL;
  }

  state.activity->format = TCX;

  mxmlDelete(tree);
  return state.activity;
}

/**
 * to_tcx_xml
 *
 * Description:
 *  Deals with the MXML specific logic for writing a TCX file as XML.
 *  TODO
 *
 * Parameters:
 *  a - the `Activity` to convert into XML.
 *
 * Return value:
 *  valid pointer - a pointer to the mxml_node_t root node for the xml.
 *                  The caller is responsible for freeing this node.
 */
static mxml_node_t *to_tcx_xml(Activity *a) {
  char buf[TIME_BUFSIZ]; /* we don't need to zero since all the same length */
  unsigned i;
  mxml_node_t *xml, *tcx;

  xml = mxmlNewXML("1.0");

  /* create tcx root */
  tcx = mxmlNewElement(xml, "TrainingCenterDatabase");
  mxmlElementSetAttr(
      tcx, "xmlns",
      "http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2");
  mxmlElementSetAttr(tcx, "xmlns:ns2",
                     "http://www.garmin.com/xmlschemas/UserProfile/v2");
  mxmlElementSetAttr(tcx, "xmlns:ns3",
                     "http://www.garmin.com/xmlschemas/ActivityExtension/v2");
  mxmlElementSetAttr(tcx, "xmlns:ns4",
                     "http://www.garmin.com/xmlschemas/ProfileExtension/v1");
  mxmlElementSetAttr(tcx, "xmlns:ns5",
                     "http://www.garmin.com/xmlschemas/ActivityGoals/v1");
  mxmlElementSetAttr(tcx, "xmlns:xsi",
                     "http://www.w3.org/2001/XMLSchema-instance");
  mxmlElementSetAttr(
      tcx, "xsi:schemaLocation",
      "http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 "
      "http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd");

  /* TODO */

  return xml;
}

/**
 * tcx_write
 *
 * Description:
 *  Write the `Activity` to `f` in TCX format.
 *
 * Parameters:
 *  f - the file descriptor for the TCX file to write to.
 *  a - the `Activity` to write.
 *
 * Return value:
 *  0 - successfully wrote TCX file.
 *  1 - unable to write TCX.
 */
int tcx_write(FILE *f, Activity *activity) {
  mxml_node_t *tree;

  assert(a != NULL);

  if (!(tree = to_tcx_xml(a))) return 1;

  if (mxmlSaveFile(tree, f, MXML_NO_CALLBACK) < 0) {
    mxmlDelete(tree);
    return 1;
  }

  mxmlDelete(tree);
  return 0;
}
