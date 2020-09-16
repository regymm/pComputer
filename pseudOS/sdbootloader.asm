# SD card first sector
# use no external asm
# currently lie in sector 0 to 1
.include "mylib/include.asm"
.section .text.boot

_start:
    lw $t0, gpio_addr
    li $t1, 0
    sw $t1, 28($t0)

    # still use 0x10000000 as stack
    lw $t0, mem_addr
    addi $sp, $t0, 0x3ffc

    la $a0, sd_str
    jal uart_printstring

    # set up main memory: from 16th sector, 128KB
    lw $t0, sdmm_start_sector_addr
    li $t1, 0x10
    sw $t1, 0($t0)
    lw $t0, sdmm_size_addr
    li $t1, 0x100
    sw $t1, 0($t0)

    # copy sectors 2~4 to sdmm
    la $a0, sd_copy_str
    jal uart_printstring
    li $a0, 2
    li $a1, 0x00000000
_copy_sectors:
    jal sd_to_mem
    addi $a0, $a0, 1
    addi $a1, $a1, 0x200
    li $t0, 5
    beq $a0, $t0, _copy_end
    j _copy_sectors
_copy_end:

    # transfer control to main memory(sdmm)
    la $a0, xfer_ctrl_str
    jal uart_printstring
    la $ra, 0x00000000
    jr $ra

_halt:
    j _halt

#void sd_wait_ready(void)
sd_wait_ready:
    lw $t0, sd_ready_addr
    lw $t1, ($t0)
    beq $t1, $zero, sd_wait_ready
    jr $ra
# sd_to_mem(int sector, int mem_start_addr)
sd_to_mem:
    push $ra
    push $s0
    push $s2
    push $s8
    # wait for ready
    jal sd_wait_ready
    # set sector
    lw $t0, sd_set_sector_addr
    sw $a0, ($t0)

    li $s8, 0x200 # max offset(not include)
    li $s0, 0 # sector offset
    move $s2, $a1 # mem_start_addr
_cfs_start:
    # set offset
    lw $t0, sd_set_offset_addr
    sw $s0, ($t0)
    # do read
    lw $t0, sd_do_read_addr
    li $t1, 1
    sw $t1, ($t0)
    # wait for ready
    jal sd_wait_ready
    # get result
    lw $t0, sd_result_addr
    lw $t1, ($t0)
    # to memory
    sw $t1, ($s2)
    addi $s0, $s0, 4
    addi $s2, $s2, 4
    beq $s0, $s8, _cfs_end
    j _cfs_start
_cfs_end:
    pop $s8
    pop $s2
    pop $s0
    pop $ra
    jr $ra

# void uart_putchar(char c)
uart_putchar:
    lw $t2, uart_addr
_uart_putchar_wait1:
    lw $t0, 8($t2)
    beq $t0, $0, _uart_putchar_wait1
    sw $a0, 0($t2) # do the real work
_uart_putchar_wait2:
    lw $t0, 8($t2)
    beq $t0, $0, _uart_putchar_wait2
    jr $ra

# void uart_printstring(char* c)
uart_printstring:
    push $ra
    push $s0
    push $s1
    push $s2
    push $s3
    push $s4
    li $s0, 0
_u_ps_start:
    push $a0
    add $a0, $a0, $s0
    lw $s4, 0($a0) # t1 contains 4 characters
    sll $s1, $s4, 0
    srl $s1, $s1, 24
    sll $s2, $s4, 8
    srl $s2, $s2, 24
    sll $s3, $s4, 16
    srl $s3, $s3, 24
    sll $s4, $s4, 24
    srl $s4, $s4, 24
    beqz $s1, _u_ps_end
    move $a0, $s1
    jal uart_putchar
    beqz $s2, _u_ps_end
    move $a0, $s2
    jal uart_putchar
    beqz $s3, _u_ps_end
    move $a0, $s3
    jal uart_putchar
    beqz $s4, _u_ps_end
    move $a0, $s4
    jal uart_putchar
    pop $a0
    addi $s0, $s0, 4
    j _u_ps_start
_u_ps_end:
    pop $a0
    pop $s4
    pop $s3
    pop $s2
    pop $s1
    pop $s0
    pop $ra
    jr $ra

.data
    .align 2
    sd_str: .asciiz "[sdboot] On SD card now.\r\n"
    .align 2
    sd_copy_str: .asciiz "[sdboot] Copy sectors to Mem...\r\n"
    .align 2
    xfer_ctrl_str: .asciiz "[sdboot] Transfer control to Mem...\r\n"
    mem_addr: .word 0x10000000
    gpio_addr: .word 0x92000000
    uart_addr: .word 0x93000000
    sdmm_start_sector_addr: .word 0x96003000
    sdmm_size_addr: .word 0x96003004
    sd_set_sector_addr: .word 0x96001000
    sd_set_offset_addr: .word 0x96001004
    sd_do_read_addr: .word 0x96001008
    sd_do_write_addr: .word 0x9600100c
    sd_ready_addr: .word 0x96001010
    sd_result_addr: .word 0x96001014
