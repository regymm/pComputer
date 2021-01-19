/**
 * File              : pcpu_main.v
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2020.11.25
 * Last Modified Date: 2021.01.03
 */
`timescale 1ns / 1ps
// pComputer main block design
`include "pCPU.vh"

module pcpu_main
    (
        input sysclk,
        
        input [1:0]sw,
        input [1:0]btn,
        output [3:0]led,

		output psram_ce,
		inout psram_mosi, 
		inout psram_miso, 
		inout psram_sio2,
		inout psram_sio3,
		output psram_sclk,

        input uart_rx,
        output uart_tx,

        input sd_ncd,
        input sd_wp,
        input sd_dat0,
        output sd_dat1,
        output sd_dat2,
        output sd_dat3,
        output sd_cmd,
        output sd_sck,

        output [2:0]TMDSp,
        output [2:0]TMDSn,
        output TMDSp_clock,
        output TMDSn_clock
    );

    //reg [1:0]sw_r;
    //reg [3:0]btn_r;
    //reg [3:0]led_r;
    //reg [2:0]ledrgb1_r;
    //reg [2:0]ledrgb2_r;

    //reg uart_rx_r;
    //reg uart_tx_r;

    wire clk_main;
    wire clk_hdmi_25;
    wire clk_hdmi_250;
    clock_wizard clock_wizard_inst(
        .clk_in1(sysclk),
        .clk_main(clk_main),
        .clk_hdmi_25(clk_hdmi_25),
        .clk_hdmi_250(clk_hdmi_250)
    );


    wire [1:0]sw_d;
    debounce #(.N(2)) debounce_inst_0(
        .clk(clk_main),
        .i_btn(sw),
        .o_state(sw_d)
    );

    wire [1:0]btn_d;
    debounce #(.N(2)) debounce_inst_1(
        .clk(clk_main),
        .i_btn(btn),
        .o_state(btn_d)
    );


    // reset
    wire rst = sw_d[0];


    // bootrom 1024*32
    wire [9:0]bootm_a;
	wire bootm_rd;
    wire [31:0]bootm_spo;
	wire bootm_ready;
	clocked_rom #(
		.WIDTH(32),
		.DEPTH(10),
		.INIT("/home/petergu/MyHome/pComputer/pseudOS/coe/result_bootrom.dat")
	) bootrom(
		.clk(clk_main),
        .a(bootm_a),
		.rd(bootm_rd),
        .spo(bootm_spo),
		.ready(bootm_ready)
	);

    
    // distributed ram 4096*32
    wire [11:0]distm_a;
    wire [31:0]distm_d;
    wire distm_we;
	wire distm_rd;
    wire [31:0]distm_spo;
	wire distm_ready;
	simple_ram #(
		.WIDTH(32),
		.DEPTH(12),
		.INIT("/home/petergu/MyHome/pComputer/pseudOS/coe/result_zeros.dat")
	) distram (
        .clk(clk_main),
        .a(distm_a),
        .d(distm_d),
        .we(distm_we),
		.rd(distm_rd),
        .spo(distm_spo),
		.ready(distm_ready)
    );

    
    // gpio
    wire [3:0]gpio_a;
    wire [31:0]gpio_d;
    wire gpio_we;
    wire [31:0]gpio_spo;
`ifdef GPIO_EN
    gpio gpio_inst(
        .clk(clk_main),
        .rst(rst),

        .a(gpio_a),
        .d(gpio_d),
        .we(gpio_we),
        .spo(gpio_spo),

        .btn(btn_d),
        .sw(sw_d),
        .led(led)
    );
`else
	assign gpio_spo = 0;
	assign led = 4'b0;
`endif


    // uart
    wire [2:0]uart_a;
    wire [31:0]uart_d;
    wire uart_we;
    wire [31:0]uart_spo;

    wire irq_uart;
`ifdef UART_EN
    uart uart_inst(
        .clk(clk_main),
        .rst(rst),

        .a(uart_a),
        .d(uart_d),
        .we(uart_we),
        .spo(uart_spo), 

        .tx(uart_tx),
        .rx(uart_rx),

        .irq(irq_uart)
    );
