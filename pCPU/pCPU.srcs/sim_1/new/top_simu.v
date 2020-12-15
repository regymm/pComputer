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
        .rgbled1(rgbled1),
        .rgbled2(rgbled2),
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
        #2000
        sw = 2'b00;

        //#10
        //btn = 4'b0000;

        //#1000
        //btn = 4'b0010;

        #1220000
        $finish;
    end
    
endmodule
