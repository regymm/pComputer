`timescale 1ns / 1ps
// pComputer clock divider

module clock_divider_bd (
    input clk_in,
    output reg clk_out = 0
);
    always @(posedge clk_in) begin
        clk_out <= ~clk_out;
    end
endmodule
