# ISA ROM
# begin at 0x80000000
.include "mylib/include.asm"
.section .text.exception


_start:
    # push
    push $t0
    push $t1
    push $t2
    push $a0
    push $ra
    # manually prepare return address to _finish
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
    pop $a0
    pop $t2
    pop $t1
    pop $t0
    eret
