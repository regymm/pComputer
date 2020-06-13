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

        input [3:0]btn,
        input [1:0]sw,
        output reg [3:0]led,
        output reg [2:0]rgbled1,
        output reg [2:0]rgbled2
    );

    always @ (*) begin
        case (a)
            0: spo = {31'b0, btn[0]};
            1: spo = {31'b0, btn[1]};
            2: spo = {31'b0, btn[2]};
            3: spo = {31'b0, btn[3]};
            4: spo = {31'b0, sw[0]};
            5: spo = {31'b0, sw[1]};
            6: spo = {31'b0, led[0]};
            7: spo = {31'b0, led[1]};
            8: spo = {31'b0, led[2]};
            9: spo = {31'b0, led[3]};
            10: spo = {29'b0, rgbled1};
            11: spo = {29'b0, rgbled2};
            default: spo = 32'b0;
        endcase
    end

    always @ (posedge clk) begin
        if (rst) begin
            led <= 4'b1111;
            rgbled1 <= 3'b0;
            rgbled2 <= 3'b0;
        end
        else if (we) begin
            case (a)
                6: led[0] <= d[0];
                7: led[1] <= d[0];
                8: led[2] <= d[0];
                9: led[3] <= d[0];
                10: rgbled1 <= d[2:0];
                11: rgbled2 <= d[2:0];
                default: ;
            endcase
        end
    end
endmodule
