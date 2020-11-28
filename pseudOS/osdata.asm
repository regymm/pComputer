.globl osstart_str
.globl setupint_str
.globl timer_int_str
.globl syscall_test_str

.globl mem_addr
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

.data
    .align 2
    osstart_str: .asciiz "pseudOS v0.1\r\n"
    .align 2
    setupint_str: .asciiz "Setup interrupt and syscall...\r\n"
    .align 2
    timer_int_str: .asciiz "Timer handler called.\r\n"
    .align 2
    syscall_test_str: .asciiz "This is syscall test string.\r\n"
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
    sd_ncd_addr: .word 0x60002000
    sd_wp_addr: .word 0x60002004
    sd_ready_addr: .word 0x60002010
