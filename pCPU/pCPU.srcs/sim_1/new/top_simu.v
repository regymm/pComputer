`timescale 1ns / 1ps
// pComputer top simu

module top_simu();
    reg clk = 0;
    reg [1:0]sw = 0;
    reg [3:0]btn = 0;
    wire [3:0]led;
    wire [2:0]led4;
    wire [2:0]led5;
    wire sd_dat0 = 0;
    //wire sd_dat0 = 1;

    top_design_wrapper top_design_wrapper_inst
    (
        .sysclk(clk),
        .btn(btn),
        .led(led),
        .led4(led4),
        .led5(led5),
        .sw(sw),
        .sd_dat0(sd_dat0)
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
        //#400
        //sw = 2'b10;
        //btn = 4'b0000;
        //#200
        sw = 2'b11;
        #500
        sw = 2'b00;

        //#10
        //btn = 4'b0000;

        //#1000
        //btn = 4'b0010;

        #32000
        $finish;
    end
    
endmodule
