# Boot ROM
.include "mylib/include.asm"
.section .text.boot

_start:
    addi $a0, $zero, 1
    jal led_ctrl
    addi $a0, $zero, 'O'
    jal uart_putchar
    addi $a0, $zero, 'K'
    jal uart_putchar

    jal setupstack

    addi $a0, $zero, 0x200
    jal sd_read_sector
    jal sd_print_block

    addi $a0, $zero, '\r'
    jal uart_putchar
    addi $a0, $zero, '\n'
    jal uart_putchar

    addi $a0, $zero, 0x200
    jal sd_read_sector
    jal sd_print_block

    addi $a0, $zero, '\r'
    jal uart_putchar
    addi $a0, $zero, '\n'
    jal uart_putchar

    addi $a0, $zero, 0x400
    jal sd_read_sector
    jal sd_print_block
    #addi $a0, $zero, isa_timer_handler
    #jal isa_timer_set_addr
    #addi $a0, $zero, isa_keyboard_handler
    #jal isa_keyboard_set_addr
_end:
    addi $a0, $zero, 1
    jal led_ctrl
    j _end

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
