/**
 * File              : privilege.v
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2020.12.26
 * Last Modified Date: 2021.01.03
 */
`timescale 1ns / 1ps

module privilege
	(
		input clk,
		input rst,

		input [11:0]a,
		input [31:0]d,
		input we,
		output reg [31:0]spo,

		input meip,
		input mtip,

		input [31:0]mepc_d,
		input mepc_we,

		input mcause_we,
		input mcause_iore,
		input [4:0]mcause_d_e, 
		// mcause_d_i is determined inside privilege module

		input mtval_we,
		input [31:0]mtval_d,

		output mcause_isinterrupt,
		//output mstatus_mpp,
		output mstatus_mie,
		output mstatus_mpie
    );

	// Control State Registers
	reg [31:0]mstatus = 32'b0_00000000_0000000000_00_00_0_1_;
	reg [31:0]misa = 32'b01_0000_00000000000001000100000000;
	reg [31:0]mie;
	reg [31:0]mtvec;
	reg [31:0]mscratch;
	reg [31:0]mepc;
	reg [31:0]mcause;
	reg [31:0]mtval;
	reg [31:0]mip;

	//reg [63:0]mtime;
	//reg [63:0]mtimecmp;

	//wire [31:0]mstatus_wpri_mask = 32'b01111111100000000000011001000100; // WPRI otherwise
	wire [31:0]mstatus_read_mask	= 32'b11111111111111111111111101110111;
	wire [31:0]mstatus_read_val		= 32'b000000000000000000000000x000x000;
	wire [31:0]mstatus_write_mask	= 32'b11111111111111111111111101110111;
	wire [31:0]mtvec_read_mask		= 32'b00000000000000000000000000000011;
	wire [31:0]mtvec_read_val		= 32'bxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx00;
	wire [31:0]mtvec_write_mask		= 32'b00000000000000000000000000000011;
	wire [31:0]mip_read_mask		= 32'b11111111111111111111111111110111;
	wire [31:0]mip_read_val			= {20'b0, meip, 3'b0, mtip, 3'b0, 1'bx, 3'b0};
	wire [31:0]mip_write_mask		= 32'b11111111111111111111111111110111; // WARL otherwise
	wire [31:0]mie_read_mask		= 32'b00000000000000000000100010001000;
	wire [31:0]mie_read_val			= 32'b00000000000000000000x000x000x000;
	wire [31:0]mie_write_mask		= 32'b11111111111111111111011101110111; // WARL otherwise
	wire [31:0]mepc_read_mask		= 32'b00000000000000000000000000000011;
	wire [31:0]mepc_read_val		= 32'bxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx00;
	wire [31:0]mepc_write_mask		= 32'b00000000000000000000000000000011;
	//wire [31:0]mip_wpri_mask		= 32'b111111111010001000100; // WARL otherwise


	//reg [31:0]sstatus;
	//reg [31:0]sie;
	//reg [31:0]stvec;
	//reg [31:0]sscratch;
	//reg [31:0]sepc;
	//reg [31:0]scause;
	//reg [31:0]stval;
	//reg [31:0]sip;

	//reg [31:0]satp;

	//reg [31:0]misa;
	//reg [31:0]timee;
	//reg [31:0]timeeh;

	assign mcause_isinterrupt = mcause[31];
	assign mstatus_mie = mstatus[3];
	assign mstatus_mpie = mstatus[7];
	

	always @ (*) begin
		case (a)
			//12'h100: spo = sstatus;
			//12'h104: spo = sie;
			//12'h105: spo = stvec;
			//12'h140: spo = sscratch;
			//12'h141: spo = sepc;
			//12'h142: spo = scause;
			//12'h143: spo = stval;
			//12'h144: spo = sip;
			//12'h180: spo = satp;

			12'h300: spo = mstatus_read_val & mstatus_read_mask + mstatus & ~mstatus_read_mask;
			12'h301: spo = misa;
			12'h304: spo = mie_read_val & mie_read_mask + mie & ~mie_read_mask;
			12'h305: spo = mtvec_read_val & mtvec_read_mask + mtvec & ~mtvec_read_mask;
			12'h340: spo = mscratch;
			12'h341: spo = mepc_read_val & mepc_read_mask + mepc & ~mepc_read_mask;
			12'h342: spo = mcause;
			12'h343: spo = mtval;
			12'h344: spo = mip_read_val & mip_read_mask + mip & ~mip_read_mask;

			//12'hC01: spo = timee;
			//12'hC81: spo = timeeh;

			default: spo = 0;
		endcase
	end

	always @ (posedge clk) begin
		if (rst) begin
		end else begin
			if (we) case (a)
				12'h300: mstatus	<= mstatus & mstatus_write_mask + d & ~mstatus_write_mask;
				12'h304: mie		<= mie & mie_write_mask + d & ~mie_write_mask;
				12'h305: mtvec		<= mtvec & mtvec_write_mask + d & ~mtvec_write_mask;
				12'h340: mscratch	<= d;
				12'h341: mepc		<= mepc & mepc_write_mask + d & ~mepc_write_mask;
				12'h342: mcause		<= d; // WLRL, should be taken care of but not now
				12'h343: mtval		<= d; // should be taken care of
				12'h344: mip		<= mip & mip_write_mask + d & ~mip_write_mask;
				default: ;
			endcase
			else if (mepc_we)
				mepc <= mepc_d;
			else if (mcause_we)
				mcause <= mcause_iore ? {1'b1, 31'b0} : {1'b0, 26'b0, mcause_d_e}; // TODO
			else if (mtval_we)
				mtval <= mtval_d;
		end
	end

endmodule
