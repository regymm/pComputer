`timescale 1ns / 1ps
// pComputer multi-cycle CPU -- control unit

module control_unit
    (
        input clk,
        input rst,
        input [31:0]instruction,
        input ALUZero,
        input ALUCf,
        input ALUOf,
        input ALUSf,
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
        output reg Cmp,

        output reg EPCWrite,
        output reg EPCSrc,
        output reg CauseWrite,
        output reg CauseSrc,
        output reg StatusWrite,
        output reg StatusSrc,
        output reg [1:0]Mfc0Src
    );

    // control unit FSM state names (values doesn't matter)
    reg [7:0]phase = IF;
    localparam IF = 0;
    localparam ID_RF = 1;
    localparam R_EX = 2;
    localparam R_END = 3;
    localparam CMP_END = 4;
    localparam LUI_END = 5;
    localparam MEM_ADDR_CALC = 10;
    localparam MEM_ACCESS_LW = 11;
    localparam WB = 12; 
    localparam MEM_ACCESS_SW = 13;
    localparam CALCI_EX = 20;
    localparam CALCI_END = 21;
    localparam CMPI_END = 22;
    localparam BEQ_END = 30;
    localparam BNE_END = 31;
    localparam J_END = 32;
    localparam JAL_END = 33;
    localparam JR_END = 34;
    localparam I_MFC0_END = 35;
    //localparam I_MTC0_END = 36;
    localparam I_ERET_END = 37;
    localparam I_SYSCALL_END = 38;
    localparam I_INT_END = 39;
    localparam I_EXCPTN_END = 40;
    localparam BAD = 99;

    // instruction[31:26] instruction type
    wire [5:0]instr_type = instruction[31:26];
    localparam TYPE_REG = 6'b000000;
    localparam TYPE_J = 6'b000010;
    localparam TYPE_JAL = 6'b000011;
    localparam TYPE_BEQ = 6'b000100;
    localparam TYPE_BNE = 6'b000101;
    localparam TYPE_BLEZ = 6'b000110; // todo
    localparam TYPE_BGTZ = 6'b000111; // todo
    localparam TYPE_ADDI = 6'b001000;
    localparam TYPE_ADDIU = 6'b001001;
    localparam TYPE_SLTI = 6'b001010; // todo
    localparam TYPE_SLTIU = 6'b001011; // todo
    localparam TYPE_ANDI = 6'b001100;
    localparam TYPE_ORI = 6'b001101;
    localparam TYPE_XORI = 6'b001110;
    localparam TYPE_LUI = 6'b001111;
    localparam TYPE_INT = 6'b010000;
    localparam TYPE_LW = 6'b100011;
    localparam TYPE_SW = 6'b101011;
    localparam TYPE_BAD = 0;

    // instruction[5:0] function
    wire [5:0]instr_funct = instruction[5:0];
    localparam FUNCT_JR = 6'b001000;
    localparam FUNCT_SYSCALL = 6'b001100;
    localparam FUNCT_ADD = 6'b100000;
    localparam FUNCT_ADDU = 6'b100001;
    localparam FUNCT_SUB = 6'b100010;
    localparam FUNCT_SUBU = 6'b100011;
    localparam FUNCT_AND = 6'b100100;
    localparam FUNCT_OR = 6'b100101;
    localparam FUNCT_XOR = 6'b100110;
    localparam FUNCT_NOR = 6'b100111; // not on todo list
    localparam FUNCT_SLT = 6'b101010;
    localparam FUNCT_SLTU = 6'b101011;

    // instruction label (values doesn't matter)
    reg [31:0]Op;
    localparam OP_ADD = 91001;
    localparam OP_SUB = 91002;
    localparam OP_AND = 91003;
    localparam OP_OR = 91004;
    localparam OP_XOR = 91005;
    localparam OP_SLT = 91006;
    localparam OP_SLTU = 91007;
    //
    localparam OP_ADDI = 92001;
    localparam OP_ANDI = 92002;
    localparam OP_ORI = 92003;
    localparam OP_XORI = 92004;
    localparam OP_LUI = 92005;
    localparam OP_SLTI = 92006;
    localparam OP_SLTIU = 92007;
    //
    localparam OP_LW = 90009;
    localparam OP_SW = 90010;
    localparam OP_BEQ = 90011;
    localparam OP_BNE = 90012;
    localparam OP_J = 90013;
    localparam OP_JAL = 90014;
    localparam OP_JR = 90015;
    localparam OP_MFC0 = 90016;
    //localparam OP_MTC0 = 90017;
    localparam OP_ERET = 90018;
    localparam OP_SYSCALL = 90019;
    localparam OP_NOP = 98000;
    localparam OP_BAD = 99000;

    // instruction decoding
    always @ (*) begin
        Op = OP_BAD;
        case (instr_type)
            TYPE_REG: case (instr_funct)
                FUNCT_JR: Op = OP_JR;
                FUNCT_SYSCALL: Op = OP_SYSCALL;
                FUNCT_ADD: Op = OP_ADD;
                FUNCT_ADDU: Op = OP_ADD;
                FUNCT_SUB: Op = OP_SUB;
                FUNCT_SUBU: Op = OP_SUB;
                FUNCT_AND: Op = OP_AND;
                FUNCT_OR: Op = OP_OR;
                FUNCT_XOR: Op = OP_XOR;
                FUNCT_SLT: Op = OP_SLT;
                default: ;
            endcase
            TYPE_J: Op = OP_J;
            TYPE_JAL: Op = OP_JAL;
            TYPE_BEQ: Op = OP_BEQ;
            TYPE_BNE: Op = OP_BNE;
            TYPE_ADDI: Op = OP_ADDI;
            TYPE_ADDIU: Op = OP_ADDI;
            TYPE_SLTI: Op = OP_SLTI;
            TYPE_SLTIU: Op = OP_SLTIU;
            TYPE_ANDI: Op = OP_ANDI;
            TYPE_ORI: Op = OP_ORI;
            TYPE_XORI: Op = OP_XORI;
            TYPE_LUI: Op = OP_LUI;
            TYPE_LW: Op = OP_LW;
            TYPE_SW: Op = OP_SW;
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

                        OP_ADD: phase <= R_EX;
                        OP_SUB: phase <= R_EX;
                        OP_AND: phase <= R_EX;
                        OP_OR: phase <= R_EX;
                        OP_XOR: phase <= R_EX;
                        OP_SLT: phase <= R_EX;
                        OP_SLTU: phase <= R_EX;

                        OP_LW: phase <= MEM_ADDR_CALC;
                        OP_SW: phase <= MEM_ADDR_CALC;
                        OP_ADDI: phase <= CALCI_EX;
                        OP_ANDI: phase <= CALCI_EX;
                        OP_ORI: phase <= CALCI_EX;
                        OP_XORI: phase <= CALCI_EX;
                        OP_SLTI: phase <= CALCI_EX;
                        OP_SLTIU: phase <= CALCI_EX;
                        OP_LUI: phase <= LUI_END;

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
                    default: phase <= BAD;
                endcase
                MEM_ACCESS_LW: phase <= WB;
                WB: phase <= IF;
                MEM_ACCESS_SW: phase <= IF;
                LUI_END: phase <= IF;
                R_EX: case (Op)
                    OP_SLT: phase <= CMP_END;
                    OP_SLTU: phase <= CMP_END;
                    default: phase <= R_END;
                endcase
                R_END: phase <= IF;
                JR_END: phase <= IF;
                J_END: phase <= IF;
                JAL_END: phase <= IF;
                BEQ_END: phase <= IF;
                BNE_END: phase <= IF;
                CALCI_EX: case (Op)
                    OP_SLTI: phase <= CMPI_END;
                    OP_SLTIU: phase <= CMPI_END;
                    default: phase <= CALCI_END;
                endcase
                CALCI_END: phase <= IF;
                CMPI_END: phase <= IF;
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
        Cmp = 0;
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
            CALCI_EX: begin
                ALUSrcA = 1; ALUSrcB = 2'b10;
                case (Op)
                    OP_ADDI: ALUm = 3'b000;
                    OP_ANDI: ALUm = 3'b010;
                    OP_ORI: ALUm = 3'b011;
                    OP_XORI: ALUm = 3'b100;
                    OP_SLTI: ALUm = 3'b001;
                    OP_SLTIU: ALUm = 3'b001;
                endcase
            end
            CALCI_END: begin
                RegWrite = 1;
            end
            CMPI_END: begin
                RegWrite = 1; RegSrc = 3'b110;
                case (Op)
                    OP_SLTI: Cmp = (ALUOf ^ ALUSf) & !ALUZero;
                    OP_SLTIU: Cmp = ALUCf;
                    default: ;
                endcase
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
                    OP_XOR: ALUm = 3'b100;
                    OP_SLT: ALUm = 3'b001;
                    OP_SLTU: ALUm = 3'b001;
                endcase
            end
            R_END: begin
                RegWrite = 1; RegDst = 2'b01;
            end
            CMP_END: begin
                RegWrite = 1; RegDst = 2'b01; RegSrc = 3'b110;
                case (Op) 
                    OP_SLT: Cmp = (ALUOf ^ ALUSf) & !ALUZero;
                    OP_SLTU: Cmp = ALUCf;
                    default: ;
                endcase
            end
            BEQ_END: begin
                ALUSrcA = 1; ALUm = 3'b001;
                PCWrite = ALUZero; PCSource = 3'b001;
            end
            BNE_END: begin
                ALUSrcA = 1; ALUm = 3'b001;
                PCWrite = !ALUZero; PCSource = 3'b001;
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
