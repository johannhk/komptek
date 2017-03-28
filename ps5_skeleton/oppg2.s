

.section .rodata
intout: .string "%ld "
strout: .string "%s "
errout: .string "Wrong number of arguments"
STR0: .string "argument 1 (a) is "
STR1: .string "argument 2 (b) is "
STR2: .string "(a^2-b^2)/-b is"
STR3: .string "\nReturning result into shell env. variable '$?'"
STR4: .string "'echo $?' displays last return value (as unsigned byte).\n"
.section .data
_x: .zero 8
_z: .zero 8
_y: .zero 8
.globl main
.section .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $1, %rdi
	cmpq	$2,%rdi
	jne ABORT
	cmpq $0, %rdi
	jz SKIP_ARGS
	movq %rdi, %rcx
	addq $16, %rsi
PARSE_ARGV:
	pushq %rcx
	pushq %rsi
	movq (%rsi), %rdi
	movq $0, %rsi
	movq $10, %rdx
	call strtol
	popq %rsi
	popq %rcx
	pushq %rax
	subq $8, %rsi
	loop PARSE_ARGV
	popq	%rdi
	popq	%rsi
SKIP_ARGS:
	call	_f
	jmp END
ABORT:
	movq $errout, %rdi
	call puts
END:
	movq %rax, %rdi
	call exit
.globl _f
_f:
	pushq	%rsp
	movq	%rsp, %rbp
	subq  	$16, %rsp
	movq  	%rdi, -8(%rbp)
	movq  	%rsi, -16(%rbp)
	#assignment
	movq  	-8(%rbp), %rax
	movq  	%rax, _x
	#assignment
	movq  	-16(%rbp), %rax
	movq  	%rax, _y
	#print
	movq  	%rax, %rdi
	movq	$strout, %rdi
	movq  	$STR0, %rsi
	movq	$0, %rax
	call printf
	#print
	movq  	%rax, %rdi
	movq	$intout, %rdi
	movq  	-8(%rbp), %rsi
	movq  	-8(%rbp), %rax
	movq	$intout, %rdi
	movq	%rax, %rdi
	movq	$intout, %rdi
	movq  	$97, %rsi

	movq	$0, %rax
	call printf
	#print
	movq  	%rax, %rdi
	movq	$strout, %rdi
	movq  	$STR1, %rsi
	movq	$0, %rax
	call printf
	#print
	movq  	%rax, %rdi
	movq	$intout, %rdi
	movq  	-16(%rbp), %rsi
	movq  	-16(%rbp), %rax
	movq	$intout, %rdi
	movq	%rax, %rdi
	movq	$intout, %rdi
	movq  	$98, %rsi

	movq	$0, %rax
	call printf
	#print
	movq  	%rax, %rdi
	movq	$strout, %rdi
	movq  	$STR2, %rsi
	movq	$0, %rax
	call printf
	#print
	movq  	%rax, %rdi
	movq  	_x, %rax
	pushq	%rax
	movq  	_y, %rax
	popq	%r8
	#addition
	addq	%r8, %rax
	pushq	%rax
	movq  	_x, %rax
	pushq	%rax
	movq  	_y, %rax
	popq	%r8
	#subtraction
	xchg	%rax, %r8
	subq	%r8, %rax
	popq	%r8
	#multiplication
	cqo
	imulq	%r8
	pushq	%rax
	#unary minus
	negq  	_y
	popq	%r8
	#division
	xchg	%rax, %r8
	cqo
	idivq	%r8
	movq	$intout, %rdi
	movq	%rax, %rdi
	movq	$intout, %rdi
	movq  	$47, %rsi

	movq	$0, %rax
	call printf
	#assignment
	movq  	_x, %rax
	pushq	%rax
	movq  	_y, %rax
	popq	%r8
	#addition
	addq	%r8, %rax
	pushq	%rax
	movq  	_x, %rax
	pushq	%rax
	movq  	_y, %rax
	popq	%r8
	#subtraction
	xchg	%rax, %r8
	subq	%r8, %rax
	popq	%r8
	#multiplication
	cqo
	imulq	%r8
	pushq	%rax
	#unary minus
	negq  	_y
	popq	%r8
	#division
	xchg	%rax, %r8
	cqo
	idivq	%r8
	movq  	%rax, _z
	#print
	movq  	%rax, %rdi
	movq	$strout, %rdi
	movq  	$STR3, %rsi
	movq	$0, %rax
	call printf
	#print
	movq  	%rax, %rdi
	movq	$strout, %rdi
	movq  	$STR4, %rsi
	movq	$0, %rax
	call printf
	movq  	_z, %rax
	leave
	ret
	movq	$0, %rax
	call exit
