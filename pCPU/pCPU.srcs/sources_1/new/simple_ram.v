`timescale 1ns / 1ps
module simple_ram
	#(
		parameter WIDTH = 32,
		parameter DEPTH = 1,
		parameter INIT = "/home/petergu/MyHome/pComputer/pCPU/null.dat"
		//parameter INIT = ""
	)
	(
		input clk, 
		input [DEPTH-1:0]a,
		input [WIDTH-1:0]d,
		input we,
		output reg [WIDTH-1:0]spo
	);

	reg [WIDTH-1:0]mem[(2**DEPTH)-1:0];
	initial $readmemh(INIT, mem);
	
	always @ (posedge clk) begin
		if (we) mem[a] <= d;
		spo <= mem[a];
	end
endmodule
