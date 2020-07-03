`timescale 1ns / 1ps
// pComputer top simu

module top_simu();
    reg clk = 0;
    reg [1:0]sw = 0;
    reg [3:0]btn = 0;
    wire [3:0]led;
    wire [2:0]led4;
    wire [2:0]led5;

    top_design_wrapper top_design_wrapper_inst
    (
        .sysclk(clk),
        .btn(btn),
        .led(led),
        .led4(led4),
        .led5(led5),
        .sw(sw)
    );
    
    //top top_inst
    //(
        //.sysclk(clk),
        //.sw(sw),
        //.led4_b(led4_b),
        //.led4_g(led4_g),
        //.led4_r(led4_r),
        //.led5_b(led5_b),
        //.led5_g(led5_g),
        //.led5_r(led5_r),
        //.led(led),
        //.btn(btn)
    //);

    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        sw = 2'b11;
        btn = 4'b0000;
        #200
        sw = 2'b00;

        //#10
        //btn = 4'b0000;

        //#1000
        //btn = 4'b0010;

        #30000
        $finish;
    end
    
endmodule
