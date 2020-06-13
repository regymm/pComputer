`timescale 1ns / 1ps
// pCPU memory address mapper

module mmapper
    (
        input [31:0]a,
        input [31:0]d,
        //input [31:0]dpra,
        input we,
        output reg [31:0]spo,
        //output reg [31:0]dpo = 0,

        // 1024*32(4KB) boot rom: 0x00000000 to 0x00000ffc
        output reg [9:0]bootm_a,
        output reg [31:0]bootm_d,
        output reg bootm_we,
        input [31:0]bootm_spo,

        // main memory: 0x10000000 to 0x10000000
        output reg [13:0]mainm_a,
        output reg [31:0]mainm_d,
        output reg mainm_we,
        input [31:0]mainm_spo,

        // 12*32 gpio: 0x20000000 to 0x2000002c
        output reg [3:0]gpio_a,
        output reg [31:0]gpio_d,
        output reg gpio_we,
        input [31:0]gpio_spo,

        // fifo uart: 
        // write 0x30000000
        // full 0x30000004
        // empty 0x30000008
        output reg [1:0]uart_a,
        output reg [31:0]uart_d,
        output reg uart_we,
        input [31:0]uart_spo,

        // 4800*8(80*30*2*8) vram: 0x40000000 to 0x4000
        // TODO
        output reg [12:0]video_a,
        output reg [31:0]video_d,
        output reg video_we,
        input [31:0]video_spo,


        // special devices:
        // counter 0x80000000
        // RNG 0x80000004
        // TODO
        output reg [1:0]special_a,
        output reg [31:0]special_d,
        output reg special_we,
        input [31:0]special_spo,

        output reg error
    );

    always @ (*) begin 
        bootm_a = a[11:2];
        bootm_d = d;
        mainm_a = a[15:2];
        mainm_d = d;
        gpio_a = a[5:2];
        gpio_d = d;
        uart_a = a[3:2];
        uart_d = d;
    end

    always @ (*) begin
        error = 0;
        bootm_we = 0;
        mainm_we = 0;
        gpio_we = 0;
        uart_we = 0;
        case (a[15:12])
            0: begin spo = bootm_spo; bootm_we = we; end
            1: begin spo = mainm_spo; mainm_we = we; end
            2: begin spo = gpio_spo; gpio_we = we; end
            3: begin spo = uart_spo; uart_we = we; end
            default: error = 1;
        endcase
    end
endmodule
