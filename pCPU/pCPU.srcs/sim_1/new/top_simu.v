`timescale 1ns / 1ps
// pComputer top simu

module top_simu();
    reg clk = 0;
    reg [1:0]sw = 0;
    reg [3:0]btn = 0;
    wire [3:0]led;
    wire [2:0]rgbled1;
    wire [2:0]rgbled2;
    wire sd_dat0 = 0;
    //wire sd_dat0 = 1;

    pcpu_main pcpu_main_inst
    (
        .sysclk(clk),
        .btn(btn),
        .led(led),
        .rgbled1(rgbled1),
        .rgbled2(rgbled2),
        .sw(sw),
        .sd_dat0(sd_dat0)
    );
    
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        //#400
        sw = 2'b01;
        //btn = 4'b0000;
        //#2000
        //sw = 2'b10;
        #2000
        sw = 2'b00;

        //#10
        //btn = 4'b0000;

        //#1000
        //btn = 4'b0010;

        #32000
        $finish;
    end
    
endmodule
