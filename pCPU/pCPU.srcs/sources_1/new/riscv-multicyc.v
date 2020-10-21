/**
 * File              : riscv-multicyc.v
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2020.10.21
 * Last Modified Date: 2020.10.21
 */
// pComputer 5-cycle RISC-V processor

`timescale 1ns / 1ps

module riscv_multicyc
	(
		input clk,
		input rst,

		output [1:0]ring,

		input irq,
		input [3:0]icause,
		output iack,

		output reg [31:0]a,
		output reg [31:0]d,
		output reg we,
		output reg rd,
		input [31:0]spo,
		input ready
    );

	localparam START_ADDR = 32'hff000000;
	localparam INVALID_ADDR =32'hffffffff;

	// 
	reg [31:0]pc;
	reg [31:0]oldpc;
	reg [31:0]instruction;

	reg [31:0]A;
	reg [31:0]B;
	reg [31:0]ALUOut;
	reg [31:0]ALUOut2;
	reg [31:0]mar;
	reg [31:0]mdr;

	// Control State Registers
	reg [31:0]mtvec;
	reg [31:0]mepc;
	reg [31:0]mcause;
	reg [31:0]mie;
	reg [31:0]mip;
	reg [31:0]mtval;
	reg [31:0]mscratch;
	reg [31:0]mstatus;

	// control signals
	wire PCWrite;
	wire [1:0]PCSrc;
	wire [1:0]IorDorW;
	wire MemRead;
	wire MemWrite;
	wire MemSrc;
	wire IRWrite;
	wire IRLate;
	wire [3:0]ALUm;
	wire ALUSrcA;
	wire ALUSrcB;
	wire RegWrite;
	wire [2:0]RegSrc;
	//wire RegDst;


    // register file
    reg [4:0]WriteRegister;
    reg [31:0]WriteData;
    wire [31:0]ReadData1;
    wire [31:0]ReadData2;
	// signal: RegWrite
    register_file register_file_inst
    (
        .clk(clk),
        .ra0(instruction[24:20]),
        .ra1(instruction[19:15]),
        .wa(instruction[11:7]),
        .we(RegWrite),
        .wd(WriteData),
        .rd0(ReadData1),
        .rd1(ReadData2)
    );

	// memory mapper
	// signal: MemWrite
	// signal: MemRead
	// signal: MemReady
	reg [31:0]mem_addr;
	reg [31:0]memwrite_data;
	reg [31:0]memread_data;
	always @ (*) begin
		a = mem_addr;
		d = memwrite_data;
		we = MemWrite;
		rd = MemRead;
		memread_data = spo;
		MemReady = ready;
	end

	// ALU
	reg [31:0]ALUIn1;
	reg [31:0]ALUIn2;
	wire [31:0]ALUResult;
    //wire ALUZero_out;
    //wire ALUCf_out;
    //wire ALUOf_out;
    //wire ALUSf_out;
	// signal in: ALUm
	// signal out: ALUZero/Cf/Of/Sf
	alu alu_inst
	(
		.m(ALUm),
		.a(ALUIn1),
		.b(ALUIn2),
		.y(ALUResult)
		//.zf(ALUZero_out),
		//.cf(ALUCf_out),
		//.of(ALUOf_out),
		//.sf(ALUSf_out)
	);

	wire [7:0]op = instruction[6:0];
	wire nse = instruction[14];
	reg [31:0]imm;
	wire [31:0]imm_i = {{21{instruction[31]}}, instruction[30:20]};
	wire [31:0]imm_i_nse = {20'b0, instruction[31:20]};
	wire [31:0]imm_b = {{21{instruction[31]}}, instruction[7], instruction[30:25], instruction[11:8]};
	wire [31:0]imm_j = {{13{instruction[31]}}, instruction[19:12], instruction[20], instruction[30:21]};
	wire [31:0]imm_u = {instruction[31:12], 12'b0};
	wire [31:0]imm_s = {{21{instruction[31]}}, instruction[30:25], instruction[11:7]};
	always @ (*) begin
		if (op == OP_LUI | op == OP_AUIPC) imm = imm_u;
		else if (op == OP_R_I) imm = nse ? imm_i_nse : imm_i;
		else if (op == OP_LOAD | op == OP_JALR) imm = imm_i;
		else if (op == OP_BR) imm = imm_b;
		else if (op == OP_JAL) imm = imm_j;
		else if (op == OP_STORE) imm = imm_s;
		else imm = 0;
	end
	localparam OP_LUI	=	7'b0110111;
	localparam OP_AUIPC	=	7'b0010111;
	localparam OP_JAL	=	7'b1101111;
	localparam OP_JALR	=	7'b1100111;
	localparam OP_BR	=	7'b1100011;
	localparam OP_LOAD	=	7'b0000011;
	localparam OP_STORE	=	7'b0100011;
	localparam OP_R_I	=	7'b0010011;
	localparam OP_R		=	7'b0110011;
	localparam OP_FENCE	=	7'b0001111;
	localparam OP_ENV	=	7'b1110011;
	reg [7:0]phase;
	reg [7:0]phase_return;
	localparam IF			=	100;
	localparam IF_REMEDY	=	200;
	localparam ID_RF		=	300;
	localparam EX			=	400;
	localparam MEM			=	500;
	localparam WB			=	600;
	localparam MEM_WAIT		=	700;
	localparam BAD			=	10000;


	// control signals
	always @ (*) begin
		PCWrite = 0;
		PCSrc = 0;
		//NewInstr = 0;
		IorDorW = 0;
		MemRead = 0;
		MemWrite = 0;
		MemSrc = 0;
		IRWrite = 0;
		IRLate = 0;
		ALUm = 0;
		ALUSrcA = 0;
		ALUSrcB = 0;
		RegWrite = 0;
		RegSrc = 0;
		//RegDst = 0;
		iack = 0;
		case (phase)
			IF: begin
				MemRead = 1;
				IRWrite = 1;
			end
			IF_REMEDY: begin
				IRWrite = 1; IRLate = 1;
			end
			ID_RF: begin
				PCWrite = 1;
				ALUSrcA = 1; ALUSrcB = 1;
			end
			EX: begin
				if (op == OP_R) begin
					ALUm = {instruction[30], instruction[14:12]};
				end else if (op == OP_R_I) begin
					ALUm = {1'b0, instruction[14:12]};
					ALUSrcB = 1;
				end else if (op == OP_JALR) begin
					ALUSrcB = 1;
				end else if (op == OP_BR) begin
					ALUm = instruction[14] ? {2'b0, instruction[14:13]} : 4'b0;
				end
			end
			MEM: begin
				if (op == OP_STORE) begin
					MemWrite = 1; IorDorW = 1;
					MemSrc = instruction[14:12]; // SB, SH, SW
				end else if (op == OP_LOAD) begin
					MemRead = 1; IorDorW = 1;
				end
			end
			WB: begin
				if (op == OP_R | op == OP_R_I) begin
					RegWrite = 1;
				end else if (op == OP_LUI) begin
					RegWrite = 1; RegSrc = 2;
				end else if (op == OP_AUIPC) begin
					RegWrite = 1;
				end else if (op == OP_JAL) begin
					RegWrite = 1; RegSrc = 3;
					PCWrite = 1; PCSrc = 2;
				end else if (op == OP_JALR) begin
					RegWrite = 1; RegSrc = 3;
					PCWrite = 1; PCSrc = 3;
				end else if (op == OP_BR) begin
					PCWrite = instruction[12] ^ &ALUOut; PCSrc = 1;
				end else if (op == OP_LOAD) begin // LB, LH, LW, LBU, LHU
					RegWrite = 1; RegSrc = {1'b1, instruction[13:12]};
				end
			end
			MEM_WAIT: begin
				IorDorW = 2;
			end
		endcase
	end
	// control FSM
	always @ (posedge clk) begin
		if (rst) begin
			phase <= IF;
		end
		else begin
			case (phase)
				IF: begin
					if (!MemReady) begin
						phase <= MEM_WAIT;
						phase_return <= IF_REMEDY;
					end else phase <= ID_RF;
				end
				IF_REMEDY: phase <= ID_RF;
				ID_RF: begin
					//if (0) phase <= I_INT_END;
					if (op == OP_ENV | 0) phase <= ID_RF;
					else if (op == OP_FENCE) phase <= IF; // FENCE does nothing in our simple architecture
					else if ( op == OP_LUI | op == OP_AUIPC | op == OP_JAL) phase <= WB;
					else phase <= EX;
				end
				EX: begin
					if (op == OP_LOAD | op == OP_STORE) phase <= MEM;
					else if (op == OP_ENV) phase <= IF;
					else phase <= WB;
				end
				MEM: begin
					phase <= MEM_WAIT;
					if (op == OP_LOAD) phase_return <= WB;
					else /* op == OP_STORE */ phase_return <= IF;
				end
				WB: begin
					phase <= IF;
				end
				MEM_WAIT: begin
					if (MemReady) phase <= phase_return;
					else phase <= MEM_WAIT;
				end
				BAD: begin
					phase <= BAD;
				end
			endcase
		end
	end

	// CPU datapath
	wire [31:0]newpc;
	always @ (*) begin
		case (IorDorW)
			0: mem_addr = pc; // instruction
			1: mem_addr = ALUOut; // data
			2: mem_addr = mar; // wait
			default: mem_addr = INVALID_ADDR;
		endcase
		case (PCSrc)
			0: newpc = pc + 4;
			1: newpc = ALUOut2; // Branch
			2: newpc = ALUOut; // JAL
			3: newpc = ALUOut & ~1; // JALR
			// exception TODO
			default: newpc = 0;
		endcase
		case (ALUSrcA)
			0: ALUIn1 = A;
			1: ALUIn1 = pc; // haven't +4
			default: ALUIn1 = 0;
		endcase
		case (ALUSrcB)
			0: ALUIn2 = B;
			1: ALUIn2 = imm;
		endcase
		case (RegSrc)
			0: WriteData = ALUOut;
			//1: WriteData = mdr;
			2: WriteData = imm;
			3: WriteData = pc; // already +4
			// LOAD
			4: WriteData = nse ? {24'b0, mdr[7:0]}: {{24{mdr[7]}}, mdr[7:0]};
			5: WriteData = nse ? {16'b0, mdr[15:0]}: {{16{mdr[15]}}, mdr[15:0]};
			6: WriteData = mdr;
			default: WriteData = 0;
		endcase
		case (MemSrc)
			// STORE
			0: memwrite_data = {24'b0, ReadData2[7:0]};
			1: memwrite_data = {16'b0, ReadData2[15:0]};
			2: memwrite_data = ReadData2;
			default: memwrite_data = 0;
		endcase
	end
	// CPU main
	always @ (posedge clk) begin
		if (rst) begin
			pc <= START_ADDR;
		end
		else begin
			A <= ReadData1;
			B <= ReadData2;
			
			ALUOut <= ALUResult; ALUOut2 <= ALUOut;

			//ALUCf <= ALUCf_out;
			//ALUOf <= ALUOf_out;
			//ALUSf <= ALUSf_out;
			//ALUZero <= ALUZero_out;

			mdr <= memread_data;
			mar <= mem_addr;

			if (PCWrite) begin
				pc <= newpc; oldpc <= pc;
			end

			if (IRWrite) begin
				if (IRLate) instruction <= mdr;
				else instruction <= memread_data;
			end
		end
	end
endmodule
