`timescale 1ns / 1ps
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

  // HDMI output
  output [2:0] TMDSp,
  output [2:0] TMDSn,
  output TMDSp_clock,
  output TMDSn_clock
  //inout HDMI_SDA,
  //inout HDMI_SCL,
  //input HDMI_HPD
);

//wire clk_pixel_x5;
//wire clk_pixel;
//hdmi_pll hdmi_pll(.inclk0(CLK_48MHZ), .c0(clk_pixel), .c1(clk_pixel_x5), .c2(clk_audio));

//localparam AUDIO_BIT_WIDTH = 16;
//localparam AUDIO_RATE = 48000;
//localparam WAVE_RATE = 480;

//logic [AUDIO_BIT_WIDTH-1:0] audio_sample_word;
//logic [AUDIO_BIT_WIDTH-1:0] audio_sample_word_dampened; // This is to avoid giving you a heart attack -- it'll be really loud if it uses the full dynamic range.
//assign audio_sample_word_dampened = audio_sample_word >> 9;

//sawtooth #(
	//.BIT_WIDTH(AUDIO_BIT_WIDTH), 
	//.SAMPLE_RATE(AUDIO_RATE), 
	//.WAVE_RATE(WAVE_RATE)
//) sawtooth (
	//.clk_audio(clk_audio), 
	//.level(audio_sample_word)
//);

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

//// my VRAM interface
//// 30 rows, 80 columns
//reg [15:0]vram[4095:0];
//reg [15:0]vout;
//reg [31:0]a_reg;
//reg [15:0]d_reg;
//reg we_reg;
(*mark_debug = "true"*) wire [11:0]a2 = ({6'b0, cy_onscreen[9:4]}) * 80 + {5'b0, cx_onscreen[9:3]};
//always @ (posedge clk) begin
	//a_reg <= a;
	//d_reg <= {d[23:16], d[31:24]}; // endian problem again
	//we_reg <= we;
	//if (we_reg) vram[a_reg[13:2]] <= d_reg[15:0];
	////spo <= {16'b0, vram[a[13:2]]};
//end
//always @ (posedge clk_pix) begin
	//vout <= vram[a2 + 1];
//end

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

//reg we_reg_reg;
reg [15:0]vout;
always @ (posedge clk_pix) begin
	vout <= vram_spo;
end
//always @ (posedge clk) begin
	//we_reg_reg <= we_reg;
	//if (!we_reg_reg) vout <= vram_spo;
	////vout <= vram_spo;
//end


//logic [7:0] character = 8'h30;
//logic [5:0] prevcy = 6'd0;
//always @(posedge clk_pix)
//begin
	//if (cy == 10'd0)
	//begin
		//character <= 8'h30;
		//prevcy <= 6'd0;
	//end
	//else if (prevcy != cy[9:4])
	//begin
		//character <= character + 8'h01;
		//prevcy <= cy[9:4];
	//end
//end

console console(
	.clk_pixel(clk_pix), 
	.codepoint(vout[7:0]), 
	.attribute(vout[15:7]), 
	//.attribute({cx[9], cy[8:6], cx[8:5]}), 
	.cx(cx_onscreen), 
	.cy(cy_onscreen), 
	.rgb(rgb)
);
endmodule
