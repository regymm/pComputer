# ISA ROM
# begin at 0x80000000
.include "mylib/include.asm"
.section .text.exception


_start:
    # push
    push $at
    push $v0
    push $v1
    push $a0
    push $a1
    push $a2
    push $a3
    push $t0
    push $t1
    push $t2
    push $t3
    push $t4
    push $t5
    push $t6
    push $t7
    push $s0
    push $s1
    push $s2
    push $s3
    push $s4
    push $s5
    push $s6
    push $s7
    push $t8
    push $t9
    push $gp
    push $sp
    push $fp
    push $ra
    # manually prepare return address to _finish
    # maybe should use $k0 here?
    lui $ra, 0x8000
    addi $ra, $ra, _finish
    mfc0 $t0, $13
    addi $t1, $zero, 8
    beq $t0, $t1, _timer
    addi $t1, $zero, 9
    beq $t0, $t1, _keyboard
    addi $t1, $zero, 1
    beq $t0, $t1, _syscall
_unknown:
    j _unknown
    # in subroutines just use jr $ra to _finish
_timer:
    lw $t0, 0x80001000
    jr $t0
_keyboard:
    lw $t0, 0x80002000
    jr $t0
_syscall:
    lw $t0, 0x80008000
    jr $t0
_finish:
    # pop
    pop $ra
    pop $fp
    pop $sp
    pop $gp
    pop $t9
    pop $t8
    pop $s7
    pop $s6
    pop $s5
    pop $s4
    pop $s3
    pop $s2
    pop $s1
    pop $s0
    pop $t7
    pop $t6
    pop $t5
    pop $t4
    pop $t3
    pop $t2
    pop $t1
    pop $t0
    pop $a3
    pop $a2
    pop $a1
    pop $a0
    pop $v1
    pop $v0
    pop $at
    eret
