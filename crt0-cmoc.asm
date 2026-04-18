INILIB	import
_main	import
	section	start
	ldy	#0
	lds	#0xefef
	ldd	#interrupt
	std	0xfff8
	ldd	#0xfc00
	lbsr	INILIB
	lbsr	_main
inf:
	bra	inf
	endsection

	if 1
	section	code
_wait	export
_ei	export
_di	export
interrupt:
	inc	_interruptCount
	rti
_wait:
	cwai	#0xef
	rts
_ei:
	andcc	#$ef
	rts
_di:
	orcc	#$10
	rts
	endsection

_interruptCount	export
	section	bss
_interruptCount:	.byte	0
	endsection

	endif
