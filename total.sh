#! /bin/sh
perl -ne 'printf "%s (%d%%)\n", $1, 100 * (hex($1) - 0x200) / (0xeff0 - 0x200) if /([\dA-F]{4}) B __end/;' a.map
