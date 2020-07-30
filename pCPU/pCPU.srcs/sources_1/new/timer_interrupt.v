`timescale 1ns / 1ps
// pComputer timer interrupt unit
// 50ms interval under 125MHz clock

module timer_interrupt
    (
        input clk_125M,
        input rst,

        output reg irq
    );
    //localparam CNTMAX = 150000000;
    localparam CNTMAX = 80000000;
    //localparam CNTMAX = 500;
    reg [31:0]counter = 0;
    always @ (posedge clk_125M) begin
        if (rst) begin
            counter <= 0;
            irq <= 0;
        end
        else begin
            counter <= counter + 1;
            if (counter == CNTMAX) begin
                counter <= 0;
                irq <= 1;
            end
            else irq <= 0;
        end
    end
endmodule
