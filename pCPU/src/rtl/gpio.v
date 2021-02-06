/**
 * File              : gpio.v
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2020.11.25
 * Last Modified Date: 2020.11.25
 */
`timescale 1ns / 1ps
// pComputer LED/Switch IO

module gpio
    (
        input clk,
        input rst,
        input [3:0]a,
        input [31:0]d,
        input we,
        output reg [31:0]spo,

        input [1:0]btn, 
		input [1:0]sw,
        output reg [3:0]led,

		output reg irq = 0 // TODO
    );

	wire [2:0]data = d[26:24];

    always @ (*) begin
        case (a)
            0: spo = {31'b0, btn[0]};
            1: spo = {31'b0, btn[1]};
            4: spo = {31'b0, sw[0]};
            5: spo = {31'b0, sw[1]};
            6: spo = {31'b0, led[0]};
            7: spo = {31'b0, led[1]};
            8: spo = {31'b0, led[2]};
			9: spo = {31'b0, led[3]};
            default: spo = 32'b0;
        endcase
    end

    always @ (posedge clk) begin
        if (rst) begin
            led <= 4'b1111;
        end
        else if (we) begin
            case (a)
                6: led[0] <= data[0];
                7: led[1] <= data[0];
                8: led[2] <= data[0];
                9: led[3] <= data[0];
                default: ;
            endcase
        end
    end
endmodule
