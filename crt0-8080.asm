	.export _wait
	.export _ei
	.export _di
	.abs
	.org	0
	di
	lxi	sp,0efefh
	lxi	d,__bss_size
	mov	a,e
	ora	d
	jz	nobss
	lxi	h,0

	mov	a,l
	sub	e
	mov	l,a
	mov	a,h
	sbb	d
	mov	h,a		; sbc	hl,de
	push	h
	push	d
	pop	h
	pop	d		;ex	de,hl
	lxi	h,__bss
	xra	a
clear_bss:
	mov	m,a
	inx	h
	inr	e
	jnz	clear_bss
	inr	d
	jnz	clear_bss
nobss:
	call	_main
_exit:
	hlt
	jmp	_exit

	.org	38h
	push	psw
	push	h
	lxi	h,_interruptCount
	inr	m
	lda	_interruptHandle
	mov	l,a
	lda	_interruptHandle+1
	mov	h,a			; ld	hl,(_interruptHandle)
	mov	a,l
	ora	h
	jz	l1
	push	b
	push	d
	lxi	d,l2
	push	d	; return address
	push	h
	ret	;jp	(hl)
l2:
	pop	d
	pop	b
l1:
	pop	h
	pop	psw
_ei:
	ei
	ret

_di:
	di
	ret

_wait:
	hlt
	ret

	.bss
	.export	_interruptCount
	.export	_interruptHandle
_interruptCount:
	.byte	0
_interruptHandle:
	.word	0
