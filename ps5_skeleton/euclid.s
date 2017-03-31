

.section .rodata
intout: .string "%ld "
strout: .string "%s "
errout: .string "Wrong number of arguments"
STR0: .string "GCD"
STR1: .string "Greatest common divisor of"
STR2: .string "and"
STR3: .string "is"
STR4: .string "and"
STR5: .string "are relative primes"
.section .data
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
	call	_euclid
	jmp END
ABORT:
	movq $errout, %rdi
	call puts
END:
	movq %rax, %rdi
	call exit

.globl _gcd
_gcd:
	pushq	%rsp
	movq	%rsp, %rbp
	subq  	$32, %rsp
	movq  	%rdi, -8(%rbp)
	movq  	%rsi, -16(%rbp)
	movq  	-8(%rbp), %rax
	#assignment
	movq  	%rax, -24(%rbp)
	#print
	movq	$strout, %rdi
	movq  	$STR0, %rsi
	movq	$0, %rax
	call printf
	movq  	-24(%rbp), %rax
	leave
	ret

.globl _euclid
_euclid:
	pushq	%rsp
	movq	%rsp, %rbp
	subq  	$16, %rsp
	movq  	%rdi, -8(%rbp)
	movq  	%rsi, -16(%rbp)
	movq  	-8(%rbp), %rax
	pushq	%rax
	movq  	$0, %rax
	popq	%r8
	#IF_CHECK
	cmpq	%r8, %rax
_IF0:
	#unary minus
	movq  	-8(%rbp), %rax
	negq	%rax
	#assignment
	movq  	%rax, -8(%rbp)
	jmp  	_ENDIF0
_ELSE0:
	jmp  	_ENDIF0

_ENDIF0:
	movq  	-16(%rbp), %rax
	pushq	%rax
	movq  	$0, %rax
	popq	%r8
	#IF_CHECK
	cmpq	%r8, %rax
_IF1:
	#unary minus
	movq  	-16(%rbp), %rax
	negq	%rax
	#assignment
	movq  	%rax, -16(%rbp)
	jmp  	_ENDIF1
_ELSE1:
	jmp  	_ENDIF1

_ENDIF1:
	movq  	$2, %rax
	pushq	%rax
	movq  	$1, %rax
	popq	%r8
	#IF_CHECK
	cmpq	%r8, %rax
	jge  	_ELSE2
	jl  	_IF2

_IF2:
	#print
	movq	$strout, %rdi
	movq  	$STR1, %rsi
	movq	$0, %rax
	call printf
	#print
	movq	$intout, %rdi
	movq  	-8(%rbp), %rsi
	movq	$0, %rax
	call printf
	#print
	movq	$strout, %rdi
	movq  	$STR2, %rsi
	movq	$0, %rax
	call printf
	#print
	movq	$intout, %rdi
	movq  	-16(%rbp), %rsi
	movq	$0, %rax
	call printf
	#print
	movq	$strout, %rdi
	movq  	$STR3, %rsi
	movq	$0, %rax
	call printf
	jmp  	_ENDIF2
_ELSE2:
	#print
	movq	$intout, %rdi
	movq  	-8(%rbp), %rsi
	movq	$0, %rax
	call printf
	#print
	movq	$strout, %rdi
	movq  	$STR4, %rsi
	movq	$0, %rax
	call printf
	#print
	movq	$intout, %rdi
	movq  	-16(%rbp), %rsi
	movq	$0, %rax
	call printf
	#print
	movq	$strout, %rdi
	movq  	$STR5, %rsi
	movq	$0, %rax
	call printf
	jmp  	_ENDIF2

_ENDIF2:
	movq  	$0, %rax
	leave
	ret