`else
	assign uart_spo = 0;
	assign uart_tx = 1;
`endif


    // sdcard
    wire [15:0]sd_a;
    wire [31:0]sd_d;
    wire sd_we;
    wire [31:0]sd_spo;

    wire irq_sd;
`ifdef SDCARD_EN
    sdcard sdcard_inst(
        .clk(clk_main),
        .rst(rst),

        .a(sd_a),
        .d(sd_d),
        .we(sd_we),
        .spo(sd_spo),

        .sd_dat0(sd_dat0),
        .sd_ncd(sd_ncd),
        .sd_dat1(sd_dat1),
        .sd_dat2(sd_dat2),
        .sd_dat3(sd_dat3),
        .sd_cmd(sd_cmd),
        .sd_sck(sd_sck),

        .irq(irq_sd) // nc
    );
`else
	assign sd_spo = 0;
	assign irq_sd = 0;
	assign sd_dat1 = 1'bZ;
	assign sd_dat2 = 1'bZ;
	assign sd_dat3 = 1'bZ;
	assign sd_cmd = 1'bZ;
	assign sd_sck = 1'bZ;
`endif

	wire [31:0]mainm_a;
	wire [31:0]mainm_d;
	wire mainm_we;
	wire mainm_rd;
	wire [31:0]mainm_spo;
	wire mainm_ready;

	wire mainm_irq;
`ifdef PSRAM_EN
	memory_controller memory_controller_inst
	(
		.rst(rst),
		.clk(clk_main),

		.a(mainm_a),
		.d(mainm_d),
		.we(mainm_we),
		.rd(mainm_rd),
		.spo(mainm_spo),
		.ready(mainm_ready), 

		.irq(mainm_irq),

		.psram_ce(psram_ce), 
		.psram_mosi(psram_mosi), 
		.psram_miso(psram_miso), 
		.psram_sio2(psram_sio2), 
		.psram_sio3(psram_sio3),
		.psram_sclk(psram_sclk)
	);
`else
`endif


    //// sdcard memory mapper
    //wire [31:0]sdmm_a;
    //wire [31:0]sdmm_d;
    //wire sdmm_we;
    //wire sdmm_rd;
    //wire [31:0]sdmm_spo;
    //wire sdmm_ready;

    //wire irq_sdmm;
    //sdmm sdmm_inst(
        //.clk(clk_main),
        //.rst(rst),

        //.a(sdmm_a),
        //.d(sdmm_d),
        //.we(sdmm_we),
        //.rd(sdmm_rd),
        //.spo(sdmm_spo),
        //.ready(sdmm_ready),

        //.sddrv_spo(sd_spo),
        //.sddrv_a(sd_a),
        //.sddrv_d(sd_d),
        //.sddrv_we(sd_we),

        //.irq(irq_sdmm)
    //);


    // video
    wire [31:0]video_a;
    wire [31:0]video_d;
    wire video_we;
    wire [31:0]video_spo;
`ifdef VIDEO_EN
	mkrvidor4000_top mkrvidor4000_top_inst(
		.clk(clk_main),
		.clk_pix(clk_hdmi_25),
		.clk_tmds(clk_hdmi_250),
		.rst(rst),

		.a(video_a),
		.d(video_d),
		.we(video_we),
		.spo(video_spo),

		.TMDSp(TMDSp),
		.TMDSn(TMDSn),
		.TMDSp_clock(TMDSp_clock),
		.TMDSn_clock(TMDSn_clock)
	);
`else
	assign video_spo = 0;
    OBUFDS OBUFDS_red(
        .I(0),
        .O(TMDSp[2]),
        .OB(TMDSn[2])
    );
    OBUFDS OBUFDS_green(
        .I(0),
        .O(TMDSp[1]),
        .OB(TMDSn[1])
    );
    OBUFDS OBUFDS_blue(
        .I(0),
        .O(TMDSp[0]),
        .OB(TMDSn[0])
    );
    OBUFDS OBUFDS_clock(
        .I(0),
        .O(TMDSp_clock),
        .OB(TMDSn_clock)
    );
