# Boot ROM
# use mylib/mylib.asm
.include "mylib/include.asm"
.section .text.boot

_start:
    lw $t0, gpio_addr
    li $t1, 0
    sw $t1, 24($t0)
    #addi $a0, $zero, 0
    #jal led_ctrl

    addi $a0, $zero, 'O'
    jal uart_putchar
    addi $a0, $zero, 'K'
    jal uart_putchar
    addi $a0, $zero, '\r'
    jal uart_putchar
    addi $a0, $zero, '\n'
    jal uart_putchar

    jal setupstack

    # print welcome
    la $a0, name_str
    jal uart_printstring

    # check sd card
    jal sd_present
    beq $v0, $zero, _err_no_sd
    la $a0, sd_detected_str
    jal uart_printstring

    # copy 6 sectors
    la $a0, sd_copy_str
    jal uart_printstring

    li $a0, 0
    lw $a1, mem_start_addr
    jal sd_to_mem
    li $a0, 1
    addi $a1, $a1, 0x200
    jal sd_to_mem
    li $a0, 2
    addi $a1, $a1, 0x200
    jal sd_to_mem
    li $a0, 3
    addi $a1, $a1, 0x200
    jal sd_to_mem
    li $a0, 4
    addi $a1, $a1, 0x200
    jal sd_to_mem
    li $a0, 5
    addi $a1, $a1, 0x200
    jal sd_to_mem



    # transfer control
    la $a0, sd_boot_str
    jal uart_printstring
    lw $ra, mem_start_addr
    jr $ra


    #addi $a0, $zero, isa_timer_handler
    #jal isa_timer_set_addr
    #addi $a0, $zero, isa_keyboard_handler
    #jal isa_keyboard_set_addr
_end:
    addi $a0, $zero, 1
    jal led_ctrl
    j _end

_err_no_sd:
    li $a0, sd_notdetected_str
    jal uart_printstring
    j _halt

_halt:
    la $a0, halt_str
    jal uart_printstring
    addi $a0, $zero, 1
    jal triled_ctrl
_halt_end:
    j _halt_end

#isa_timer_handler:
    #addi $a0, $zero, 1
    #push $ra
    #jal led_ctrl
    #addi $a0, $zero, 'I'
    #jal uart_putchar
    #addi $a0, $zero, 'S'
    #jal uart_putchar
    #addi $a0, $zero, 'A'
    #jal uart_putchar
    #pop $ra
    #jr $ra
    ##eret

#isa_keyboard_handler:
    ##push $ra
    ##jal uart_putchar
    ##pop $ra
    #jr $ra
.data
    .align 2
    name_str: .asciiz "pComputer bootrom v1.0\r\n"
    .align 2
    sd_detected_str: .asciiz "[bootrom] SD card detected.\r\n"
    .align 2
    sd_notdetected_str: .asciiz "[bootrom] SD card not detected!\r\n"
    .align 2
    sd_copy_str: .asciiz "[bootrom] Copy 6 sectors to memory...\r\n"
    .align 2
    sd_boot_str: .asciiz "[bootrom] Transfer control now...\r\n"
    .align 2
    halt_str: .asciiz "[bootrom] halt.\r\n"

    mem_start_addr: .word 0x10000000

