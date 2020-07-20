.text
_start:
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
