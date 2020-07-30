`timescale 1ns / 1ps
// pComputer primitive MMU
// single level page table
// 1MB page: 23 bit offset, total 256 pages(per instance)
// VA:
// [31:28] memory section selection
// [27:20] VPN
// [19:0] offset
// PTE:
// [31:24] PPN
// [23:3] Unused
// [1] Privileged mode
// [0] Present

module mmu
    (
        input clk, 
        input rst, 

        input [31:0]va,
        input [31:0]vd,
        input vwe, 
        input vrd, 
        output reg [31:0]vspo, 
        output reg vready,

        output reg [31:0]pa,
        output reg [31:0]pd,
        output reg pwe, 
        output reg prd, 
        input [31:0]pspo, 
        input pready
    );

    reg [31:0]ptbr = 0; // page table base register
    reg enabled = 0;

    localparam IDLE = 4'h0;
    localparam CHECK_RANGE = 4'h1;
    localparam READ_PT = 4'h2;
    localparam MEM_WAIT = 4'hf;
    reg [3:0]phase = IDLE;

    always @ (posedge clk) begin
        if (rst) begin
            ptbr <= 0;
            enabled <= 0;
            phase <= IDLE;
        end
        else begin

        end
    end

    always @ (*) begin
        if (!enabled) begin
            pa = va;
            pd = vd;
            pwe = vwe; 
            prd = vrd;
            vspo = pspo;
            vready = pready;
        end
    end

endmodule
