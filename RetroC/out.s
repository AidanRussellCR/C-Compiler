	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq    %rsp, %rbp
	.comm	x,8,8
	.comm	y,8,8
	movq	$6, %r8
	movq	%r8, x(%rip)
	movq	$12, %r8
	movq	%r8, y(%rip)
	movq	x(%rip), %r8
	movq	y(%rip), %r9
	cmpq	%r9, %r8
	je	L1
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	jmp	L2
L1:
	movq	y(%rip), %r8
	movq	%r8, %rdi
	call	printint
L2:
	movl $0, %eax
	popq	%rbp
	ret
