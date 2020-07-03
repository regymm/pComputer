# Boot ROM
.include "mylib/include.asm"
.section .text.boot

_start:
    jal setupstack
    #addi $a0, $zero, 'O'
    #jal uart_putchar
    #addi $a0, $zero, 'K'
    #jal uart_putchar
    addi $a0, $zero, isa_handler
    jal isa_set_addr
    ##lw $s0, mem_addr
    addi $t0, $zero, 0
_end:
    addi $t0, $t0, 1
    addi $a0, $zero, 0
    jal led_ctrl
    j _end

# the real interrupt/exception handler
isa_handler:
    #addi $t0, $zero, 0
    #addi $t1, $zero, 0
    #addi $t2, $zero, 0
    addi $a0, $zero, 1
    push $ra
    jal led_ctrl
    pop $ra
    #addi $a0, $zero, 'I'
    #jal uart_putchar
    #addi $a0, $zero, 'S'
    #jal uart_putchar
    #addi $a0, $zero, 'A'
    #jal uart_putchar
    jr $ra
    #eret

.data 
    mem_addr: .word 0x10000000
