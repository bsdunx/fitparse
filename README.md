# fitparse

![pre-alpha](http://img.shields.io/badge/status-pre--alpha-lightgrey.svg)

## Overview

C parser for various fitness device file formats. `fitparse` enables reading and
converting between several fitness file formats, as well as providing several
utilities for cleaning up potential errors in the data.

## Installation

  make
  make test # optional
  make install

### Dependencies

`fitparse` depends on `Mini-XML` and the `git` date code. Both of these are
included in the `lib/` directory of the project and are statically linked by
default into the resulting binary. These projects are incredibly small -
bundling them with `fitparse` does not add any appreciable overhead - and
unlikely to be preinstalled on your system. Advanced users are free to tweak the
`Makefile` to alter how dependencies are used.

## Features

- small binary which is easy to build and incorporate into other projects
- read and write files in .CSV, .GPX, .TCX and .FIT format
- currently supports both running and bicycling files
- corrects missing or invalid data and cleans up dropouts and spikes
- merging and splitting files
- calculating summary data for files

## Examples

  fitparse 20140218-1.gpx -o output.csv

  fitparse --merge 20140218-1.gpx 20140218-2.tcx -o merged.fit

  fitparse --summary merged.fit

  fitparse --laps 20149218-1.gpx

TODO: add terminal gif.

## Usage

Include `fitparse.h` in your project for details in order to read in fitness
files and manipulate the data. `fitparse.h` should contain the most up to data
information about what calls are available.

## Options

TODO

## Future

- support additional file types
- support swimming / multisport

## Bugs

TODO

## Contributing

TODO

## License

`fitparse` is licensed under the GPLv2 as described in the LICENSE file.
Much of the code for `fitparse` was influenced by GoldenCheetah, and certain
files reflect this in their headers. `fitparse` also relies on several libraries
bundled in the `lib/` directory, which have their own licenses. For reference:

- [GoldenCheetah](https://github.com/GoldenCheetah/GoldenCheetah) - GPLv2
- [Mini-XML](https://github.com/scheibo/mxml) - LGPLv2 w/ static linking
- [date](https://github.com/scheibo/date) - GPLv2
