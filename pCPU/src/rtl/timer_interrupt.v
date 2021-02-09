`timescale 1ns / 1ps
// pComputer timer interrupt unit
// 50ms interval under 125MHz clock
`include "pCPU.vh"

module timer
	#(
		parameter TIMER_COUNTER = 4000
	)
    (
        input clk,
        input rst,

        output reg irq
    );
    reg [31:0]counter = 0;
    always @ (posedge clk) begin
        if (rst) begin
            counter <= 0;
            irq <= 0;
        end
        else begin
            counter <= counter + 1;
            if (counter == TIMER_COUNTER) begin
                counter <= 0;
                irq <= 1;
            end
            else irq <= 0;
        end
    end
endmodule
