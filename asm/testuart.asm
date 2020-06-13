.section .text.boot

_start:
    addi $t0, $0, 1
    addi $t1, $0, 2
    sw $t0, 0x00002028($0)
    sw $t1, 0x0000202c($0)
    addi $t3, $0, 65
    sw $t3, 0x00003000($0)
    addi $t3, $0, 66
    sw $t3, 0x00003000($0)
    addi $t3, $0, 67
    sw $t3, 0x00003000($0)
    addi $t3, $0, 68
    sw $t3, 0x00003000($0)
    j _loop
_loop:
    lw $t1, 0x00002000($0)
    sw $t1, 0x00002018($0)
    lw $s1, 0x00003004($0)
    sw $s1, 0x0000201c($0)
    lw $s1, 0x00003008($0)
    sw $s1, 0x00002020($0)
    lw $s1, 0x0000300c($0)
    sw $s1, 0x00002024($0)
    addi $s2, $0, 1
    beq $t1, $s2, _print
    j _loop
_print:
    addi $t3, $0, 69
    sw $t3, 0x00003000($0)
    j _loop
_fail:
    addi $t0, $0, 7
    addi $t1, $0, 7
    sw $t0, 0x00002028($0)
    sw $t1, 0x0000202b($0)
    j _fail
