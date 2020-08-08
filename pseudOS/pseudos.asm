# pseudOS main
.include "mylib/include.asm"
.text

_start:
    # re-setup stack
    jal setupstack

    li $a0, 1
    jal led_ctrl

    la $a0, osstart_str
    jal uart_printstring

    lw $t0, sdmm_start_sector_addr
    li $t1, 0x10
    sw $t1, 0($t0)
    lw $t0, sdmm_size_addr
    li $t1, 0x100
    sw $t1, 0($t0)

    li $t0, 'A'
    sw $t0, 0x00000000
    sw $t0, 0x00000004
    sw $t0, 0x00000008
    sw $t0, 0x0000000c
    lw $a0, 0x00000000
    jal uart_putchar

    li $t0, 'B'
    sw $t0, 0x0001fffc
    lw $a0, 0x0001fffc
    jal uart_putchar

    lw $a0, 0x00000000
    jal uart_putchar

    #lw $a0, 0x0001fffc
    #jal uart_putchar
    ## test sd card write
    #li $a0, 0
    #jal sd_read_sector
    #li $s0, 0
    #li $s8, 2048
    #lw $s1, sd_data_addr
#_sdw:
    #add $s2, $s1, $s0
    #li $t0, 'X'
    #sw $t0, 0($s2)
    #addi $s0, $s0, 4
    #bne $s0, $s8, _sdw

    #lw $s1, sd_do_write_addr
    #li $t0, 1
    #sw $t0, 0($s1)

    #lw $t1, sd_ready_addr
#_sdwait1:
    #lw $t2, 0($t1)
    #beq $t2, $zero, _sdwait1

    #la $a0, done_str
    #jal uart_printstring

    #la $a0, setupint_str
    #jal uart_printstring
    #la $a0, timer_handler
    #jal isa_timer_set_addr
    #jal isa_timer_enable
    la $a0, syscall_handler
    jal isa_syscall_set_addr
    jal isa_syscall_enable

    li $v0, 4
    la $a0, syscall_test_str
    syscall

_end:
    j _end

timer_handler:
    #j timer_handler
    push $ra
    la $a0, timer_int_str
    jal uart_printstring
    pop $ra
    jr $ra

# v0 is the call code, a0 is the parameter
syscall_handler:
    push $ra
    beq $v0, 4, _syscall_printstring
_syscall_printstring:
    jal uart_printstring
    j _syscall_end
_syscall_end:
    pop $ra
    jr $ra

.data
    .align 2
    osstart_str: .asciiz "pseudOS v0.1\r\n"
    .align 2
    setupint_str: .asciiz "Setup interrupt and syscall...\r\n"
    .align 2
    timer_int_str: .asciiz "Timer handler called.\r\n"
    .align 2
    syscall_test_str: .asciiz "This is syscall test string.\r\n"
    .align 2
    done_str: .asciiz "Done.\r\n"
