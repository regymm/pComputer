.macro push reg 
    addi $sp, $sp, -4
    sw \reg, ($sp)
.endm

.macro pop reg
    lw \reg, ($sp)
    addi $sp, $sp, 4
.endm

