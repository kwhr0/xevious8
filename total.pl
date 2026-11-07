#! /usr/bin/perl

open(S, "a.map") || die;
while (<S>) {
	printf "%s (%d%%)\n", $1, 100 * (hex($1) - 0x200) / (0xeff0 - 0x200) if /([\dA-F]{4}) B __end/ || /BSS\s+\w+\s+00([\dA-F]{4})/ || /program_end.+load at ([\dA-F]{4})/ || /0000([\dA-F]{4})  s__HEAP /;
}
close S;
exit 0;
