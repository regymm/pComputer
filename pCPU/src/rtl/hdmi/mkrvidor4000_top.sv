`timescale 1ns / 1ps
`define HDMI_PICTURE
module mkrvidor4000_top
(
	input clk,
	input rst,
	input clk_pix,
	input clk_tmds,
	//input clk_audio,

	input [31:0]a,
	input [31:0]d,
	input we,
	output logic [31:0]spo = 0,

//`ifdef HDMI_PICTURE
	//output req,
	//input gnt,
	//output burst_en,
	//output [7:0]burst_length,
	//output [31:0]vram_a,
	//output [31:0]vram_d,
	//output reg vram_we,
	//output reg vram_rd,
	//input [31:0]vram_spo,
	//input vram_ready,
//`endif


	// HDMI output
	output [2:0] TMDSp,
	output [2:0] TMDSn,
	output TMDSp_clock,
	output TMDSn_clock
);

(*mark_debug = "true"*) logic [23:0] rgb;
logic [10:0]cx;
logic [9:0] cy;
(*mark_debug = "true"*) wire [10:0]cx_next;
(*mark_debug = "true"*) wire [9:0] cy_next;
hdmi #(
	.VIDEO_ID_CODE(1), 
	.DVI_OUTPUT(1),
	.DDRIO(0)
	//.AUDIO_RATE(AUDIO_RATE), 
	//.AUDIO_BIT_WIDTH(AUDIO_BIT_WIDTH)
) hdmi(
	.clk_pixel_x10(clk_tmds), 
	.clk_pixel(clk_pix), 
	.clk_audio(0), 
	.rgb(rgb), 
	//.audio_sample_word(0), 
	.tmds_p(TMDSp), 
	.tmds_clock_p(TMDSp_clock), 
	.tmds_n(TMDSn), 
	.tmds_clock_n(TMDSn_clock), 
	.cx(cx), 
	.cy(cy),
	.cx_next(cx_next),
	.cy_next(cy_next)
);

wire [10:0]cx_onscreen = cx_next - 160;
wire [9:0] cy_onscreen = cy_next - 45;
wire [31:0]data = {d[7:0], d[15:8], d[23:16], d[31:24]};

`ifdef HDMI_PICTURE
	assign burst_en = 1;
	assign burst_length = 512;
	assign lowmem_d = 0;
	assign lowmem_we = 0;

	// (cx_onscreen/2 + cy_onscreen/2 * 480/2)
	wire [16:0]pix_a_pix = (cx_onscreen/2) + cy_onscreen/2 * 640/2;
	// /4 for memory address (each 32-bit elem contains 4 pixels)
	wire [14:0]pix_a_vram = pix_a_pix[16:2];
	//reg [14:0]pix_a = 0;
	wire [31:0]pix_data;
	reg [7:0]pix_curr;
	always @ (*) begin case (pix_a_pix[1:0])
			2'b00: pix_curr = pix_data[7:0];
			2'b01: pix_curr = pix_data[15:8];
			2'b10: pix_curr = pix_data[23:16];
			2'b11: pix_curr = pix_data[31:24];
	endcase end
	assign rgb = {
		{pix_curr[7:5], pix_curr[7:5] == 3'b111 ? 5'b1 : 5'b0},
		{pix_curr[4:2], pix_curr[4:2] == 3'b111 ? 5'b1 : 5'b0},
		{pix_curr[1:0], pix_curr[1:0] == 2'b11 ? 6'b1 : 6'b0}
	};

	reg [14:0]pix_a_last;
	always @(posedge clk_pix) begin
		pix_a_last <= pix_a_vram;
	end
	wire [31:0]pix_spo_l;
	wire [31:0]pix_spo_h;
	assign pix_data = pix_a_last[14] ? pix_spo_h : pix_spo_l;
	// 75KB total VRAM, supports 640x480 2bit monochrome color
	// or 320x240 8bit 3-3-2 color
	simple_dp_ram #(
		.WIDTH(32),
		.DEPTH(14)
	) vram_low64K (
		.clk1(clk),
		.a1(a[15:2]),
		.d1(data),
		.we1(we & !a[16]),
		.clk2(clk_pix),
		.a2(pix_a_vram),
		.rd2(1),
		.spo2(pix_spo_l)
		//.ready()
	);
	simple_dp_ram#(
		.WIDTH(32),
		.DEPTH(12)
	) vram_high16K (
		.clk1(clk),
		.a1(a[15:2]),
		.d1(data),
		.we1(we & a[16]),
		.clk2(clk_pix),
		.a2(pix_a_vram),
		.rd2(1),
		.spo2(pix_spo_h)
		//.ready()
	);


`else
// HDMI console only

// my VRAM interface
// 30 rows, 80 columns
(*mark_debug = "true"*) wire [11:0]a2 = ({6'b0, cy_onscreen[9:4]}) * 80 + {5'b0, cx_onscreen[9:3]};

reg [13:0]a_reg;
reg [15:0]d_reg;
reg we_reg;
wire [15:0]vram_spo;
always @ (posedge clk) begin
	we_reg <= we;
	a_reg <= we ? a[13:2] : a2;
	d_reg <= {d[23:16], d[31:24]};
end

simple_ram #(
	.WIDTH(16),
	.DEPTH(12)
) video_ram (
	.clk(clk),
	.a(a_reg),
	.d(d_reg),
	.we(we_reg),
	.spo(vram_spo)
);

reg [15:0]vout;
always @ (posedge clk_pix) begin
	vout <= vram_spo;
end

console console(
	.clk_pixel(clk_pix), 
	.codepoint(vout[7:0]), 
	.attribute(vout[15:7]), 
	//.attribute({cx[9], cy[8:6], cx[8:5]}), 
	.cx(cx_onscreen), 
	.cy(cy_onscreen), 
	.rgb(rgb)
);
`endif
endmodule
