.include "mylib/include.asm"
.text

.globl setupstack

.globl isa_timer_set_addr
.globl isa_timer_enable
.globl isa_keyboard_set_addr
.globl isa_keyboard_enable
.globl isa_syscall_set_addr
.globl isa_syscall_enable

.globl uart_putchar
.globl uart_getchar
.globl uart_printstring
.globl led_ctrl
.globl triled_ctrl

.globl sd_read_sector
.globl sd_print_block
.globl sd_present

.globl mem_addr
.globl gpio_addr
.globl uart_addr
.globl isa_timer_addr
.globl isa_timer_mask
.globl isa_keyboard_addr
.globl isa_keyboard_mask
.globl isa_syscall_addr
.globl isa_syscall_mask
.globl sd_data_addr
.globl sd_addr_addr
.globl sd_do_read_addr
.globl sd_do_write_addr
.globl sd_ncd_addr
.globl sd_wp_addr
.globl sd_ready_addr
.globl sdmm_start_sector_addr
.globl sdmm_size_addr

#.extern mem_addr

# void setupstack()
# stack begin from 0x10003ffc (16KB)
setupstack:
    lw $t0, mem_addr
    addi $sp, $t0, 0x3ffc
    jr $ra

# void isa_timer_set_addr(int addr)
isa_timer_set_addr:
    lw $t1, isa_timer_addr
    sw $a0, 0($t1)
    jr $ra
isa_timer_enable:
    lw $t1, isa_timer_mask
    sw $zero, 0($t1)
    jr $ra
isa_keyboard_set_addr:
    lw $t1, isa_keyboard_addr
    sw $a0, 0($t1)
    jr $ra
isa_keyboard_enable:
    lw $t1, isa_keyboard_mask
    sw $zero, 0($t1)
    jr $ra
isa_syscall_set_addr:
    lw $t1, isa_syscall_addr
    sw $a0, 0($t1)
    jr $ra
isa_syscall_enable:
    lw $t1, isa_syscall_mask
    sw $zero, 0($t1)
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

# char uart_getchar()
uart_getchar:
    lw $t2, uart_addr
    addi $t1, $0, 1
    sw $t1, 4($t2)
_loop_uart_getchar:
    lw $t0, 4($t2)
    beq $t0, $0, _loop_uart_getchar
    lw $t0, 0($t2)
    addi $v0, $t0, 0
    jr $ra

# void led_ctrl(int a)
led_ctrl:
    lw $t2, gpio_addr
    addi $t1, $t2, 24
    sw $a0, 0($t1)
    addi $t1, $t2, 28
    sw $a0, 0($t1)
    addi $t1, $t2, 32
    sw $a0, 0($t1)
    addi $t1, $t2, 36
    sw $a0, 0($t1)
    jr $ra

triled_ctrl:
    lw $t2, gpio_addr
    addi $t1, $t2, 40
    sw $a0, 0($t1)
    addi $t1, $t2, 44
    sw $a0, 0($t1)
    jr $ra

#int sd_present(void)
sd_present:
    lw $t0, sd_ncd_addr
    lw $t1, 0($t0)
    beq $t1, $zero, _sd_present
    j _sd_notpresent
_sd_notpresent:
    addi $v0, $zero, 0
    jr $ra
_sd_present:
    addi $v0, $zero, 1
    jr $ra

# void sd_read_sector(int sector_addr)
sd_read_sector:
    # wait for ready
    lw $t1, sd_ready_addr
_sd_read_sector_wait1:
    lw $t2, 0($t1)
    beq $t2, $zero, _sd_read_sector_wait1
    # now ready
    lw $t1, sd_addr_addr
    sw $a0, 0($t1)
    # address set
    addi $t0, $zero, 1
    lw $t1, sd_do_read_addr
    sw $t0, 0($t1)
    # read command sent
    ## wait for not ready
    #lw $t1, sd_ready_addr
#_sd_read_sector_nwait2:
    #lw $t2, 0($t1)
    #beq $t2, $t0, _sd_read_sector_nwait2
    # and wait for ready again
    lw $t1, sd_ready_addr
_sd_read_sector_wait3:
    lw $t2, 0($t1)
    beq $t2, $zero, _sd_read_sector_wait3
    jr $ra

# void sd_print_block()
sd_print_block:
    push $ra
    lw $t1, sd_data_addr
    addi $t0, $zero, 0
    addi $t4, $zero, 2048 # 512 * 4
_sd_print_block_loop:
    add $t2, $t0, $t1
    lw $a0, 0($t2)
    push $t0
    push $t1
    push $t4
    jal uart_putchar
    pop $t4
    pop $t1
    pop $t0
    addi $t0, $t0, 4
    beq $t0, $t4, _sd_print_block_end
    j _sd_print_block_loop
_sd_print_block_end:
    pop $ra
    jr $ra

.data
    mem_addr: .word 0x10000000
    gpio_addr: .word 0x92000000
    uart_addr: .word 0x93000000
    isa_timer_addr: .word 0x80001000
    isa_timer_mask: .word 0x80001004
    isa_keyboard_addr: .word 0x80002000
    isa_keyboard_mask: .word 0x80002004
    isa_syscall_addr: .word 0x80008000
    isa_syscall_mask: .word 0x80008004
    sd_data_addr: .word 0x96000000
    sd_addr_addr: .word 0x96001000
    sd_do_read_addr: .word 0x96001004
    sd_do_write_addr: .word 0x96001008
    sd_ncd_addr: .word 0x96002000
    sd_wp_addr: .word 0x96002004
    sd_ready_addr: .word 0x96002010
    sdmm_start_sector_addr: .word 0x96003000
    sdmm_size_addr: .word 0x96003004
