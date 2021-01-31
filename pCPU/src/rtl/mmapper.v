/**
 * File              : mmapper.v
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.01.24
 * Last Modified Date: 2021.01.24
 */
`timescale 1ns / 1ps
// pCPU memory address mapper (or "bus")

module mmapper
    (
        (*mark_debug = "true"*)input [31:0]a,
        (*mark_debug = "true"*)input [31:0]d,
        (*mark_debug = "true"*)input we,
        (*mark_debug = "true"*)input rd,
        (*mark_debug = "true"*)output reg [31:0]spo,
        (*mark_debug = "true"*)output reg ready,

        // 4096*32(32KB) distributed memory: 0x10000000 to 0x10007ffc
        output reg [11:0]distm_a,
        output reg [31:0]distm_d,
        output reg distm_we,
		output reg distm_rd,
        input [31:0]distm_spo,
		input distm_ready,

		// 8MB PSRAM: 0x20000000 to 0x21fffffc
        output reg [21:0]mainm_a,
        output reg [31:0]mainm_d,
        output reg mainm_we,
        output reg mainm_rd,
        input [31:0]mainm_spo,
        input mainm_ready,

        //// special devices:
        //// counter 0x50000000
        //// RNG 0x50000004
        //// this should be moved into cp0
        //output reg [1:0]special_a = 0,
        //output reg [31:0]special_d = 0,
        //output reg special_we = 0,
        //input [31:0]special_spo,


        // interrupt service routine: 0x80000000
        // there's another mapper inside the ISR unit
        // so pass the full address
        output reg [31:0]isr_a,
        output reg [31:0]isr_d,
        output reg isr_we,
        input [31:0]isr_spo,

        // MMIO devices
        // 
        // gpio: 0x92000000
        output reg [3:0]gpio_a,
        output reg [31:0]gpio_d,
        output reg gpio_we,
        input [31:0]gpio_spo,

        // uart: 0x93000000
        output reg [2:0]uart_a,
        output reg [31:0]uart_d,
        output reg uart_we,
        input [31:0]uart_spo,

        // vram: 0x9400000 to 0x9400
        output reg [31:0]video_a = 0,
        output reg [31:0]video_d = 0,
        output reg video_we = 0,
        input [31:0]video_spo,

        // SD card control: 0x96000000
        output reg [31:0]sd_a,
        output reg [31:0]sd_d,
        output reg sd_we,
        //output reg sd_rd,
        input [31:0]sd_spo,
        //input sd_ready,

		// CH375b: 0x97000000
		output reg [2:0]usb_a,
		output reg [31:0]usb_d,
		output reg usb_we,
		input [31:0]usb_spo,

        
        // 0xe0000000 MMU control

        // 1024*32(8KB) boot rom: 0xf0000000 to 0xf00007fc
        output reg [9:0]bootm_a,
		output reg bootm_rd,
        input [31:0]bootm_spo,
		input bootm_ready,


        output reg irq
    );

    always @ (*) begin 
        bootm_a = a[11:2];
        distm_a = a[13:2];
        distm_d = d;
		mainm_a = a[23:2];
		mainm_d = d;
        gpio_a = a[5:2];
        gpio_d = d;
        uart_a = a[4:2];
        uart_d = d;
        video_a = a;
        video_d = d;
        sd_a = a[31:0];
        sd_d = d;
		usb_a = a[4:2];
		usb_d = d;
        isr_a = a;
        isr_d = d;
    end

    always @ (*) begin
        irq = 0;
        distm_we = 0;
		mainm_we = 0;
		mainm_rd = 0;
        gpio_we = 0;
        uart_we = 0;
        video_we = 0;
        sd_we = 0;
		usb_we = 0;
        isr_we = 0;
		bootm_rd = 0;
        spo = 0;
        ready = 1;
        if (a[31:28] == 4'h1) begin
            distm_we = we;
			distm_rd = rd;
            spo = distm_spo;
			ready = distm_ready;
        end else if (a[31:28] == 4'h2) begin
            mainm_we = we;
            mainm_rd = rd;
            spo = mainm_spo;
			ready = mainm_ready;
        end else if (a[31:28] == 4'h8) begin
            spo = isr_spo;
            isr_we = we;
        end else if (a[31:28] == 4'h9) begin
            case (a[27:24])
                4'h2: begin
                    spo = gpio_spo;
                    gpio_we = we;
                end
                4'h3: begin
                    spo = uart_spo;
                    uart_we = we;
                end
                4'h4: begin
                    spo = video_spo;
                    video_we = we;
                end
                4'h6: begin
                    spo = sd_spo;
                    sd_we = we;
                end
                4'h7: begin
                    spo = usb_spo;
                    usb_we = we;
                end
                default: irq = 1;
            endcase
        end else if (a[31:28] == 4'hf) begin
			bootm_rd = rd;
            spo = bootm_spo;
			ready = bootm_ready;
        end
        else irq = 1;
        //case (a[31:28])
            //0: 
            //1: 
            //2: begin end
            //3: 
            //6: begin spo = sd_spo; sd_we = we; end
            //7: 
            //8: begin spo = isr_spo; isr_we = we; end
            //default: irq = 1;
        //endcase
    end
endmodule
