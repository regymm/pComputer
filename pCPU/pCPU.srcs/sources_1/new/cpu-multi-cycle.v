`timescale 1ns / 1ps
// pComputer multi-cycle CPU

module cpu_multi_cycle
    (
        input clk,
        input rst,

        output reg [31:0]a,
        output reg [31:0]d,
        output reg we,
        input [31:0]spo
    );

    reg [31:0]instruction = 0;
    reg [31:0]pc = 0;

    reg [31:0]mdr = 0;

    reg [31:0]ALUOut = 0;
    reg [31:0]A = 0;
    reg [31:0]B = 0;

    // control signals
    reg PCWriteCond;
    reg PCWrite;
    reg IorD;
    reg MemRead;
    reg MemWrite;
    reg [1:0]RegSrc;
    reg IRWrite;
    reg [1:0]PCSource;
    reg [2:0]ALUm;
    reg ALUSrcA;
    reg [1:0]ALUSrcB;
    reg RegWrite;
    reg [1:0]RegDst;

    wire [31:0]imm = {{16{instruction[15]}}, instruction[15:0]};

    reg [31:0]newpc;

    reg [4:0]WriteRegister;
    reg [31:0]WriteData;
    wire [31:0]ReadData1;
    wire [31:0]ReadData2;
    register_file register_file_inst
    (
        .clk(clk),
        .ra0(instruction[25:21]),
        .ra1(instruction[20:16]),
        .wa(WriteRegister),
        .we(RegWrite),
        .wd(WriteData),
        .rd0(ReadData1),
        .rd1(ReadData2)
    );

    reg [31:0]mem_addr;
    reg [31:0]MemData;
    // connect to memory mapper
    always @ (*) begin
        a = mem_addr;
        d = B;
        we = MemWrite;
        MemData = spo;
    end

    reg [31:0]ALUIn1;
    reg [31:0]ALUIn2;
    wire [31:0]ALUResult;
    wire ALUZero;
    alu alu_inst
    (
        .m(ALUm),
        .a(ALUIn1),
        .b(ALUIn2),
        .y(ALUResult),
        .zf(ALUZero)
        //.cf(),
        //.of()
    );
    
    // control unit FSM state names
    reg [3:0]phase = IF;
    localparam IF = 0;
    localparam ID_RF = 1;
    localparam MEM_ADDR_CALC = 2;
    localparam MEM_ACCESS_LW = 3;
    localparam WB = 4; 
    localparam MEM_ACCESS_SW = 5;
    localparam R_EX = 6;
    localparam R_END = 7;
    localparam BEQ_END = 8;
    localparam J_END = 9;
    localparam JAL_END = 10;
    localparam JR_END = 11;
    localparam ADDI_END = 12;
    localparam LUI_END = 13;
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
    localparam TYPE_BAD = 0;

    // instruction[5:0] function
    wire [5:0]instr_funct = instruction[5:0];
    localparam FUNCT_ADD = 6'b100000;
    localparam FUNCT_SUB = 6'b100010;
    localparam FUNCT_AND = 6'b100100;
    localparam FUNCT_OR = 6'b100101;
    localparam FUNCT_SLT = 6'b101010;
    localparam FUNCT_JR = 6'b001000;

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
    localparam OP_BAD = 99000;

    // instruction decoding: for signal generation and fsm movement
    // to determine Op
    always @ (*) begin
        case (instr_type)
            TYPE_REG: case (instr_funct)
                FUNCT_ADD: Op = OP_ADD;
                FUNCT_SUB: Op = OP_SUB;
                FUNCT_AND: Op = OP_AND;
                FUNCT_OR: Op = OP_OR;
                FUNCT_SLT: Op = OP_SLT;
                FUNCT_JR: Op = OP_JR;
                default: ;
            endcase
            TYPE_ADDI: Op = OP_ADDI;
            TYPE_LUI: Op = OP_LUI;
            TYPE_LW: Op = OP_LW;
            TYPE_SW: Op = OP_SW;
            TYPE_BEQ: Op = OP_BEQ;
            TYPE_J: Op = OP_J;
            TYPE_JAL: Op = OP_JAL;
            default: Op = OP_BAD;
        endcase
    end

    // control fsm
    always @ (posedge clk) begin
        if (rst) begin
            phase <= IF;
        end
        else begin
            case(phase)
                IF: phase <= ID_RF;
                ID_RF: 
                    case(Op)
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
                        default: phase <= BAD;
                    endcase
                MEM_ADDR_CALC:
                    case (Op)
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
                default: phase <= BAD;
            endcase
        end
    end

    // control signals for each FSM states
    wire PCwe = (ALUZero & PCWriteCond) | PCWrite;
    always @ (*) begin
        PCWriteCond = 0;
        PCWrite = 0;
        IorD = 0;
        MemRead = 0;
        MemWrite = 0;
        RegSrc = 2'b00;
        IRWrite = 0;
        PCSource = 2'b00;
        ALUm = 3'b000;
        ALUSrcA = 0;
        ALUSrcB = 0;
        RegWrite = 0;
        RegDst = 2'b00;
        case (phase)
            IF: begin MemRead = 1; ALUSrcB = 2'b01; IRWrite = 1; PCWrite = 1; end
            ID_RF: ALUSrcB = 2'b11;
            MEM_ADDR_CALC: begin ALUSrcA = 1; ALUSrcB = 2'b10; end
            MEM_ACCESS_LW: begin MemRead = 1; IorD = 1; end
            ADDI_END: begin RegWrite = 1; end
            LUI_END: begin RegWrite = 1; RegSrc = 2'b10; end
            WB: begin RegWrite = 1; RegSrc = 2'b01; end
            MEM_ACCESS_SW: begin IorD = 1; MemWrite = 1; end
            R_EX: begin
                ALUSrcA = 1;
                case (Op)
                    OP_ADD: ALUm = 3'b000;
                    OP_SUB: ALUm = 3'b001;
                    OP_AND: ALUm = 3'b010;
                    OP_OR: ALUm = 3'b011;
                endcase
            end
            R_END: begin RegWrite = 1; RegDst = 2'b01; end
            BEQ_END: begin ALUSrcA = 1; ALUm = 3'b001; PCWriteCond = 1; PCSource = 2'b01; end
            J_END: begin PCWrite = 1; PCSource = 2'b10; end
            JAL_END: begin PCWrite = 1; PCSource = 2'b10; RegWrite = 1; RegDst = 2'b10; RegSrc = 2'b11; end
            JR_END: begin PCWrite = 1; PCSource = 2'b11; end
            default: ;
        endcase
    end

    // datapath
    always @ (*) begin
        if (IorD) mem_addr = ALUOut; else mem_addr = pc;
        if (ALUSrcA) ALUIn1 = A; else ALUIn1 = pc;
        case (RegDst)
            0: WriteRegister = instruction[20:16];
            1: WriteRegister = instruction[15:11];
            2: WriteRegister = 5'b11111;
        endcase
        case (RegSrc)
            0: WriteData = ALUOut;
            1: WriteData = mdr;
            2: WriteData = {instruction[15:0], 16'b0};
            3: WriteData = pc;
        endcase
        case (ALUSrcB)
            0: ALUIn2 = B;
            1: ALUIn2 = 4;
            2: ALUIn2 = imm;
            3: ALUIn2 = imm << 2;
        endcase
        case (PCSource)
            0: newpc = ALUResult;
            1: newpc = ALUOut;
            2: newpc = {pc[31:28], instruction[25:0], 2'b0};
            3: newpc = A;
        endcase
    end
    always @ (posedge clk) begin
        if (rst) begin
            pc <= 32'b0;
            instruction <= 0;
            mdr <= 0;
            ALUOut <= 0;
            A <= 0;
            B <= 0;
        end
        else begin
            A <= ReadData1;
            B <= ReadData2;
            ALUOut <= ALUResult;
            mdr <= MemData;
            if (PCwe) pc <= newpc;
            if (IRWrite) instruction <= MemData;
        end
    end
endmodule

