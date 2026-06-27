	.code
	.export	_wait
	.export	__tmp
	.export	__hireg
	.export	_interruptCount
	.export	_interruptHandle
	.export	_di
	.export	_ei

	jmp	start
	jmp	intr
start:
	ld	t,=0
	ld	sp,=0xefef	; initial stack pointer
	ld	p3,=0
; clear bss
	ld	p2,=__bss
l2:
	ld	ea,p2
	sub	ea,=__end
	or	a,e
	bz	l1
	ld	a,=0
	st	a,@1,p2
	bra	l2
;
l1:
	.byte 0x3b,1	;or s,=1
	jsr _main
	.byte	4	;halt (emulator)

intr:
	push	ea
	ld	a,s
	push	a
	nop
	nop
;	ild	a,:_interruptCount
	ld	ea,:_interruptHandle
	or	a,e
	bz	intr1
	ld	ea,t
	push	ea
	push	p2
	push	p3
	ld	ea,:__tmp
	push	ea
	ld	ea,=intr2-1
	push	ea	; return address
	ld	ea,:_interruptHandle
	add	ea,=0xffff
	ld	pc,ea
intr2:
	pop	ea
	st	ea,:__tmp
	pop	p3
	pop	p2
	pop	ea
	ld	t,ea
intr1:
	pop	a
	ld	s,a
	pop	ea
	.byte 0x3b,1	;or s,=1
	ret

_di:
	.byte	0x39,0xfe	;and s,=0xfe
	ret

_ei:
	.byte	0x3b,1	;or s,=1
	ret

_wait:
	.byte	5
;	.byte	0	;compare test
	ret

	.dp
__hireg:
	.ds	2
__tmp:
	.ds	2
_interruptCount:
	.ds	1
_interruptHandle:
	.word	0
