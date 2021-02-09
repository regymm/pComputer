.section .text.boot

# C functions
.globl sd_c_start
.globl interrupt_service_routine

# ASM functions
.globl isr_asm
.globl csrr_mstatus
.globl csrw_mstatus
.globl csrr_mie
.globl csrw_mie
.globl csrr_mtvec
.globl csrw_mtvec
.globl csrr_mcause
.globl csrw_mcause
.globl csrr_mscratch
.globl csrw_mscratch

.globl crit_enter
.globl crit_leave

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

csrr_mstatus:
	csrr a0, mstatus
	ret
csrw_mstatus:
	csrw mstatus, a0
	ret
csrr_mcause:
	csrr a0, mcause
	ret
csrw_mcause:
	csrw mcause, a0
	ret
csrr_mie:
	csrr a0, mie
	ret
csrw_mie:
	csrw mie, a0
	ret
csrr_mtvec:
	csrr a0, mtvec
	ret
csrw_mtvec:
	csrw mtvec, a0
	ret
csrr_mscratch:
	csrr a0, mscratch
	ret
csrw_mscratch:
	csrw mscratch, a0
	ret
# critical zone management
crit_enter:
	ret
crit_leave:
	ret

# interrupt/exception jumps here
isr_asm:
#	csrrw a0, mscratch, a0
#	sw a1, 0(a0)
#	sw a2, 4(a0)
#	sw a3, 8(a0)
#	sw a4, 12(a0)
#	
#	csrr a1, mcause
#	bgez a1, e
	csrrw s0, mscratch, s0
	sw a1, 0(s0)
	sw a2, 4(s0)
	sw a3, 8(s0)
	sw a4, 12(s0)
	sw a5, 16(s0)
	sw a6, 20(s0)
	sw a7, 24(s0)

	sw a0, 28(s0)

	sw s1, 32(s0)
	sw s2, 36(s0)
	sw s3, 40(s0)
	sw s4, 44(s0)
	sw s5, 48(s0)
	sw s6, 52(s0)
	sw s7, 56(s0)
	sw s8, 60(s0)
	sw s9, 64(s0)
	sw s10, 68(s0)
	sw s11, 72(s0)

	sw t0, 78(s0)
	sw t1, 80(s0)
	sw t2, 84(s0)
	sw t3, 88(s0)
	sw t4, 92(s0)
	sw t5, 96(s0)
	sw t6, 100(s0)

	sw ra, 104(s0)
	sw sp, 108(s0)
	sw gp, 112(s0)
	sw tp, 116(s0)

	# callee saved s0

	jal interrupt_service_routine

	lw s1, 0(s0)
	lw s2, 4(s0)
	lw s3, 8(s0)
	lw s4, 12(s0)
	lw s5, 16(s0)
	lw s6, 20(s0)
	lw s7, 24(s0)

	lw a0, 28(s0)

	lw s1, 32(s0)
	lw s2, 36(s0)
	lw s3, 40(s0)
	lw s4, 44(s0)
	lw s5, 48(s0)
	lw s6, 52(s0)
	lw s7, 56(s0)
	lw s8, 60(s0)
	lw s9, 64(s0)
	lw s10, 68(s0)
	lw s11, 72(s0)

	lw t0, 76(s0)
	lw t1, 80(s0)
	lw t2, 84(s0)
	lw t3, 88(s0)
	lw t4, 92(s0)
	lw t5, 96(s0)
	lw t6, 100(s0)

	lw ra, 104(s0)
	lw sp, 108(s0)
	lw gp, 112(s0)
	lw tp, 116(s0)
	csrrw s0, mscratch, s0
	mret
