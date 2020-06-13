.section .text.boot

_start:
    addi $t0, $0, 3
    addi $t1, $0, 7
    sw $t0, 0x00002028($0)
    sw $t1, 0x0000202c($0)
    j _loop
_loop:
    lw $s1, 0x00002000($0)
    lw $s2, 0x00002004($0)
    lw $s3, 0x00002008($0)
    lw $s4, 0x0000200c($0)
    sw $s1, 0x00002018($0)
    sw $s2, 0x0000201c($0)
    sw $s3, 0x00002020($0)
    sw $s4, 0x00002024($0)
    j _loop
_fail:
    addi $t0, $0, 1
    addi $t1, $0, 2
    sw $t0, 0x00002028($0)
    sw $t1, 0x0000202b($0)
    j _fail
