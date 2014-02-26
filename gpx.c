#include <stdio.h>

#include "activity.h"
#include "mxml.h"

/*
Activity *activity =  activity_new();
int error = gpx_read("file.gpx", &activity);
activity_destroy(activity);
*/

static void sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *sax_data) {
  const char *name, *data;
  int ws;
  if (event == MXML_SAX_ELEMENT_OPEN) {
    name = mxmlGetElement(node);
    fprintf(stderr, "open: <%s>\n", name);
  } else if (event == MXML_SAX_ELEMENT_CLOSE) {
    name = mxmlGetElement(node);
    fprintf(stderr, "close: </%s>\n", name);
  } else if (event == MXML_SAX_DATA) {
    data = mxmlGetText(node, &ws);
    fprintf(stderr, "data: '%s' (%d)\n", data, ws);
  }
}

int gpx_read(char *filename, Activity *activity) {
  FILE *f = NULL;
  if (!(f = fopen(filename, "r"))) {
    return 1;
  }

  mxmlSAXLoadFile(NULL, f, MXML_TEXT_CALLBACK, sax_cb, NULL);

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
  Activity activity =  activity_new();
  int ret = gpx_read(argv[1], &activity);
  /*activity_destroy(&activity);*/
  return ret;
}
