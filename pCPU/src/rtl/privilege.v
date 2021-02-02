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

		// from interrupt.v
		input eip,
		output reg eip_reply,
		// from timer.v
		input tip,
		output reg tip_reply,

		//input mcause_we,
		//input mcause_iore,
		//input [4:0]mcause_d_e,
		//// mcause_d_i is determined inside privilege module

		//input mtval_we,
		//input [31:0]mtval_d,

		// handle mie & mpie when exception begins or ends
		input on_exc_enter,
		input on_exc_leave,

		// for CPU exception
		input [31:0]pc_in,
		output [31:0]mtvec_out,
		// for CPU mret
		output [31:0]mepc_out,

		//output mcause_isinterrupt,
		////output mstatus_mpp,
		//output mstatus_mie,
		//output mstatus_mpie,


		// interrupt that goes into CPU directly
		output reg interrupt,
		input int_reply
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

	assign mepc_out = mepc;
	assign mtvec_out = mtvec;

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
	wire [31:0]mip_read_val			= {20'b0, eip, 3'b0, tip, 3'b0, 1'bx, 3'b0};
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
	
	wire meie = mie[11];
	wire mtie = mie[7];
	wire msie = mie[3];

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
			else if (on_exc_enter) begin
				mstatus <= {mstatus[31:8], mstatus[3], mstatus[6:4], 1'b0, mstatus[2:0]};
				mepc <= pc_in;
			end else if (on_exc_leave) begin
				mstatus <= {mstatus[31:8], 1'b1, mstatus[6:4], mstatus[7], mstatus[2:0]};
			end
			//else if (mepc_we)
				//mepc <= mepc_d;
			//else if (mcause_we)
				//mcause <= mcause_iore ? {1'b1, 31'b0} : {1'b0, 26'b0, mcause_d_e}; // TODO
			//else if (mtval_we)
				//mtval <= mtval_d;
		end
	end

	reg int_reply_reg;
	reg int_pending;
	reg eip_reg;
	reg tip_reg;
	reg [1:0]int_source;
	always @ (posedge clk) begin
		int_reply_reg <= int_reply;
		int_pending <= mstatus_mie & (eip&meie | tip&mtie | msie);
		eip_reg <= eip;
		tip_reg <= tip;
	end

	localparam IDLE = 2'b00;
	localparam BUSY = 2'b01;
	localparam END = 2'b11;
	reg [1:0]state = IDLE;
	always @ (posedge clk) begin
		if (rst) begin
			state <= IDLE;
			interrupt <= 0;
			eip_reply <= 0;
			tip_reply <= 0;
		end else begin
			case (state)
				IDLE: begin
					if (int_pending) begin
						int_source <= {eip&meie, tip&mtie};
						interrupt <= 1;
						state <= BUSY;
					end
				end
				BUSY: begin
					if (int_reply_reg) begin
						interrupt <= 0;
						if (int_source[1]) eip_reply <= 1;
						else if (int_source[0]) tip_reply <= 1;
						//else 0; // handle sie
						state <= END;
					end
				end
				END: begin
					if (int_source[1]) eip_reply <= 0;
					else if (int_source[0]) tip_reply <= 0;
					state <= IDLE;
				end
				default: state <= IDLE;
			endcase
		end

	end

endmodule
