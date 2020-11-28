.section .text
.globl _start

_start:
	# arithmetic tests
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
	li a0, 1
	li a1, 2
	xor a3, a0, a1
	li a4, 3
	bne a3, a4, fail

	li a0, 0xf
	li a1, 0xfe
	and a3, a0, a1
	li a4, 0xe
	bne a3, a4, fail

	li a0, 2
	li a1, 3
	slt a3, a0, a1
	li a4, 1
	bne a3, a4, fail

	li a0, -2
	li a1, 2
	slt a3, a0, a1
	li a4, 1
	bne a3, a4, fail
	sltu a3, a0, a1
	beq a3, a4, fail

	li a0, 0xf0000000
	li a1, 3
	sll a3, a0, a1
	li a4, 0x80000000
	bne a3, a4, fail

	li a0, 0xf0000000
	li a1, 4
	srl a3, a0, a1
	li a4, 0x0f000000
	bne a3, a4, fail
	sra a3, a0, a1
	li a4, 0xff000000
	bne a3, a4, fail

	# jump test
	jal func_dummy

	# load store tests
	li a1, 0xabcdf2f4
	la a0, 0x10000000
	sw a1, 0(a0)
	sh a1, 4(a0)
	sb a1, 8(a0)
	lw a3, 0(a0)
	li a4, 0xabcdf2f4
	bne a3, a4, fail
	lw a3, 4(a0)
	li a4, 0x0000f2f4
	bne a3, a4, fail
	lw a3, 8(a0)
	li a4, 0x000000f4
	bne a3, a4, fail

	lhu a3, 0(a0)
	li a4, 0x0000f2f4
	bne a3, a4, fail
	lbu a3, 0(a0)
	li a4, 0x000000f4
	bne a3, a4, fail
	lh a3, 0(a0)
	li a4, 0xfffff2f4
	bne a3, a4, fail
	lb a3, 0(a0)
	li a4, 0xfffffff4
	beq a3, a4, cont2
	j fail
cont2:
	# RV32M tests
	li a0, 0xabcd1234
	li a1, 0x7777ffff
	mul a3, a0, a1
	mulh a4, a0, a1
	mulhsu a5, a0, a1
	mulhu a6, a0, a1
	li s3, 0x0892edcc
	li s4, 0xd8b4db97
	li s5, 0xd8b4db97
	li s6, 0x502cdb96
	bne a3, s3, fail
	bne a4, s4, fail
	bne a5, s5, fail
	bne a6, s6, fail

	li a0, 0xcbcd1234
	li a1, 0x1777ffff
	div a3, a0, a1
	divu a4, a0, a1
	rem a5, a0, a1
	remu a6, a0, a1
	li s3, 0xfffffffe
	li s4, 0x00000008
	li s5, 0xabcd1234
	li s6, 0x100d123c
	bne a3, s3, fail
	bne a4, s4, fail
	bne a5, s5, fail
	bne a6, s6, fail

	la a0, 0x92000000
	li a1, 0
	sw a1, 24(a0)
	sw a1, 28(a0)
	li a1, 2
	sw a1, 40(a0)
	li a1, 1
	sw a1, 44(a0)



	li a0, 'O'
	jal uart_putchar
	li a0, 'K'
	jal uart_putchar
	li a0, '\n'
	jal uart_putchar
	j end
end:
	j end
	nop
	nop
	nop
	nop
	nop
	nop
fail:
	j fail

func_dummy:
	ret
	j fail


# void uart_putchar(char c)
uart_putchar:
    lw t2, uart_addr
1:
    lw t0, 8(t2)
    beq t0, zero, 1b
    sw a0, 0(t2) # do the real work
2:
    lw t0, 8(t2)
    beq t0, zero, 2b
	ret

.section .data
    uart_addr: .word 0x93000000
