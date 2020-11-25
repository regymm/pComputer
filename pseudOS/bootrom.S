.section .text
.globl _start

_start:
	#li t1, 0xdeadbeef
	li t2, 23
	addi t3, zero, 4
	addi t2, t2, 5
	add t4, t2, t3
	sub t5, t2, t3
	beq t5, t2, fail
	ori t6, zero, 1
	bne t6, t5, cont1
	j fail
cont1:
	j end
end:
	j end
	nop
	nop
	nop
	nop
fail:
	j fail


.section .data
msg:
	.string "Hello World\n"
