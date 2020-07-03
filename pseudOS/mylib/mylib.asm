.text

.globl isa_set_addr
.globl setupstack

.globl uart_putchar
.globl uart_getchar
.globl led_ctrl

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
# stack has 100 positions
setupstack:
    lw $t0, mem_addr
    addi $sp, $t0, 400
    jr $ra

# void isa_set_addr(int addr)
# make a manual j instruction at isa_handler_addr to our handler
isa_set_addr:
    #addi $t0, $zero, 0
    #lui $t0, 0x0800
    #add $t0, $t0, $a0
    lw $t1, isa_timer_addr
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
