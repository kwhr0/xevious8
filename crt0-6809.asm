	.export _exit

	.code
start:
	orcc	#$10		; disable interrupt
	lds	#$efef	; extended
	ldx	#interrupt
	stx	$fff8
	ldx	#__bss_size
	beq	nobss
	clra
	clrb
	subd	#__bss_size
	ldx	#__bss
clear_bss:
	clr	,x
	leax	1,x
	incb
	bne	clear_bss
	inca
	bne	clear_bss
nobss:
	lbsr	_main
_exit:
	cwai	#0xef
	bra	_exit

	.export	_interruptCount
interrupt:
;	.byte	0x12,0x12,0x12
	inc	_interruptCount
	rti
	.bss
_interruptCount:	.byte	0


	.code
	.export	_wait
	.export	_ei
	.export	_di
_wait:
	cwai	#0xef
	rts
_ei:
	andcc	#$ef
	rts
_di:
	orcc	#$10
	rts
