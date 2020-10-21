`timescale 1ns / 1ps
module simple_rom
	#(
		parameter WIDTH = 32,
		parameter DEPTH = 1,
		parameter INIT = "/home/petergu/MyHome/pComputer/pCPU/regfile.dat"
	)
	(
		input [DEPTH-1:0]a,
		output reg [WIDTH-1:0]spo
	);

	reg [WIDTH-1:0]mem[(2**DEPTH)-1:0];
	initial $readmemh(INIT, mem);
	
	always @ (a) begin
		spo = mem[a];
	end
endmodule
