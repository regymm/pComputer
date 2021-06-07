/**
 * File              : gpio.v
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2020.11.25
 * Last Modified Date: 2020.11.25
 */
`timescale 1ns / 1ps
// pComputer LED/Switch IO

module cache_cpu
	#(
		parameter WAYS=1,
		parameter WAY_LINES=128,
		parameter WAY_WORDS_PER_BLOCK=32,
		parameter WAY_TAG_LENGTH=32
	)
    (
        input clk,
        input rst,

        input [31:0]a,
        input [31:0]d,
        input we,
		input rd,
        output reg [31:0]spo,
		output ready,

		output burst_en,
		output [7:0]burst_length,
		output [31:0]lowmem_a,
		output [31:0]lowmem_d,
		output lowmem_we,
		output lowmem_rd,
		input [31:0]lowmem_spo,
		input lowmem_ready,

		output hit,
		output miss
    );

	localparam IDLE = 0;
	localparam HIT = 1;
	localparam LOAD = 2;
	localparam WRITEBACK = 3;
	localparam INIT = -1;
	reg [3:0]state = IDLE;

	reg [WAYS-1:0]way_en;
	reg way_we;
	reg way_tag_we;
	reg way_valid_in;
	reg way_dirty_in;
	reg way_iord_in;
    always @ (*) begin
		way_en[0] = 0;
		way_we = 0;
		way_tag_we = 0;

		way_valid_in = 0;
		way_dirty_in = 0;
		way_iord_in = 0;
		case (state)
			INIT: begin
			end
			IDLE: begin
			end
			HIT: begin
				if (weorrd) begin
					way_en[0] = 1;
					way_we = 1;
					way_tag_we = 1;

					way_valid_in = 1;
					way_dirty_in = 1;
					way_iord_in = 0;
				end else begin
					way_en[0] = 1;
					way_we = 0;
					way_tag_we = 1;

					way_valid_in = 1;
					way_dirty_in = way_tag_dirty;
					way_iord_in = 0;
				end
			end
			LOAD: begin
			end
			WRITEBACK: begin
			end
		endcase
    end

	reg weorrd;
	wire ready = !(we | rd) & (state == IDLE);
	reg [7:0]count;

    always @ (posedge clk) begin
        if (rst) begin
			state <= INIT;
			spo <= 0;
        end
        else begin
			if (state == INIT) begin
				if (& way_init_done) state <= IDLE;
			end else if (state == IDLE) begin
				if (we | rd) begin
					if (| way_hit) begin
						state <= HIT;
						//way_en[0] <= 1;
						//way_we <= 1;
						//way_tag_we <= 1;

						//way_valid_in <= 1;
						//way_dirty_in <= 1;
						//way_iord_in <= 0;
						weorrd <= we;
					end else begin
						if (!way_tag_valid) begin
							state <= LOAD;
						end else if (!way_tag_dirty) begin
							state <= LOAD;
						end else if (way_tag_dirty) begin
							state <= WRITEBACK;
						end
					end
				end
				//if (we) begin
				//end else if (rd) begin
				//end else begin
					//// real idle
				//end
			end else if (state == HIT) begin
				state <= IDLE;
				spo <= way_spo[0];
			end else if (state == WRITEBACK) begin
				//state <= LOAD;
			end else if (state == LOAD) begin
				//state <= HIT;
			end
        end
    end

	wire [WAYS-1:0]way_en;
	wire [31:0]way_spo[WAYS-1:0];
	wire [WAY_TAG_LENGTH-1:0]way_tag_out[WAYS-1:0];
	wire [WAYS-1:0]way_init_done;

	wire [WAYS-1:0]way_tag_valid;
	wire [WAYS-1:0]way_tag_dirty;
	wire [WAYS-1:0]way_tag_iord;
	wire [31-($clog2(WAY_LINES)+$clog2(WAY_WORDS_PER_BLOCK)+2):0]way_tag_addr[WAYS-1:0];

	wire [WAYS-1:0]way_hit;

	wire [31:0]way_a = a;
	wire [31:0]way_d = d;
	wire [$clog2(WAY_LINES)+$clog2(WAY_WORDS_PER_BLOCK)+2-1 - 3:0]way_zeros = 0;
	wire [31:0]way_tag_in = {a[31:$clog2(WAY_LINES)+$clog2(WAY_WORDS_PER_BLOCK)+2], way_zeros, way_iord_in, way_dirty_in, way_valid_in};

	genvar i;
	generate
		for (i = 0; i < WAYS; i = i + 1) begin
			cacheway #(
				.LINES(WAY_LINES),
				.WORDS_PER_BLOCK(WAY_WORDS_PER_BLOCK),
				.TAG_LENGTH(WAY_TAG_LENGTH)
			) way_gen (
				.clk(clk),
				.rst(rst),
				.en(way_en[i]),
				.a(way_a),
				.d(way_d),
				.we(way_we),
				.spo(way_spo[i]),
				.tag_we(way_tag_we),
				.tag_in(way_tag_in),
				.tag_out(way_tag_out[i]),
				.init_done(way_init_done[i])
			);

			assign way_tag_valid[i] = way_tag_out[i][0];
			assign way_tag_dirty[i] = way_tag_out[i][1];
			assign way_tag_iord[i] = way_tag_out[i][2];
			assign way_tag_addr[i] = way_tag_out[i][31:$clog2(WAY_LINES)+$clog2(WAY_WORDS_PER_BLOCK)+2];
			assign way_hit[i] = way_tag_valid[i] & (way_tag_addr[i] == 
				a[31:$clog2(WAY_LINES)+$clog2(WAY_WORDS_PER_BLOCK)+2]);
		end
	endgenerate


endmodule
