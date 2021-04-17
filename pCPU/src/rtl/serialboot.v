/**
 * File              : serialboot.v
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.04.17
 * Last Modified Date: 2021.04.17
 */

// transfer UART input directly into PSRAM
// CPU will hang waiting during the process
// 
// UART input format: hexadecimal 0-9a-f, all
//  glued together, any illegal character stop
//  transfer

module serialboot(
	input clk,
	input rst,

	input [2:0]a,
	input [31:0]d,
	input we,
	output ready,

	input [7:0]uart_data,
	input uart_ready,

	output [31:0]mem_a,
	output [31:0]mem_d,
	output mem_we,
	input mem_ready
	);

	wire [3:0]uart_data_bin;
	always @ (*) begin
		uart_data_bin = 4'hF;
		if (uart_data >= 8'd48 && uart_data <= 8'd57)
			uart_data_bin = uart_data - 8'd48;
		else if (uart_data >= 8'd97 && uart_data <= 8'd102)
			uart_data_bin = uart_data - 8'd97;
	end

	reg [31:0]mem_start_addr;

	reg [2:0]uart_byte_cnt;
	reg [3:0]uart_byte[7:0];
	always @ (posedge clk) begin
		if (rst) begin
			uart_byte_cnt <= 0;
		end else begin
			if (uart_ready) begin
				uart_byte[uart_byte_cnt] <= uart_data_bin;
				uart_byte_cnt <= uart_byte_cnt + 1;
			end
		end
	end

	wire mem_write_new = uart_byte_cnt == 7 && uart_ready;
	wire mem_write_data = {uart_byte[6], uart_byte[7], uart_byte[4], uart_byte[5], uart_byte[2], uart_byte[3], uart_byte[0], uart_byte[1]};
	always @ (posedge clk) begin
		if (rst) begin
		end else begin
			if (mem_write_new) begin
			end
		end
	end
	wire transferring;
endmodule
