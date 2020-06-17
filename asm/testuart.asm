.section .text.boot

_start:
    addi $t0, $0, 1
    addi $t1, $0, 2
    sw $t0, 0x20000028($0)
    sw $t1, 0x2000002c($0)
    addi $t3, $0, 65
    sw $t3, 0x30000000($0)
    addi $t3, $0, 66
    sw $t3, 0x30000000($0)
    addi $t3, $0, 67
    sw $t3, 0x30000000($0)
    addi $t3, $0, 68
    sw $t3, 0x30000000($0)
    j _loop
_loop:
    lw $t1, 0x20000000($0)
    sw $t1, 0x20000018($0)
    lw $s1, 0x30000004($0)
    sw $s1, 0x2000001c($0)
    lw $s1, 0x30000008($0)
    sw $s1, 0x20000020($0)
    lw $s1, 0x3000000c($0)
    sw $s1, 0x20000024($0)
    addi $s2, $0, 1
    beq $t1, $s2, _print
    j _loop
_print:
    addi $t3, $0, 69
    sw $t3, 0x30000000($0)
    j _loop
_fail:
    addi $t0, $0, 7
    addi $t1, $0, 7
    sw $t0, 0x20000028($0)
    sw $t1, 0x2000002b($0)
    j _fail
