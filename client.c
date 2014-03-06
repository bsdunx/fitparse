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
 * -c/config = config file (has user data and options to pass to things like csv
 *write)
 *
 * input format not required - can always tell or guess
 *
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
 */

int main(int argc, char *argv[]) {

  /*static struct option long_options[] = { };*/
  /*getopt_long_only(argc, argv,);*/

  return 0;
}
