# fitparse

![pre-alpha](http://img.shields.io/badge/status-pre--alpha-lightgrey.svg)

## Overview

C parser for various fitness device file formats. `fitparse` enables reading and
converting between several fitness file formats, as well as providing several
utilities for cleaning up potential errors in the data.

## License

`fitparse` is licensed under the GPLv2 as described in the LICENSE file.
Much of the code for `fitparse` was influenced by GoldenCheetah, and certain
files reflect this in their headers. `fitparse` also relies on several libraries
bundled in the `lib/` directory, which have their own licenses. For reference:

- [GoldenCheetah](https://github.com/GoldenCheetah/GoldenCheetah) - GPLv2
- [Mini-XML](https://github.com/scheibo/mxml) - LGPLv2 w/ static linking
- [date](https://github.com/scheibo/date) - GPLv2
