# Boot ROM
.include "mylib/include.asm"
.section .text.boot

_start:
    addi $a0, $zero, 0
    jal led_ctrl
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
    li $a0, name_str
    jal uart_printstring

    # check sd card
    jal sd_present
    beq $v0, $zero, _err_no_sd
    li $a0, sd_detected_str
    jal uart_printstring

    # read first sector
    li $a0, sd_read1st_str
    jal uart_printstring
    addi $a0, $zero, 0x000
    jal sd_read_sector
    #jal sd_print_block

    # copy first sector to memory
    li $a0, sd_copy_str
    jal uart_printstring
    addi $t0, $zero, 0
    addi $t9, $zero, 2048
    lw $t1, sd_data_addr
    lw $t2, mem_start_addr
_cfs_start:
    lw $t3, 0($t1)
    sll $t3, $t3, 0
    lw $t4, 4($t1)
    sll $t4, $t4, 4
    add $t3, $t3, $t4
    lw $t3, 8($t1)
    sll $t3, $t3, 8
    add $t3, $t3, $t4
    lw $t3, 12($t1)
    sll $t3, $t3, 12
    add $t3, $t3, $t4
    sw $t3, 0($t2)
    addi $t0, $t0, 4
    addi $t1, $t1, 16
    addi $t2, $t2, 4
    beq $t0, $t9, _cfs_end
    j _cfs_start
_cfs_end:

    # transfer control
    li $a0, sd_boot_str
    jal uart_printstring
    lw $t0, mem_start_addr
    addi $ra, $t0, 0
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
    li $a0, halt_str
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
    name_str: .asciiz "pComputer bootrom v1.0\r\n"
    sd_detected_str: .asciiz "SD card detected.\r\n"
    sd_notdetected_str: .asciiz "SD card not detected!\r\n"
    sd_read1st_str: .asciiz "Read first sector...\r\n"
    sd_copy_str: .asciiz "Copy first sector to memory...\r\n"
    sd_boot_str: .asciiz "Transfer control now...\r\n"
    halt_str: .asciiz "halt.\r\n"

    mem_start_addr: .word 0x10000000

