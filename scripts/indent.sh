#!/bin/sh
#
# Since this source tree uses the same coding style as Linux (or at least try
# to), this script was added to ensure all (most) source files are indented
# properly.
#
# References:
# > Linux coding style guide
#   https://www.kernel.org/doc/Documentation/CodingStyle
# > Indent flags (where I copied this from):
#   http://www.gnu.org/software/indent/manual/html_section/indent_4.html
#
indent -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 \
-saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 \
-cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai $@
