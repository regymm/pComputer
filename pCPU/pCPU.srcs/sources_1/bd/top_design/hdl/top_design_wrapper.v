//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2019.1 (lin64) Build 2552052 Fri May 24 14:47:09 MDT 2019
//Date        : Fri Jun 12 21:17:57 2020
//Host        : petergu-dell running 64-bit Arch Linux
//Command     : generate_target top_design_wrapper.bd
//Design      : top_design_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module top_design_wrapper
   (btn,
    led,
    led4,
    led5,
    sw,
    sysclk,
    uart_rx,
    uart_tx);
  input [3:0]btn;
  output [3:0]led;
  output [2:0]led4;
  output [2:0]led5;
  input [1:0]sw;
  input sysclk;
  input uart_rx;
  output uart_tx;

  wire [3:0]btn;
  wire [3:0]led;
  wire [2:0]led4;
  wire [2:0]led5;
  wire [1:0]sw;
  wire sysclk;
  wire uart_rx;
  wire uart_tx;

  top_design top_design_i
       (.btn(btn),
        .led(led),
        .led4(led4),
        .led5(led5),
        .sw(sw),
        .sysclk(sysclk),
        .uart_rx(uart_rx),
        .uart_tx(uart_tx));
endmodule
