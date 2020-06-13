`timescale 1ns / 1ps
// SPI mode SD card driver


module sdcard
    (
        input clk,
        input rst,

        output reg cs, // SD_DAT[3]
        output mosi, // SD_CMD
        input miso, // SD_DAT[0]
        output sclk, // SD_SCLK
        // SD_DAT[2] and SD_DAT[1] high, SD_RESET low
        
        output ready,
        input [31:0]address,
        input rd,
        input wr,
        input [7:0]din,
        output reg [7:0]dout,
        output reg byte_available,
        output reg ready_for_next_byte

        //input SD_CD,
        //output SD_RESET,
        //output SD_SCK,
        //output SD_CMD,
        //input [3:0]SD_DAT
    );
endmodule
