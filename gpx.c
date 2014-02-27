#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "activity.h"
#include "mxml.h"
#include "util.h"

typedef enum { false, true } bool;

typedef struct {
  bool metadata;
  bool first_element;
  bool first_time;
  DataPoint data;
} State;

int allspace(const char *str) {
  while (isspace(*str)) {
    str++;
  }
  return !*str;
}

static int sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *sax_data) {
  const char *name, *attr, *data;
  char *end;
  State *state = (State *)sax_data;

  if (event == MXML_SAX_ELEMENT_OPEN) {
    name = mxmlGetElement(node);
    if (state->metadata) return 0;

    name = mxmlGetElement(node);
    if (state->first_element && strcmp(name, "gpx")) {
      fprintf(stderr, "error\n");
      return 1; /* stop reading the file */
    }

    if (!strcmp(name, "metadata")) {
      state->metadata = true;
    } else if (!strcmp(name, "trkpt")) {
      attr = mxmlElementGetAttr(node, "lat");
      if (attr) {
        state->data.latitude = strtod(attr, &end);
        if (*end) state->data.latitude = UNSET_LATITUDE;
      }
      attr = mxmlElementGetAttr(node, "lon");
      if (attr) {
        state->data.longitude = strtod(attr, &end);
        if (*end) state->data.longitude = UNSET_LONGITUDE;
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
      state->data.timestamp = parse_timestamp(data);
      if (!state->data.timestamp) state->data.timestamp = UNSET_TIMESTAMP;
    } else if (!strcmp(name, "ele")) {
      state->data.altitude = (int32_t)(strtod(data, &end) * 1000);
      if (*end) state->data.altitude = UNSET_ALTITUDE;
    } else if (!strcmp(name, "gpxtpx:hr")) {
      state->data.heart_rate = (uint8_t)strtod(data, &end);
      if (*end) state->data.heart_rate = UNSET_HEART_RATE;
    } else if (!strcmp(name, "gpxdata:hr")) {
      state->data.heart_rate = (uint8_t)strtod(data, &end);
      if (*end) state->data.heart_rate = UNSET_HEART_RATE;
    } else if (!strcmp(name, "gpxdata:temp")) {
      state->data.temperature = (int8_t)strtod(data, &end);
      if (*end) state->data.temperature = UNSET_TEMPERATURE;
    } else if (!strcmp(name, "gpxdata:cadence")) {
      state->data.cadence = (uint8_t)strtod(data, &end);
      if (*end) state->data.cadence = UNSET_CADENCE;
    } else if (!strcmp(name, "gpxdata:bikepower")) {
      state->data.power = (uint16_t)strtod(data, &end);
      if (*end) state->data.power = UNSET_POWER;
    } else if (!strcmp(name, "trkpt")) {
      /* TODO */
      print_data_point(&(state->data));
      CLEAR_DATA_POINT(state->data);
    }
  } else if (event == MXML_SAX_DATA) {
    mxmlRetain(node);
  }

  return 0;
}

int gpx_read(char *filename, Activity *activity) {
  FILE *f = NULL;
  State state = {false /* metadata */, true /* first_element */,
                 true /* first_time */, UNSET_DATA_POINT};
  if (!(f = fopen(filename, "r"))) {
    return 1;
  }

  if (!mxmlSAXLoadFile(NULL, f, MXML_OPAQUE_CALLBACK, sax_cb, (void *)&state)) {
    fprintf(stderr, "failed\n");
    fclose(f);
    return 1;
  }

  fprintf(stderr, "Made it\n");

  fclose(f);
  return 0;
}

int gpx_write(char *filename, Activity *activity) {
  /*FILE *f;
  mxml_node_t *tree;

  f = fopen(filename, "w");
  mxmlSaveFile(tree, f, MXML_NO_CALLBACK);

  mxmlDelete(tree);
  fclose(f);
  return 0;*/
  return 1;
}

/* TODO */
int main(int argc, char *argv[]) {
  Activity activity = activity_new();
  int ret = gpx_read(argv[1], &activity);
  /*activity_destroy(&activity);*/
  return ret;
}
