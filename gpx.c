#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "activity.h"
#include "mxml.h"
#include "util.h"

typedef struct {
  Activity *activity;
  bool metadata;
  bool first_element;
  double first_time;
  DataPoint dp;
} State;

static inline void parse_field(DataField field, State *state, const char *str) {
  char *end;
  state->dp.data[field] = strtod(str, &end);
  if (*end) state->dp.data[field] = UNSET_FIELD;
}

static int sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *sax_data) {
  const char *name, *attr, *data;
  State *state = (State *)sax_data;

  if (event == MXML_SAX_ELEMENT_OPEN) {
    name = mxmlGetElement(node);
    if (state->metadata) return 0;

    name = mxmlGetElement(node);

    if (state->first_element) {
      if (strcmp(name, "gpx")) {
        return 1; /* stop reading the file */
      }

      /* TODO confirm no memory leak */
      /* We must retain an element so that mxmlSAXLoadFile can return it */
      mxmlRetain(node);
    }

    if (!strcmp(name, "metadata")) {
      state->metadata = true;
    } else if (!strcmp(name, "trkpt")) {
      attr = mxmlElementGetAttr(node, "lat");
      if (attr) {
        parse_field(Latitude, state, attr);
      }
      attr = mxmlElementGetAttr(node, "lon");
      if (attr) {
        parse_field(Longitude, state, attr);
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
      state->dp.data[Timestamp] = parse_timestamp(data);
      if (state->first_time == UNSET_FIELD && state->dp.data[Timestamp] != UNSET_FIELD) {
        state->first_time = state->dp.data[Timestamp];
      }
    } else if (!strcmp(name, "ele")) {
      parse_field(Altitude, state, data);
    } else if (!strcmp(name, "gpxdata:hr") || !strcmp(name, "gpxtpx:hr")) {
      parse_field(HeartRate, state, data);
    } else if (!strcmp(name, "gpxdata:temp") || !strcmp(name, "gpxtpx:atemp")) {
      parse_field(Temperature, state, data);
    } else if (!strcmp(name, "gpxdata:cadence") ||
               !strcmp(name, "gpxtpx:cad")) {
      parse_field(Cadence, state, data);
    } else if (!strcmp(name, "gpxdata:bikepower")) {
      parse_field(Power, state, data);
    } else if (!strcmp(name, "trkpt")) {
      activity_add_point(state->activity, &(state->dp));
      /*print_data_point(&(state->dp)); [> TODO <]*/
      unset_data_point(&(state->dp));
    }
  } else if (event == MXML_SAX_DATA) {
    mxmlRetain(node);
  }

  return 0;
}

Activity *gpx_read(char *filename) {
  FILE *f = NULL;
  mxml_node_t *tree;
  State state = { NULL, false /* metadata */, true /* first_element */, UNSET_FIELD /* first_time */, {{0}}};
  unset_data_point(&(state.dp));

  if (!(f = fopen(filename, "r"))) {
    return NULL;
  }

  if (!(state.activity = activity_new())) {
    return NULL;
  }

  if (!(tree = mxmlSAXLoadFile(NULL, f, MXML_OPAQUE_CALLBACK, sax_cb, (void *)&state))) {
    activity_destroy(state.activity);
    fclose(f);
    return NULL;
  }

  state.activity->format = GPX;

  mxmlDelete(tree);
  fclose(f);
  return state.activity;
}

static mxml_node_t *to_gpx_xml(Activity *a) {
  unsigned i;
  mxml_node_t *xml; /*, *gpx, *trk, *trkseg, *trkpt, *ele, *time, *hr, *temp,
*cadence, *bikepower; */

  xml = mxmlNewXML("1.0");
  for (i = 0; i < a->num_points; i++) {
    /* TODO */
  }
  return xml;
}

int gpx_write(char *filename, Activity *a) {
  FILE *f;
  mxml_node_t *tree;

  f = fopen(filename, "w");
  if (!(f = fopen(filename, "w")) || !(tree = to_gpx_xml(a))) {
    return 1;
  }

  if (mxmlSaveFile(tree, f, MXML_NO_CALLBACK) < 0) {
    mxmlDelete(tree);
    fclose(f);
    return 1;
  }

  mxmlDelete(tree);
  fclose(f);
  return 0;
}
