.include "mylib/include.asm"
.text

.globl setupstack

.globl isa_timer_set_addr
.globl isa_keyboard_set_addr
.globl isa_syscall_set_addr

.globl uart_putchar
.globl uart_getchar
.globl led_ctrl

.globl sd_read_sector
.globl sd_print_block

.globl mem_addr
.globl gpio_addr
.globl uart_addr
.globl isa_timer_addr
.globl isa_timer_mask
.globl isa_keyboard_addr
.globl isa_keyboard_mask
.globl isa_syscall_addr
.globl isa_syscall_mask

# void setupstack()
# stack has 200 positions
setupstack:
    lw $t0, mem_addr
    addi $sp, $t0, 800
    jr $ra

# void isa_timer_set_addr(int addr)
isa_timer_set_addr:
    lw $t1, isa_timer_addr
    sw $a0, 0($t1)
    jr $ra
isa_keyboard_set_addr:
    lw $t1, isa_keyboard_addr
    sw $a0, 0($t1)
    jr $ra
isa_syscall_set_addr:
    lw $t1, isa_syscall_addr
    sw $a0, 0($t1)
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

# void gpio_addr(int a)
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
    # wait for not ready
    lw $t1, sd_ready_addr
_sd_read_sector_nwait2:
    lw $t2, 0($t1)
    beq $t2, $t0, _sd_read_sector_nwait2
    # and wait for ready again
_sd_read_sector_wait3:
    lw $t2, 0($t1)
    beq $t2, $zero, _sd_read_sector_nwait2
    jr $ra

# void sd_print_block()
sd_print_block:
    push $ra
    lw $t1, sd_data_addr
    addi $t0, $zero, 0
    addi $t4, $zero, 2048 # 512 * 4
_sd_print_block_loop:
    add $t2, $t0, $t1
    lw $t3, 0($t2) # the result
    andi $a0, $t3, 0xF
    push $t0
    push $t1
    push $t3
    push $t4
    jal uart_putchar # print the low 4 bit
    pop $t4
    pop $t3
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
    gpio_addr: .word 0x20000000
    uart_addr: .word 0x30000000
    isa_timer_addr: .word 0x80001000
    isa_timer_mask: .word 0x80001004
    isa_keyboard_addr: .word 0x80002000
    isa_keyboard_mask: .word 0x80002004
    isa_syscall_addr: .word 0x80008000
    isa_syscall_mask: .word 0x80008004
    sd_data_addr: .word 0x60000000
    sd_addr_addr: .word 0x60001000
    sd_do_read_addr: .word 0x60001004
    sd_do_write_addr: .word 0x60001008
    sd_ready_addr: .word 0x60002010
