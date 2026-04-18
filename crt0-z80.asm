	.export _wait
	.export _ei
	.export _di
	.abs
	.org	0
	di
	ld	sp,0efefh
	ld	de,__bss_size
	ld	a,e
	or	d
	jr	z,nobss
	ld	hl,0
	xor	a
	sbc	hl,de
	ex	de,hl
	ld	hl,__bss
clear_bss:
	ld	(hl),a
	inc	hl
	inc	e
	jr	nz,clear_bss
	inc	d
	jr	nz,clear_bss
nobss:
	call	_main
_exit:
	halt
	jr	_exit

	.org	38h
	push	af
	push	hl
	ld	hl,_interruptCount
	inc	(hl)
	ld	hl,(_interruptHandle)
	ld	a,l
	or	h
	jr	z,l1
	push	bc
	push	de
	push	ix
	push	iy
	ld	de,l2
	push	de	; return address
	jp	(hl)
l2:
	pop	iy
	pop	ix
	pop	de
	pop	bc
l1:
	pop	hl
	pop	af
_ei:
	ei
	ret

_di:
	di
	ret

_wait:
	halt
	ret

	.bss
	.export	_interruptCount
	.export	_interruptHandle
_interruptCount:
	.byte	0
_interruptHandle:
	.word	0
