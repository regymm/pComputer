`timescale 1ns / 1ps
// pComputer top simu
`define SIMULATION

module top_simu();
    reg clk = 0;
    reg [1:0]sw = 0;
    reg [1:0]btn = 0;
    wire [3:0]led;
    wire sd_dat0 = 0;
    //wire sd_dat0 = 1;
	wire psram_ce;
	wire psram_mosi;
	wire psram_miso;
	wire psram_sio2;
	wire psram_sio3;
	wire psram_sclk;

    pcpu_main pcpu_main_inst
    (
        .sysclk(clk),
        .btn(btn),
        .led(led),
        .sw(sw),
        .sd_dat0(sd_dat0),
		.psram_ce(psram_ce),
		.psram_mosi(psram_mosi),
		.psram_miso(psram_miso),
		.psram_sio2(psram_sio2),
		.psram_sio3(psram_sio3),
		.psram_sclk(psram_sclk)
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
        #4000
        sw = 2'b00;

        //#10
        //btn = 4'b0000;

        //#1000
        //btn = 4'b0010;

        #200000
		sw = 2'b01;
		#4000
		sw = 2'b00;
		#200000
        $finish;
    end
    
endmodule
