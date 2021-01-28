.section .text.boot
.globl sd_c_start

sd_start:
	la a0, 0x92000000
	li a1, 0
	sw a1, 24(a0)
	sw a1, 28(a0)
	sw a1, 32(a0)
	sw a1, 36(a0)

	la sp, 0x200ffffc
	j sd_c_start
	j end
end:
	la a0, 0x92000000
	li a1, 1
	#sw a1, 24(a0)
	sw a1, 28(a0)
	#sw a1, 32(a0)
	sw a1, 36(a0)
	j end
