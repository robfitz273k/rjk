#
# Copyright (C) 2000 Robert Fitzsimons
#

	.code32

	.balign	0x08
.global	_start
_start:
	cli
	cmpl	$0x2BADB002, %eax
0:	jne	0b
	movl	$0x00006000, %esp
	call	setup_pic
	pushl	%ebx
	call	setup
	addl	$4, %esp
	lgdt	gdtr
	lidt	idtr
	movl	$page_table_directory, %eax
	movl	%eax, %cr3
	movl	%cr0, %eax
	orl	$0x80000001, %eax
	movl	%eax, %cr0
	ljmp	$0x0010, $.Lpm

	.balign	0x08
.Lpm:
	movw	$0x0018, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss
	movl	$0x00006000, %esp
	call	init
0:	jmp	0b

	.balign	0x08
setup_pic:
	movb	$0x11, %al
	outb	%al, $0x20
	jmp	0f
0:	movb	$0x20, %al
	outb	%al, $0x21
	jmp	0f
0:	movb	$0x04, %al
	outb	%al, $0x21
	jmp	0f
0:	movb	$0x01, %al
	outb	%al, $0x21
	jmp	0f
0:	movb	$0xFF, %al
	outb	%al, $0x21
	jmp	0f
0:	movb	$0x11, %al
	outb	%al, $0xA0
	jmp	0f
0:	movb	$0x28, %al
	outb	%al, $0xA1
	jmp	0f
0:	movb	$0x04, %al
	outb	%al, $0xA1
	jmp	0f
0:	movb	$0x01, %al
	outb	%al, $0xA1
	jmp	0f
0:	movb	$0xFF, %al
	outb	%al, $0xA1
	jmp	0f
0:	ret

	.macro	ISR_NULL
	.balign	0x10
	iret
	.endm

	.macro	ISR_EXCEPTION handler stack
	.balign	0x10
	.if	\stack
	pushl	$0
	.endif
	pusha
	.byte	0x6A, ((. - .Lisr_exception) / 0x10)
	call	\handler
	addl	$8, %esp
	popa
	iret
	.endm

	.macro	ISR_IRQ handler pic2
	.balign	0x10
	pusha
	.byte	0x6A, ((. - .Lisr_irq) / 0x10)
	call	handle_irq
	.if	\pic2
	outb	%al, $0xA0
	.endif
	outb	%al, $0x20
	popl	%eax
	popa
	iret
	.endm

	.balign	0x10
.global	isr
isr:
.Lisr_exception:
	ISR_EXCEPTION handle_exception 1 /* 0 Divide Error */
	ISR_EXCEPTION handle_exception 1 /* 1 Debug Exceptions */
	ISR_EXCEPTION handle_exception 1 /* 2 NMI Interrupt */
	ISR_EXCEPTION handle_exception 1 /* 3 Breakpoint */
	ISR_EXCEPTION handle_exception 1 /* 4 Overflow */
	ISR_EXCEPTION handle_exception 1 /* 5 BOUND Range Exceeded */
	ISR_EXCEPTION handle_exception 1 /* 6 Invalid Opcode */
	ISR_EXCEPTION handle_exception 1 /* 7 Device Not Available (FPU) */
	ISR_EXCEPTION handle_exception 0 /* 8 Double Fault */
	ISR_EXCEPTION handle_exception 1 /* 9 Coprocessor Segment Overrun */
	ISR_EXCEPTION handle_exception 0 /* 10 Invalid TSS */
	ISR_EXCEPTION handle_exception 0 /* 11 Segment Not Present */
	ISR_EXCEPTION handle_exception 0 /* 12 Stack-Segment Fault */
	ISR_EXCEPTION handle_exception 0 /* 13 General Protection */
	ISR_EXCEPTION handle_exception 0 /* 14 Page Fault */
	ISR_NULL /* 15 */
	ISR_EXCEPTION handle_exception 1 /* 16 Floating-Point Error */
	ISR_EXCEPTION handle_exception 0 /* 17 Alignment Check */
	ISR_EXCEPTION handle_exception 1 /* 18 Machine Check */
	ISR_EXCEPTION handle_exception 1 /* 19 Streaming SIMD Extensions */
	ISR_NULL /* 20 */
	ISR_NULL /* 21 */
	ISR_NULL /* 22 */
	ISR_NULL /* 23 */
	ISR_NULL /* 24 */
	ISR_NULL /* 25 */
	ISR_NULL /* 26 */
	ISR_NULL /* 27 */
	ISR_NULL /* 28 */
	ISR_NULL /* 29 */
	ISR_NULL /* 30 */
	ISR_NULL /* 31 */
	.balign	0x10
.Lisr_irq:
	ISR_IRQ handle_irq 0 /* IRQ 0 */
	ISR_IRQ handle_irq 0 /* IRQ 1 */
	ISR_IRQ handle_irq 0 /* IRQ 2 */
	ISR_IRQ handle_irq 0 /* IRQ 3 */
	ISR_IRQ handle_irq 0 /* IRQ 4 */
	ISR_IRQ handle_irq 0 /* IRQ 5 */
	ISR_IRQ handle_irq 0 /* IRQ 6 */
	ISR_IRQ handle_irq 0 /* IRQ 7 */
	ISR_IRQ handle_irq 1 /* IRQ 8 */
	ISR_IRQ handle_irq 1 /* IRQ 9 */
	ISR_IRQ handle_irq 1 /* IRQ 10 */
	ISR_IRQ handle_irq 1 /* IRQ 11 */
	ISR_IRQ handle_irq 1 /* IRQ 12 */
	ISR_IRQ handle_irq 1 /* IRQ 13 */
	ISR_IRQ handle_irq 1 /* IRQ 14 */
	ISR_IRQ handle_irq 1 /* IRQ 15 */

	.balign	0x08
multiboot_header:
	.long	0x1BADB002	# magic
	.long	0x00000003	# flags
	.long	(0x00000000 - (0x00000003 + 0x1BADB002))	# checksum

	.data

	.balign	0x08
.global	idt
idt:
	.rept	48
	.word	0x0000, 0x0010, 0x8E00, 0x0000
	.endr

	.balign	0x08
.global	gdt
gdt:
	.word	0x0000, 0x0000, 0x0000, 0x0000
	.word	0x0000, 0x0000, 0x0000, 0x0000
	.word	0xFFFF, 0x0000, 0x9A00, 0x00CF
	.word	0xFFFF, 0x0000, 0x9200, 0x00CF

	.balign 0x08
	.word	0x0000
idtr:
	.word	((256 * 8) - 1)
	.long	idt

	.balign 0x08
	.word	0x0000
gdtr:
	.word	((4 * 8) - 1)
	.long	gdt

