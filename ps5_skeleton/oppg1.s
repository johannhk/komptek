.globl main
.globl factor

.data
str:
	.string "%ld is a prime factor \n"

.text
factor:
	pushq %rbp	#save the basepointer
	movq %rsp, 	%rbp #set new base pointer
	subq $16, 	%rsp#allocate memory in stack pointer
	movq %rdi, -8(%rbp)#n is first in stack
	sarq $1, 	%rdi #shifts n right once
	movq %rdi, -16(%rbp)#f is second in stack
	#movq -16(%rbp), %rdi


loop:
	movq $0, 	%rdx #to divide buffer
	movq -8(%rbp), %rax # move n to divide buffer
	idivq -16(%rbp) # divide rdx:rax(n) by f
	cmpq $0, 	%rdx # compare divide result with 0
	je if
	decq -16(%rbp)#decrementing f after if
	jmp loop


if:
	cmpq $1, -16(%rbp)
	je else
	movq  -16(%rbp), %rdi
	call factor
	
	movq $0, 	%rdx #move 0 to divide buffer MS
	movq -8(%rbp), %rax # move n to divide buffer LS
	idivq -16(%rbp) #divide f by n
	movq %rax, %rdi
	call factor
	jmp return

else:

	movq $str, %rdi #put string as argument 0
	movq -8(%rbp), %rsi #put n in argument 1
	call printf
	jmp return
	
return:
	leave
	ret

main:
	pushq %rbp	#save the basepointer
	movq %rsp, 	%rbp #set new base pointer
	movq $1836311903, %rdi
	call factor
	jmp return