`endif



    // interrupt unit
    wire iack;
    wire irq;
    wire [3:0]icause;

    wire [31:0]isr_a;
    wire [31:0]isr_d;
    wire isr_we;
    wire [31:0]isr_spo;
`ifdef IRQ_EN
    // timer interrupt
    wire irq_timer;
    timer_interrupt timer_interrupt_inst(
        .clk_125M(clk_main),
        .rst(rst),
        .irq(irq_timer)
    );

    interrupt_unit interrupt_unit_inst(
        .clk(clk_main),
        .rst(rst),

        .iack(iack),
        .irq(irq),
        .icause(icause),

        .irq_timer(irq_timer),
        .irq_keyboard(irq_uart),
        .irq_sdcard(irq_sd),

        .a(isr_a),
        .d(isr_d),
        .we(isr_we),
        .spo(isr_spo)
    );
`else
	assign irq = 0;
	assign icause = 0;
	assign isr_spo = 0;
`endif

    // cpu-multi-cycle
    wire [31:0]spo;
    wire ready;
    wire [31:0]a;
    wire [31:0]d;
    wire we;
    wire rd;

	wire [1:0]ring;
	riscv_multicyc riscv_multicyc_inst(
		.clk(clk_main),
		.rst(rst),

		.irq(irq),
		.icause(icause),
		.iack(iack),

		.spo(spo),
		.ready(ready),
		.a(a),
		.d(d),
		.we(we),
		.rd(rd),

		.ring(ring)
	);


    // MMU
    wire virq;
    
    wire [31:0]pspo;
    wire pready;
    wire pirq;
    wire [31:0]pa;
    wire [31:0]pd;
    wire pwe;
    wire prd;
`ifdef MMU_EN
    mmu mmu_inst(
        .clk(clk_main),
        .rst(rst),

        .ring(ring),

        .va(a),
        .vd(d),
        .vwe(we),
        .vrd(rd),
        .vspo(spo),
        .vready(ready),
        .virq(virq), // nc

        .pspo(pspo),
        .pready(pready),
        .pa(pa),
        .pd(pd),
        .pwe(pwe),
        .prd(prd)
    );
`else
	assign virq = 0;
	assign pa = a;
	assign pd = d;
	assign pwe = we;
	assign prd = rd;
	assign spo = pspo;
	assign ready = pready;
`endif

    // memory mapper
    mmapper mmapper_inst(
        .a(pa),
        .d(pd),
        .we(pwe),
        .rd(prd),
        .spo(pspo),
        .ready(pready),

        .isr_a(isr_a),
        .isr_d(isr_d),
        .isr_we(isr_we),
        .isr_spo(isr_spo),

        .bootm_a(bootm_a),
		.bootm_rd(bootm_rd),
        .bootm_spo(bootm_spo),
		.bootm_ready(bootm_ready),

        .distm_a(distm_a),
        .distm_d(distm_d),
        .distm_we(distm_we),
		.distm_rd(distm_rd),
        .distm_spo(distm_spo),
		.distm_ready(distm_ready),

		.mainm_a(mainm_a),
		.mainm_d(mainm_d),
		.mainm_we(mainm_we),
		.mainm_rd(mainm_rd),
		.mainm_spo(mainm_spo),
		.mainm_ready(mainm_ready),

        .sd_spo(sd_spo),
        .sd_a(sd_a),
        .sd_d(sd_d),
        .sd_we(sd_we),

        .gpio_spo(gpio_spo),
        .gpio_a(gpio_a),
        .gpio_d(gpio_d),
        .gpio_we(gpio_we),

        .uart_spo(uart_spo),
        .uart_a(uart_a),
        .uart_d(uart_d),
        .uart_we(uart_we),

        .video_spo(video_spo),
        .video_a(video_a),
        .video_d(video_d),
        .video_we(video_we),

        .irq(pirq)
    );
endmodule
