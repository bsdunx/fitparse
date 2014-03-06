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
 */

#include <getopt.h>
#include <ctype.h>

#include "fitparse.h"
#include "util.h"

#define CLIENT_VERSION "0.0.1"
#define MAX_INPUT 32


/*
 * --csv, --gpx, --fit, --tcx, --format={csv,gpx,fit,tcx} (downcase)
 * all output format
 *
 * -o/-output = file name to output (could also just be second name)
 *
 * input format not required - can always tell or guess
 *
 * --merge - takes two files
 * --split - needs a timestamp... (or distance?)
 * --crop - needs a timestamp (or distance?)
 *
 * --fix, --fix={gps,hr,power,gaps,all} (downcase)
 *
 * --summary - human readable summary
 * --laps - basic laps summary
 *
 *  also reads from stdin and outputs to stdout by default
 *
 * -c/config = config file (has user data and options to pass to things like csv
 *write)
 * --gender=m,f --units=metric,imperial (u) --hrmax=205  --ftp=300
 *
 *
 *  --help (?), --version
 *
 */

typedef struct {
  FileFormat format;
  char **input, *output, *config;
  bool merge, split, crop, summary, laps;
  // TODO fix
  Gender gender;
  Units units;
  unsigned hr, ftp
}


} Options;

static int version(void) {
    printf("client: %s fitparse: %s", CLIENT_VERSION, FITPARSE_VERSION);
    return 0;
}

static int help(char *name) {
  printf("Usage %s\n", name);
  return 1;
}

static int validate_options(Options *options) {
}

static void destroy_options(Options *options) {
  if (options->output) free(options->output);
  if (options->config) free(options->config);
  if (options->input) free(options->input);
}

int main(int argc, char *argv[]) {

  Options options = {0};

  static struct option longopts[] = {
    {"help", required_argument, NULL, 'h'},
    {"version", required_argument, NULL, 'v'},

    {"output", required_argument, NULL, 'o'},
    {"config", required_argument, NULL, 'c'},

    {"csv", no_argument, &options.format, CSV},
    {"gpx", no_argument, &options.format, GPX},
    {"tcx", no_argument, &options.format, TCX},
    {"fit", no_argument, &options.format, FIT},

    {"summary", no_argument, &options.summary, true},
    {"laps", no_argument, &options.laps, true},

    {"merge", no_argument, &options.merge, true},
    {"split", required_argument, &options.split, true},
    {"crop", required_argument, &options.crop, true},

    {"format", required_argument, NULL, 0},
    {"fix", required_argument, NULL, 0},

    {"gender", required_argument, NULL, 0},
    {"units", required_argument, NULL, 0},
    {"hr", required_argument, NULL, 0},
    {"ftp", required_argument, NULL, 0},
  };

  int c, longindex = 0;
  while ((c = getopt_long(argc, argv, "vhoc", longopts, &longindex)) != -1) {
    switch (c) {
      case 0:
        if (!strcmp("format", longopts[longindex].name)) {
          downcase(optarg);
          if ((options.format = file_format(optarg)) == UnknownFileFormat) {
            fprintf(stderr, "Unknown file format: %s\n", optarg);
            destroy_options(&options);
            return usage(argv[0]);
          }
        }

        if (!strcmp("fix", longopts[longindex].name)) {
          downcase(optarg);
          // TODO
        }

        if (!strcmp("gender", longopts[longindex].name)) {
          options.gender = (tolower(*optarg) == 'f') ? Female : Male;
        }
        if (!strcmp("units", longopts[longindex].name)) {
          options.units = (tolower(*optarg) == 'i') ? Imperial : Metric;
        }


        // TODO
        break;
      case 'v':
        destroy_options(&options);
        return version();
        break;
      case 'h':
        destroy_options(&options);
        return usage(argv[0]);
        break;
      case 'o':
        options.output = strdup(optarg);
        break;
      case 'c':
        options.config = strdup(optarg);
        break;
      default:
        destroy_options(&options);
        return usage(argv[0]);
    }
  }
  if (optind < argc) {
    if (!(options.input = malloc((argc-optind) * sizeof(*options.input)))) return 1;
    for (i= 0; optind < argc; optind++, i++) {
      options.input[i] = argv[optind];
    }
  }

  if (!validate_options(&options)) {
    destroy_options(&options);
    return usage(argv[0]);
  }

  destroy_options(&options);
  return 0;
}
