`timescale 1ns / 1ps
// pComputer multi-cycle CPU

module cpu_multi_cycle
    (
        input clk,
        input rst,
        input irq,
        input [3:0]icause,
        output iack,

        output reg [31:0]a,
        output reg [31:0]d,
        output reg we,
        input [31:0]spo
    );

    // internal registers
    reg [31:0]instruction = 0;
    reg [31:0]pc = 0;
    reg [31:0]current_pc = 0;
    reg [31:0]mdr = 0;
    reg [31:0]ALUOut = 0;
    reg ALUCf = 0;
    reg ALUOf = 0;
    reg ALUSf = 0;
    reg [31:0]A = 0;
    reg [31:0]B = 0;

    // some signals
    wire ALUZero;
    reg [31:0]newpc;
    wire [31:0]imm = {{16{instruction[15]}}, instruction[15:0]};
    wire [31:0]status;

    // control unit signals
    wire PCWrite;
    wire NewInstr;
    wire IorD;
    wire MemRead;
    wire MemWrite;
    wire [2:0]RegSrc;
    wire IRWrite;
    wire [2:0]PCSource;
    wire [2:0]ALUm;
    wire ALUSrcA;
    wire [1:0]ALUSrcB;
    wire RegWrite;
    wire [1:0]RegDst;
    wire Cmp;
    wire EPCWrite;
    wire EPCSrc;
    wire CauseWrite;
    wire CauseSrc;
    wire StatusWrite;
    wire StatusSrc;
    wire [1:0]Mfc0Src;
    control_unit control_unit_inst
    (
        .clk(clk),
        .rst(rst),
        .instruction(instruction),
        .ALUZero(ALUZero),
        .ALUCf(ALUCf),
        .ALUOf(ALUOf),
        .ALUSf(ALUSf),
        .status(status),
        .irq(irq),
        .iack(iack),

        .PCWrite(PCWrite),
        .NewInstr(NewInstr),
        .IorD(IorD),
        .MemRead(MemRead),
        .MemWrite(MemWrite),
        .RegSrc(RegSrc),
        .IRWrite(IRWrite),
        .PCSource(PCSource),
        .ALUm(ALUm),
        .ALUSrcA(ALUSrcA),
        .ALUSrcB(ALUSrcB),
        .RegWrite(RegWrite),
        .RegDst(RegDst),
        .Cmp(Cmp),

        .EPCSrc(EPCSrc),
        .EPCWrite(EPCWrite),
        .CauseWrite(CauseWrite),
        .CauseSrc(CauseSrc),
        .StatusWrite(StatusWrite),
        .StatusSrc(StatusSrc),
        .Mfc0Src(Mfc0Src)
    );

    // register file
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

    // memory mapper
    reg [31:0]mem_addr;
    reg [31:0]MemData;
    always @ (*) begin
        a = mem_addr;
        d = B;
        we = MemWrite;
        MemData = spo;
    end

    // ALU
    reg [31:0]ALUIn1;
    reg [31:0]ALUIn2;
    wire [31:0]ALUResult;
    wire ALUCf_wire;
    wire ALUOf_wire;
    wire ALUSf_wire;
    alu alu_inst
    (
        .m(ALUm),
        .a(ALUIn1),
        .b(ALUIn2),
        .y(ALUResult),
        .zf(ALUZero),
        .cf(ALUCf_wire),
        .of(ALUOf_wire),
        .sf(ALUSf_wire)
    );

    // coprocessor0
    wire [31:0]mfc0out;
    wire [31:0]epc;
    coprocessor0 coprocessor0_inst
    (
        .clk(clk),
        .rst(rst),
        .Mfc0Src(Mfc0Src),
        .EPCSrc(EPCSrc),
        .CauseSrc(CauseSrc),
        .StatusSrc(StatusSrc),
        .EPCWrite(EPCWrite),
        .CauseWrite(CauseWrite),
        .StatusWrite(StatusWrite),

        .pc(pc),
        .current_pc(current_pc),
        .causedata_outside(icause),
        .mtc0in(B),
        .mfc0out(mfc0out),
        .epc_out(epc),
        .status_out(status)
    );

    // datapath -- main
    always @ (*) begin
        case (IorD)
            0: mem_addr = pc;
            1: mem_addr = ALUOut;
        endcase
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
            4: WriteData = mfc0out;
            5: WriteData = {31'b0, Cmp};
        endcase
        case (ALUSrcB)
            0: ALUIn2 = B;
            1: ALUIn2 = 4;
            2: ALUIn2 = imm;
            3: ALUIn2 = imm << 2;
        endcase
        case (ALUSrcA)
            0: ALUIn1 = pc;
            1: ALUIn1 = A;
        endcase
        case (PCSource)
            0: newpc = ALUResult;
            1: newpc = ALUOut;
            2: newpc = {pc[31:28], instruction[25:0], 2'b0};
            3: newpc = A;
            4: newpc = epc;
            5: newpc = 32'h80000000;
        endcase
    end
    always @ (posedge clk) begin
        if (rst) begin
            pc <= 32'b0;
            instruction <= 0;
            mdr <= 0;
            ALUOut <= 0;
            ALUCf <= 0;
            ALUOf <= 0;
            A <= 0;
            B <= 0;
        end
        else begin
            A <= ReadData1;
            B <= ReadData2;
            ALUOut <= ALUResult;
            ALUCf <= ALUCf_wire;
            ALUOf <= ALUOf_wire;
            ALUSf <= ALUSf_wire;
            mdr <= MemData;
            if (PCWrite) pc <= newpc;
            if (NewInstr) current_pc <= pc;
            if (IRWrite) instruction <= MemData;
        end
    end
endmodule
