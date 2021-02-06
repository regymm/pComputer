`timescale 1ns / 1ps
// pComputer interrupt control unit

module interrupt_unit
    (
        input clk,
        input rst,

        output interrupt,
		output int_istimer,
        input int_reply,

        input i_timer,
        input i_uart,
        input i_gpio,
        //input irq_sdcard,

        input [2:0]a,
        input [31:0]d,
        input we,
        output reg [31:0]spo
    );


	reg i_timer_mask;
	reg i_uart_mask;
	reg i_gpio_mask;

    // handler & memory control
    always @ (*) begin
        case (a[2:0])
			3'b0: spo = {29'b0, i_gpio_mask, i_uart_mask, i_timer_mask};
			default: spo = 0;
        endcase
    end
    always @ (posedge clk) begin
        if (rst) begin
            // default no interrupt
            i_timer_mask <= 1;
            i_uart_mask <= 1;
            i_gpio_mask <= 1;
        end
        else begin
			if (we) case (a[2:0])
				3'b0: {i_gpio_mask, i_uart_mask, i_timer_mask} <= d[26:24];
				default: ;
			endcase
        end
    end

	reg int_reply_reg;
	reg i_timer_reg;
	reg i_uart_reg;
	reg i_gpio_reg;
	always @ (posedge clk) begin
		int_reply_reg <= int_reply;
		i_timer_reg <= i_timer;
		i_uart_reg <= i_uart;
		i_gpio_reg <= i_gpio;
	end

    reg i_timer_save = 0;
    reg i_uart_save = 0;
    reg i_gpio_save = 0;

	reg int_istimer_reg = 0;

	localparam IDLE = 1'b0;
	localparam ISSUE = 1'b1;
	//localparam REPLY = 2'b10;
	//localparam END = 2'b11;
	reg [0:0]state = IDLE;

    // interrupt sendout & receive control
    always @ (posedge clk) begin
        if (rst) begin
            i_timer_save <= 0;
            i_uart_save <= 0;
            i_gpio_save <= 0;
			int_istimer_reg <= 0;
			state <= IDLE;
        end
        else begin
            if (i_timer_reg & !i_timer_mask) i_timer_save <= 1;
            if (i_uart_reg & !i_uart_mask) i_uart_save <= 1;
            if (i_gpio_reg & !i_gpio_mask) i_gpio_save <= 1;

			case (state) 
				IDLE: begin
					// priority defined here
					if (i_timer_save) begin
						state <= ISSUE;
						i_timer_save <= 0;
						int_istimer_reg <= 1;
					end else if (i_uart_save) begin
						state <= ISSUE;
						i_uart_save <= 0;
					end else if (i_gpio_save) begin
						state <= ISSUE;
						i_gpio_save <= 0;
					end
				end
				ISSUE: begin
					if (int_reply_reg) begin
						int_istimer_reg <= 0;
						state <= IDLE;
					end
				end
			endcase
        end
    end

	assign interrupt = (state == ISSUE); 
	assign int_istimer = int_istimer_reg;
	
endmodule
