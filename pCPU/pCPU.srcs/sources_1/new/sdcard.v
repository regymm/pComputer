`timescale 1ns / 1ps
// pComputer memory mapped SPI mode SD card driver
//
//  3.3V          ----+++              wp
//                    |||         +---- =-----
//                    RRR         =8
//  MISO(SD_DAT[0]) --|||---------=7
//                    |||  GND ---=6
//  SCLK(SD_CLK)    --||+---------=5
//                    ||   VCC ---=4
//                    ||   GND ---=3
//  MOSI(SD_CMD)    --|+----------=2
//  CS(SD_DAT[3])   --+-----------=1
//                                  +=9
//                                    +-------
//
// *addresses have already x4
// read/write 0x0000 to 0x07fc: 512*8 R/W block cache
// write 0x1000: set <address> for R/W, auto 512 aligned (may lost changes)
// write 0x1004: do a read at <address> (may lost changes)
// write 0x1008: do a write to <address>
// ~~write 0x1010: sync~~
// read 0x2000: negative card detect
// read 0x2004: write protected
// read 0x2010: ready, used for polling
// read 0x2014: dirty?
// ~~read/write 0x2000: <auto sync>~~

module sdcard
    (
        input clk,
        input clk_slow,
        input rst,

        input sd_dat0,
        input sd_ncd,
        input sd_wp,
        output sd_dat1,
        output sd_dat2,
        output sd_dat3,
        output sd_cmd,
        output sd_sck,
        // SD_DAT[2] and SD_DAT[1] high, SD_RESET low
        
        // memory interface
        input [15:0]a,
        input [31:0]d,
        input we,
        output reg [31:0]spo,

        output reg irq = 0
    );

    // input 125MHz
    wire clk_2;
    wire clk_4;
    wire clk_8;
    wire clk_16;
    wire clk_32;
    wire clk_64; // this is 1.95MHz
    wire clk_128;
    wire clk_256; // this is 488KHz
    wire clk_512; // this is 244KHz
    clock_divider div1(clk, clk_2);
    clock_divider div2(clk_2, clk_4);
    clock_divider div3(clk_4, clk_8);
    clock_divider div4(clk_8, clk_16);
    clock_divider div5(clk_16, clk_32);
    clock_divider div6(clk_32, clk_64);
    //clock_divider div7(clk_64, clk_128);
    //clock_divider div8(clk_128, clk_256);
    //clock_divider div9(clk_256, clk_512);

    assign sd_dat1 = 1;
    assign sd_dat2 = 1;
    //assign sd_reset = 0;

    reg [31:0]sd_address = 0;
    reg [7:0]block[511:0];
    reg dirty = 0;

    reg sd_rd = 1; // tmp
    wire [7:0]sd_dout;
    wire sd_readnext;
    reg sd_wr = 0;
    reg [7:0]sd_din = 0;
    wire sd_writenext;
    wire sd_ready;
    wire [4:0]sd_status;
    sd_controller sd_controller_inst
    (
        .clk(clk_64),
        .reset(rst),

        .cs(sd_dat3),
        .mosi(sd_cmd),
        .miso(sd_dat0),
        .sclk(sd_sck),

        .address(sd_address),

        .rd(sd_rd),
        .dout(sd_dout),
        .byte_available(sd_readnext),
        .wr(sd_wr),
        .din(sd_din),
        .ready_for_next_byte(sd_writenext),

        .ready(sd_ready),

        .status(sd_status)
    );

    // manual slow clock posedge detection
    reg sd_ready_old = 0;
    reg sd_readnext_old = 0;
    reg sd_writenext_old = 0;
    reg sd_ready_oold = 0;
    reg sd_readnext_oold = 0;
    reg sd_writenext_oold = 0;
    always @ (posedge clk) begin
        sd_ready_old <= sd_ready;
        sd_readnext_old <= sd_readnext;
        sd_writenext_old <= sd_writenext;
        sd_ready_oold <= sd_ready_old;
        sd_readnext_oold <= sd_readnext_old;
        sd_writenext_oold <= sd_writenext_old;
    end
    wire sd_ready_posedge = !sd_ready_oold & sd_ready_old;
    wire sd_readnext_posedge = !sd_readnext_oold & sd_readnext_old;
    wire sd_writenext_posedge = !sd_writenext_oold & sd_writenext_old;

    reg reading = 0;
    reg writing = 0;

    reg [9:0]counter = 0;

    always @ (posedge clk) begin
        if (rst) begin
            sd_address <= 0;
            dirty <= 0;
            reading <= 0;
            writing <= 0;
        end
        begin
            if (sd_ready) begin
                if (we) begin
                    case (a)
                        16'h1000: sd_address <= d;
                        16'h1004: sd_rd <= d[0];
                        16'h1008: sd_wr <= d[0];
                        default: ;
                    endcase
                end
                // send/receive data has finished
                reading <= 0;
                writing <= 0;
            end
            else begin
                // sdcard has received signal and not IDLE
                // so we can stop rd/wr signals
                // and prepare to send/receive data
                if (sd_rd) begin
                    sd_rd <= 0;
                    reading <= 1; counter <= 0;
                end
                if (sd_wr) begin
                    sd_wr <= 0;
                    writing <= 1; counter <= 0;
                    dirty <= 0;
                end
            end

            if (reading & sd_readnext_posedge) begin
                block[counter] <= sd_dout;
                counter <= counter + 1;
            end
            else if (writing & sd_writenext_posedge) begin
                sd_din <= block[counter];
                counter <= counter + 1;
            end
            else if (we & (a[15:12] == 0)) begin
                block[a[10:2]] <= d;
                dirty <= 1;
            end
        end
    end

    // handle non-relevant reading
    always @ (*) begin
        if (a[15:12] == 0) spo = block[a[10:2]];
        else case (a)
            16'h2000: spo = sd_ncd;
            16'h2004: spo = sd_wp;
            16'h2010: spo = sd_ready;
            16'h2014: spo = dirty;
            default: ;
        endcase
    end

    // interrupt when ready
    //assign irq = sd_ready_posedge;

endmodule

module clock_divider (
    input clk_in,
    output reg clk_out = 0
);
    always @(posedge clk_in) begin
        clk_out <= ~clk_out;
    end
endmodule
