#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "activity.h"
#include "mxml.h"
#include "util.h"

typedef struct {
  bool metadata;
  bool first_element;
  bool first_time;
  DataPoint dp;
} State;

int allspace(const char *str) {
  while (isspace(*str)) {
    str++;
  }
  return !*str;
}

static inline void parse_field(DataField field, State* state, const char *str) {
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

    if (state->first_element && strcmp(name, "gpx")) {
      fprintf(stderr, "error\n"); /* TODO */
      return 1; /* stop reading the file */
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
    } else if (!strcmp(name, "ele")) {
      parse_field(Altitude, state, data);
    } else if (!strcmp(name, "gpxdata:hr") || !strcmp(name, "gpxtpx:hr")) {
      parse_field(HeartRate, state, data);
    } else if (!strcmp(name, "gpxdata:temp") || !strcmp(name, "gpxtpx:atemp")) {
      parse_field(Temperature, state, data);
    } else if (!strcmp(name, "gpxdata:cadence") || !strcmp(name, "gpxtpx:cad")) {
      parse_field(Cadence, state, data);
    } else if (!strcmp(name, "gpxdata:bikepower")) {
      parse_field(Power, state, data);
    } else if (!strcmp(name, "trkpt")) {
      /* TODO */
      print_data_point(&(state->dp));
      unset_data_point(&(state->dp));
    }
  } else if (event == MXML_SAX_DATA) {
    mxmlRetain(node);
  }

  return 0;
}

int gpx_read(char *filename, Activity *activity) {
  FILE *f = NULL;
  State state = { false, true, true, {{0}} };
  unset_data_point(&(state.dp));

  if (!(f = fopen(filename, "r"))) {
    return 1;
  }

  if (!mxmlSAXLoadFile(NULL, f, MXML_OPAQUE_CALLBACK, sax_cb, (void *)&state)) {
    fprintf(stderr, "failed\n"); /* TODO */
    fclose(f);
    return 1;
  }

  fprintf(stderr, "Made it\n"); /* TODO */

  fclose(f);
  return 0;
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

/* TODO */
int main(int argc, char *argv[]) {
  Activity *a = activity_new();
  int err = gpx_read(argv[1], a);
  if (!err && argc > 2) {
    err = gpx_write(argv[2], a);
  }
  activity_destroy(a);
  return err;
}
