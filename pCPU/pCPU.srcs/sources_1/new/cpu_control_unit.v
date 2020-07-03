`timescale 1ns / 1ps
// pComputer multi-cycle CPU -- control unit

module control_unit
    (
        input clk,
        input rst,
        input [31:0]instruction,
        input ALUZero,
        input [31:0]status,
        input irq,
        output reg iack,

        output reg PCWrite,
        output reg NewInstr,
        output reg IorD,
        output reg MemRead,
        output reg MemWrite,
        output reg [2:0]RegSrc,
        output reg IRWrite,
        output reg [2:0]PCSource,
        output reg [2:0]ALUm,
        output reg ALUSrcA,
        output reg [1:0]ALUSrcB,
        output reg RegWrite,
        output reg [1:0]RegDst,

        output reg EPCWrite,
        output reg EPCSrc,
        output reg CauseWrite,
        output reg CauseSrc,
        output reg StatusWrite,
        output reg StatusSrc,
        output reg [1:0]Mfc0Src
    );

    // control unit FSM state names
    reg [7:0]phase = IF;
    localparam IF = 0;
    localparam ID_RF = 1;
    localparam MEM_ADDR_CALC = 2;
    localparam MEM_ACCESS_LW = 3;
    localparam WB = 4; 
    localparam MEM_ACCESS_SW = 5;
    localparam ADDI_END = 12;
    localparam R_EX = 6;
    localparam R_END = 7;
    localparam BEQ_END = 8;
    localparam J_END = 9;
    localparam JAL_END = 10;
    localparam JR_END = 11;
    localparam LUI_END = 13;
    localparam I_MFC0_END = 14;
    //localparam I_MTC0_END = 15;
    localparam I_ERET_END = 16;
    localparam I_SYSCALL_END = 17;
    localparam I_INT_END = 18;
    localparam I_EXCPTN_END = 19;
    localparam BAD = 99;

    // instruction[31:26] instruction type
    wire [5:0]instr_type = instruction[31:26];
    localparam TYPE_REG = 6'b000000;
    localparam TYPE_ADDI = 6'b001000;
    localparam TYPE_LUI = 6'b001111;
    localparam TYPE_LW = 6'b100011;
    localparam TYPE_SW = 6'b101011;
    localparam TYPE_BEQ = 6'b000100;
    localparam TYPE_J = 6'b000010;
    localparam TYPE_JAL = 6'b000011;
    localparam TYPE_INT = 6'b010000;
    localparam TYPE_BAD = 0;

    // instruction[5:0] function
    wire [5:0]instr_funct = instruction[5:0];
    localparam FUNCT_ADD = 6'b100000;
    localparam FUNCT_SUB = 6'b100010;
    localparam FUNCT_AND = 6'b100100;
    localparam FUNCT_OR = 6'b100101;
    localparam FUNCT_SLT = 6'b101010;
    localparam FUNCT_JR = 6'b001000;
    localparam FUNCT_SYSCALL = 6'b001100;

    // instruction label
    reg [31:0]Op;
    localparam OP_ADD = 91001;
    localparam OP_SUB = 91001;
    localparam OP_AND = 91001;
    localparam OP_OR = 91001;
    localparam OP_SLT = 91001;
    localparam OP_ADDI = 90002;
    localparam OP_LUI = 90003;
    localparam OP_LW = 90004;
    localparam OP_SW = 90005;
    localparam OP_BEQ = 90006;
    localparam OP_J = 90007;
    localparam OP_JAL = 90008;
    localparam OP_JR = 90009;
    localparam OP_MFC0 = 90010;
    //localparam OP_MTC0 = 90011;
    localparam OP_ERET = 90012;
    localparam OP_SYSCALL = 90013;
    localparam OP_NOP = 91000;
    localparam OP_BAD = 99000;

    // instruction decoding
    always @ (*) begin
        Op = OP_BAD;
        case (instr_type)
            TYPE_REG: case (instr_funct)
                FUNCT_ADD: Op = OP_ADD;
                FUNCT_SUB: Op = OP_SUB;
                FUNCT_AND: Op = OP_AND;
                FUNCT_OR: Op = OP_OR;
                FUNCT_SLT: Op = OP_SLT;
                FUNCT_JR: Op = OP_JR;
                FUNCT_SYSCALL: Op = OP_SYSCALL;
                default: ;
            endcase
            TYPE_ADDI: Op = OP_ADDI;
            TYPE_LUI: Op = OP_LUI;
            TYPE_LW: Op = OP_LW;
            TYPE_SW: Op = OP_SW;
            TYPE_BEQ: Op = OP_BEQ;
            TYPE_J: Op = OP_J;
            TYPE_JAL: Op = OP_JAL;
            TYPE_INT: case (instruction[25:21])
                5'b10000: Op = OP_ERET;
                5'b00000: Op = OP_MFC0;
                //5'b00100: Op = OP_MTC0;
                default: ;
            endcase
            default: ;
        endcase
        if (instruction == 32'b0) Op = OP_NOP;
    end

    // control fsm
    always @ (posedge clk) begin
        if (rst) begin
            phase <= IF;
        end
        else begin
            case(phase)
                IF: phase <= ID_RF;
                ID_RF: begin
                    if (status[3] & irq) phase <= I_INT_END;
                    else case(Op)
                        OP_NOP: phase <= IF;

                        OP_LW: phase <= MEM_ADDR_CALC;
                        OP_SW: phase <= MEM_ADDR_CALC;
                        OP_ADDI: phase <= MEM_ADDR_CALC;
                        OP_LUI: phase <= LUI_END;

                        OP_ADD: phase <= R_EX;
                        OP_SUB: phase <= R_EX;
                        OP_AND: phase <= R_EX;
                        OP_OR: phase <= R_EX;

                        OP_BEQ: phase <= BEQ_END;

                        OP_J: phase <= J_END;
                        OP_JAL: phase <= JAL_END;
                        OP_JR: phase <= JR_END;

                        OP_MFC0: phase <= I_MFC0_END;
                        //OP_MTC0: phase <= I_MTC0_END;
                        OP_ERET: phase <= I_ERET_END;
                        OP_SYSCALL: begin
                            if (status[0]) phase <= I_SYSCALL_END;
                            else phase <= IF;
                        end
                        default: phase <= BAD;
                    endcase
                end
                MEM_ADDR_CALC: case (Op)
                    OP_LW: phase <= MEM_ACCESS_LW;
                    OP_SW: phase <= MEM_ACCESS_SW;
                    OP_ADDI: phase <= ADDI_END;
                    default: phase <= BAD;
                endcase
                MEM_ACCESS_LW: phase <= WB;
                WB: phase <= IF;
                MEM_ACCESS_SW: phase <= IF;
                ADDI_END: phase <= IF;
                LUI_END: phase <= IF;
                R_EX: phase <= R_END;
                R_END: phase <= IF;
                BEQ_END: phase <= IF;
                J_END: phase <= IF;
                JAL_END: phase <= IF;
                JR_END: phase <= IF;
                I_MFC0_END: phase <= IF;
                //I_MTC0_END: phase <= IF;
                I_ERET_END: phase <= IF;
                I_SYSCALL_END: phase <= IF;
                I_INT_END: phase <= IF;
                default: phase <= BAD;
            endcase
        end
    end

    // control signals for each FSM states
    always @ (*) begin
        PCWrite = 0;
        NewInstr = 0;
        IorD = 0;
        MemRead = 0;
        MemWrite = 0;
        RegSrc = 3'b000;
        IRWrite = 0;
        PCSource = 3'b000;
        ALUm = 3'b000;
        ALUSrcA = 0;
        ALUSrcB = 0;
        RegWrite = 0;
        RegDst = 2'b00;
        EPCWrite = 0;
        EPCSrc = 0;
        CauseWrite = 0;
        CauseSrc = 0;
        StatusWrite = 0;
        StatusSrc = 0;
        Mfc0Src = 2'b0;
        iack = 0;
        case (phase)
            IF: begin
                MemRead = 1;
                IRWrite = 1;
                PCWrite = 1;
                NewInstr = 1;
                ALUSrcB = 2'b01;
            end
            ID_RF: begin
                ALUSrcB = 2'b11;
            end
            MEM_ADDR_CALC: begin
                ALUSrcA = 1; ALUSrcB = 2'b10;
            end
            MEM_ACCESS_LW: begin
                MemRead = 1; IorD = 1;
            end
            ADDI_END: begin
                RegWrite = 1;
            end
            LUI_END: begin
                RegWrite = 1; RegSrc = 3'b010;
            end
            WB: begin
                RegWrite = 1; RegSrc = 3'b001;
            end
            MEM_ACCESS_SW: begin
                MemWrite = 1; IorD = 1;
            end
            R_EX: begin
                ALUSrcA = 1;
                case (Op)
                    OP_ADD: ALUm = 3'b000;
                    OP_SUB: ALUm = 3'b001;
                    OP_AND: ALUm = 3'b010;
                    OP_OR: ALUm = 3'b011;
                endcase
            end
            R_END: begin
                RegWrite = 1; RegDst = 2'b01;
            end
            BEQ_END: begin
                ALUSrcA = 1; ALUm = 3'b001;
                PCWrite = ALUZero; PCSource = 3'b001;
            end
            J_END: begin
                PCWrite = 1; PCSource = 3'b010;
            end
            JAL_END: begin
                PCWrite = 1; PCSource = 3'b010;
                RegWrite = 1; RegDst = 2'b10; RegSrc = 3'b011;
            end
            JR_END: begin
                PCWrite = 1; PCSource = 3'b011;
            end
            I_MFC0_END: begin
                RegWrite = 1; RegSrc = 3'b100;
                case (instruction[15:11])
                    12: Mfc0Src = 2;
                    13: Mfc0Src = 1;
                    14: Mfc0Src = 0;
                endcase
            end
            //I_MTC0_END: begin ; end
            I_ERET_END: begin
                PCWrite = 1; PCSource = 3'b100;
                StatusWrite = 1; StatusSrc = 1;
            end
            I_SYSCALL_END: begin
                PCWrite = 1; PCSource = 3'b101;
                CauseWrite = 1; CauseSrc = 1;
                StatusWrite = 1; StatusSrc = 0;
                EPCWrite = 1; EPCSrc = 0;
            end
            I_INT_END: begin
                PCWrite = 1; PCSource = 3'b101;
                CauseWrite = 1; CauseSrc = 0;
                StatusWrite = 1; StatusSrc = 0;
                EPCWrite = 1; EPCSrc = 1;
                iack = 1;
            //I_EXCPTN_END: begin
                //EPCWrite = 1; PCWrite = 1; PCSource = 3'b101; CauseWrite = 1; CauseSrc = 2; StatusWrite = 1; StatusSrc = 0; iack = 1; 
            end
        endcase
    end
endmodule
