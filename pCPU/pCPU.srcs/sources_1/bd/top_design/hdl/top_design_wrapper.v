//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2019.1 (lin64) Build 2552052 Fri May 24 14:47:09 MDT 2019
//Date        : Wed Jul 15 12:43:20 2020
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
    sd_cmd,
    sd_dat0,
    sd_dat1,
    sd_dat2,
    sd_dat3,
    sd_ncd,
    sd_sck,
    sd_wp,
    sw,
    sysclk,
    uart_rx,
    uart_tx);
  input [3:0]btn;
  output [3:0]led;
  output [2:0]led4;
  output [2:0]led5;
  output sd_cmd;
  input sd_dat0;
  output sd_dat1;
  output sd_dat2;
  output sd_dat3;
  input sd_ncd;
  output sd_sck;
  input sd_wp;
  input [1:0]sw;
  input sysclk;
  input uart_rx;
  output uart_tx;

  wire [3:0]btn;
  wire [3:0]led;
  wire [2:0]led4;
  wire [2:0]led5;
  wire sd_cmd;
  wire sd_dat0;
  wire sd_dat1;
  wire sd_dat2;
  wire sd_dat3;
  wire sd_ncd;
  wire sd_sck;
  wire sd_wp;
  wire [1:0]sw;
  wire sysclk;
  wire uart_rx;
  wire uart_tx;

  top_design top_design_i
       (.btn(btn),
        .led(led),
        .led4(led4),
        .led5(led5),
        .sd_cmd(sd_cmd),
        .sd_dat0(sd_dat0),
        .sd_dat1(sd_dat1),
        .sd_dat2(sd_dat2),
        .sd_dat3(sd_dat3),
        .sd_ncd(sd_ncd),
        .sd_sck(sd_sck),
        .sd_wp(sd_wp),
        .sw(sw),
        .sysclk(sysclk),
        .uart_rx(uart_rx),
        .uart_tx(uart_tx));
endmodule
