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
.globl csrr_mepc
.globl csrw_mepc

.globl crit_enter
.globl crit_leave

.equ REGS_SAVE_ADDR, 0x10000000

sd_start:
	la a0, 0x92000000
	li a1, 0
	sw a1, 24(a0)
	sw a1, 28(a0)
	sw a1, 32(a0)
	sw a1, 36(a0)

	# stack
	la sp, 0x200ffffc
	j sd_c_start
	# never here
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
csrr_mepc:
	csrr a0, mepc
	ret
csrw_mepc:
	csrw mepc, a0
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
#	csrrw s0, mscratch, s0
	csrw mscratch, s0
	la s0, REGS_SAVE_ADDR
	sw a0, 0(s0)
	sw a1, 4(s0)
	sw a2, 8(s0)
	sw a3, 12(s0)
	sw a4, 16(s0)
	sw a5, 20(s0)
	sw a6, 24(s0)
	sw a7, 28(s0)

	la a0, REGS_SAVE_ADDR # a0 changed here, but already saved!
	csrr s0, mscratch
	sw s0, 32(a0)
	la s0, REGS_SAVE_ADDR

#	sw s0, 32(a0)
	sw s1, 36(s0)
	sw s2, 40(s0)
	sw s3, 44(s0)
	sw s4, 48(s0)
	sw s5, 52(s0)
	sw s6, 56(s0)
	sw s7, 60(s0)
	sw s8, 64(s0)
	sw s9, 68(s0)
	sw s10, 72(s0)
	sw s11, 76(s0)

	sw t0, 80(s0)
	sw t1, 84(s0)
	sw t2, 88(s0)
	sw t3, 92(s0)
	sw t4, 96(s0)
	sw t5, 100(s0)
	sw t6, 104(s0)

	sw ra, 108(s0)
	sw sp, 112(s0)
	sw gp, 116(s0)
	sw tp, 120(s0)

	# still on old stack
	jal interrupt_service_routine
	# regs on REGS_SAVE_ADDR may already changed(proc switch)!
	# so no re-entrant
	# should use current stack to support re-entrant

	lw a0, 0(s0)
	lw a1, 4(s0)
	lw a2, 8(s0)
	lw a3, 12(s0)
	lw a4, 16(s0)
	lw a5, 20(s0)
	lw a6, 24(s0)
	lw a7, 28(s0)

	#	lw s0, 32(s0) TODO: make sure s0 works fine
	lw s1, 36(s0)
	lw s2, 40(s0)
	lw s3, 44(s0)
	lw s4, 48(s0)
	lw s5, 52(s0)
	lw s6, 56(s0)
	lw s7, 60(s0)
	lw s8, 64(s0)
	lw s9, 68(s0)
	lw s10, 72(s0)
	lw s11, 76(s0)

	lw t0, 80(s0)
	lw t1, 84(s0)
	lw t2, 88(s0)
	lw t3, 92(s0)
	lw t4, 96(s0)
	lw t5, 100(s0)
	lw t6, 104(s0)

	lw ra, 108(s0)
	lw sp, 112(s0)
	lw gp, 116(s0)
	lw tp, 120(s0)

	# restore s0 at last
	lw s0, 32(s0)
#	csrrw s0, mscratch, s0
	mret
