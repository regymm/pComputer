.text
_start:
    lw $t1, isa_timer_addr
    la $t0, timer_handler 
    sw $t0, 0($t1)
    lw $t1, isa_timer_mask
    sw $zero, 0($t1)
_end2:
    li $t0, 20
    j _end2


    li $t4, 0x70436f6d
    sll $t1, $t4, 0
    srl $t1, $t1, 12
    sll $t2, $t4, 4
    srl $t2, $t2, 12
    sll $t3, $t4, 8
    srl $t3, $t3, 12
    sll $t4, $t4, 12
    srl $t4, $t4, 12

    addi $t0, $zero, 5
    addi $t1, $zero, 9
    add $t2, $t0, $t1 # 14
    addu $t3, $t0, $t1 # 14
    addi $t0, $zero, 3 # 3
    sub $t2, $t1, $t0 # 6
    and $t2, $t1, $t0 # 1
    or $t2, $t1, $t0 # 11
    xor $t2, $t2, $t0 # 8

    addi $t0, $zero, -2
    addi $t1, $zero, 2
    slt $t2, $t1, $t0 # 0
    sltu $t2, $t1, $t0 # 1
    sltiu $t2, $t0, 10 # 0
    slti $t2, $t0, 10 # 1

    li $t0, 8
    li $t1, 1
    sll $t2, $t0, 2
    srl $t2, $t0, 2
    sllv $t2, $t0, $t1
    srlv $t2, $t0, $t1
    #addiu $t0, $zero, 45
    #lui $t0, 45
    #move $t2, $t0
    #mul $t2, $t0, $t9
_end:
    j _end
timer_handler:
    li $t0, 100
    jr $ra
.data
    isa_timer_addr: .word 0x80001000
    isa_timer_mask: .word 0x80001004

