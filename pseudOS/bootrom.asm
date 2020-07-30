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
    la $a0, name_str
    jal uart_printstring

    # check sd card
    jal sd_present
    beq $v0, $zero, _err_no_sd
    la $a0, sd_detected_str
    jal uart_printstring

    # copy 6 sectors
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

# sd_to_mem(int sector, int mem_start_addr)
sd_to_mem:
    push $ra
    # read sector
    push $a0
    la $a0, sd_read1st_str
    jal uart_printstring
    pop $a0
    #addi $a0, $zero, 0x000
    jal sd_read_sector
    #jal sd_print_block

    # copy sector to memory
    push $a0
    la $a0, sd_copy_str
    jal uart_printstring
    pop $a0

    li $s0, 0
    li $s8, 2048
    lw $s1, sd_data_addr
    move $s2, $a1 # mem_start_addr
_cfs_start:
    lw $t0, 0($s1)
    sll $t0, $t0, 24
    lw $t1, 4($s1)
    sll $t1, $t1, 16
    add $t0, $t0, $t1
    lw $t2, 8($s1)
    sll $t2, $t2, 8
    add $t0, $t0, $t2
    lw $t3, 12($s1)
    sll $t3, $t3, 0
    add $t0, $t0, $t3
    sw $t0, 0($s2)
    addi $s0, $s0, 16
    addi $s1, $s1, 16
    addi $s2, $s2, 4
    beq $s0, $s8, _cfs_end
    j _cfs_start
_cfs_end:
    pop $ra
    jr $ra

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
    sd_detected_str: .asciiz "SD card detected.\r\n"
    .align 2
    sd_notdetected_str: .asciiz "SD card not detected!\r\n"
    .align 2
    sd_read1st_str: .asciiz "Read sector...\r\n"
    .align 2
    sd_copy_str: .asciiz "Copy sector to memory...\r\n"
    .align 2
    sd_boot_str: .asciiz "Transfer control now...\r\n"
    .align 2
    halt_str: .asciiz "halt.\r\n"

    mem_start_addr: .word 0x10000000

