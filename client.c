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

#include "fitparse.h"

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



int main(int argc, char *argv[]) {

  ProgramOptions options;

  static struct option long_options[] = {
    {"help", required_argument, NULL, '?'},
    {"version", required_argument, NULL, 'v'},

    {"output", required_argument, NULL, 'o'},
    {"config", required_argument, NULL, 'c'},

    {"csv", no_argument, &options.csv, true},
    {"gpx", no_argument, &options.gpx, true},
    {"tcx", no_argument, &options.fit, true},
    {"fit", no_argument, &options.tcx, true},

    {"summary", no_argument, &options.summary, true},
    {"laps", no_argument, &options.laps, true},

    {"merge", no_argument, &options.merge, true},
    {"split", required_argument, &options.split_, true},
    {"crop", required_argument, &options.crop_, true},

    {"format", required_argument, &options.format_, true},
    {"fix", required_argument, &options.fix_, true},

    {"gender", required_argument, &options.gender_, true},
    {"units", required_argument, &options.units_, true},
    {"hr", required_argument, &options.hr_, true},
    {"ftp", required_argument, &options.ftp_, true},
  };
  /*getopt_long_only(argc, argv,);*/

  // one output file in general
  // could be multiple outputs - splitting on lap or timestamp? - still, only one output name which is actually output base
  // multiple inputs though

  if (merge) {
    // can be one file
  }

  if (split) {
    // should be one file
  }

  if (crop) {
      // should be one file
  }

  if (fix) {
    // can be multiple files
  }


  // otherwise take multiple files and output them into format


  // otherwise


  return 0;
}
