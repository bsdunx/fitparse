#include <stdio.h>
#include <ctype.h>

#include "activity.h"
#include "mxml.h"

/*
Activity *activity =  activity_new();
int error = gpx_read("file.gpx", &activity);
activity_destroy(activity);
*/

int allspace(const char *str) {
  while (isspace(*str)) { str++; }
  return !*str;
}

static void sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *sax_data) {
  const char *name, *data;
  if (event == MXML_SAX_ELEMENT_OPEN) {
    name = mxmlGetElement(node);
    fprintf(stderr, "open: <%s>\n", name);
  } else if (event == MXML_SAX_ELEMENT_CLOSE) {
    name = mxmlGetElement(node);
    data = mxmlGetOpaque(node);
    if (!allspace(data)) {
      fprintf(stderr, "data: %d '%s'\n", mxmlGetType(node), data);
    }
    fprintf(stderr, "close: </%s>\n", name);
  } else if (event == MXML_SAX_DATA) {
    mxmlRetain(node);
  }
}

int gpx_read(char *filename, Activity *activity) {
  FILE *f = NULL;
  if (!(f = fopen(filename, "r"))) {
    return 1;
  }

  mxmlSAXLoadFile(NULL, f, MXML_OPAQUE_CALLBACK, sax_cb, NULL);

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
