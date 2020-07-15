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

        // 512*32(4KB) boot rom: 0x00000000 to 0x000007fc
        output reg [8:0]bootm_a,
        input [31:0]bootm_spo,

        // 512*32(4KB) main memory: 0x10000000 to 0x100007fc
        output reg [13:0]mainm_a,
        output reg [31:0]mainm_d,
        output reg mainm_we,
        input [31:0]mainm_spo,

        // 12*32 gpio: 0x20000000 to 0x20000030
        output reg [3:0]gpio_a,
        output reg [31:0]gpio_d,
        output reg gpio_we,
        input [31:0]gpio_spo,

        // fifo uart: 
        // 0x30000000
        output reg [2:0]uart_a,
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
        // counter 0x50000000
        // RNG 0x50000004
        // TODO
        output reg [1:0]special_a,
        output reg [31:0]special_d,
        output reg special_we,
        input [31:0]special_spo,

        // SD card 0
        // 0x60000000
        output reg [15:0]sd_a,
        output reg [31:0]sd_d,
        output reg sd_we,
        input [31:0]sd_spo,

        // interrupt service routine: 0x80000000
        // there's another mapper inside the ISR unit
        // so pass the full address
        output reg [31:0]isr_a,
        output reg [31:0]isr_d,
        output reg isr_we,
        input [31:0]isr_spo,

        output reg irq
    );

    always @ (*) begin 
        bootm_a = a[10:2];
        mainm_a = a[15:2];
        mainm_d = d;
        gpio_a = a[5:2];
        gpio_d = d;
        uart_a = a[4:2];
        uart_d = d;
        sd_a = a[15:0];
        sd_d = d;
        isr_a = a;
        isr_d = d;
    end

    always @ (*) begin
        irq = 0;
        mainm_we = 0;
        gpio_we = 0;
        uart_we = 0;
        sd_we = 0;
        isr_we = 0;
        case (a[31:28])
            0: begin spo = bootm_spo; end
            1: begin spo = mainm_spo; mainm_we = we; end
            2: begin spo = gpio_spo; gpio_we = we; end
            3: begin spo = uart_spo; uart_we = we; end
            6: begin spo = sd_spo; sd_we = we; end
            8: begin spo = isr_spo; isr_we = we; end
            default: irq = 1;
        endcase
    end
endmodule
