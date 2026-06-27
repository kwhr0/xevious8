	.export	_exit
	.export	__STARTUP__ : absolute = 1
	.import	zerobss, _main
	.import	initlib, donelib
	.import	__STACKSTART__

	.include "zeropage.inc"

	.segment "STARTUP"

	lda	#<irq
	sta	$fffe
	lda	#>irq
	sta	$ffff
	lda #<__STACKSTART__
	ldx #>__STACKSTART__
	sta c_sp
	stx c_sp+1
	jsr zerobss
	jsr initlib
	jsr _main
_exit:
	pha
	jsr donelib
	pla
	rts

	.code
irq:
	php
;	pha
	inc	_interruptCount
;	pla
	plp
	rti

	.export	_ei
	.export	_di
	.export	_wait
_ei:
	cli
	rts
_di:
	sei
	rts
_wait:
	rts

	.export	_interruptCount
	.bss
_interruptCount:
	.byte	0
