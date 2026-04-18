	.code
	.export _exit
start:
	sei		; disable interrupts
	lds	#$efef	; extended
	ldx	#interrupt
	stx	$fff8
	ldx	#swi
	stx	$fffa
	ldx	#nmi
	stx	$fffc
	ldx	#__bss_size
	beq	nobss
	clra
	clrb
	subb	#<__bss_size
	sbca	#>__bss_size
	ldx	#__bss
clear_bss:
	clr	,x
	inx
	incb
	bne	clear_bss
	inca
	bne	clear_bss
nobss:
	jsr	_main
_exit:
	wai
	bra	_exit

swi:
	rti
nmi:
	rti

	.export	_interruptCount
	.export	_interruptHandle
interrupt:
	.byte	1,1,1
;	inc	_interruptCount
	ldx	@tmp
	stx	@tmp_i
	ldx	@tmp1
	stx	@tmp1_i
	ldx	@tmp2
	stx	@tmp2_i
	ldx	_interruptHandle
	beq	interrupt1
	jsr	,x
interrupt1:
	ldx	@tmp_i
	stx	@tmp
	ldx	@tmp1_i
	stx	@tmp1
	ldx	@tmp2_i
	stx	@tmp2
	rti
	.zp
tmp_i:	.word	0
tmp1_i:	.word	0
tmp2_i:	.word	0

	.bss
_interruptCount:	.byte	0
_interruptHandle:	.word	0


	.code
	.export	_wait
	.export	_ei
	.export	_di
_wait:
	wai
	rts
_ei:
	cli
	rts
_di:
	sei
	rts
