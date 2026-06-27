
	.code

	.export	_wait
	.export	_ei
	.export	_di
	.export	_clrs

start:
	lda	#$f0
	sta	@sp
	lda	#$ef
	sta	@sp+1	; initial stack pointer $eff0
	jsr	_main
_exit:
	.byte	0xcb	; wai
	jmp	_exit

_wait:
	.byte	0xcb	; wai
	rts

	.export	_sr4
_sr4:
	ldx	#0
	lda	(@sp,x)
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	ldy	#1
	jmp	__addysp

_ei:
	rts
_di:
	rts
_clrs:
	rts
	export _interruptCount
_interruptCount:
	.byte	0
