`timescale 1ns / 1ps
// pComputer multi-cycle CPU -- coprocessor0(interrupt/exception handler)

module coprocessor0
    (
        input clk,
        input rst,
        
        input [1:0]Mfc0Src,
        input EPCSrc,
        input CauseSrc,
        input StatusSrc,
        input EPCWrite,
        input CauseWrite,
        input StatusWrite,

        input [31:0]pc,
        input [31:0]current_pc,
        input [3:0]causedata_outside,
        input [31:0]mtc0in,
        output reg [31:0]mfc0out,
        output wire [31:0]epc_out,
        output wire [31:0]status_out
    );

    // cp0 registers
    reg [31:0]cause = 0;
    reg [31:0]status = 0;
    reg [31:0]epc = 0;

    // cp0 signals
    reg [31:0]CauseData;
    reg [31:0]StatusData;
    reg [31:0]EPCData;
    assign epc_out = epc;
    assign status_out = status;

    // datpath -- coprocessor 0
    always @ (*) begin
        case (Mfc0Src)
            0: mfc0out = epc;
            1: mfc0out = cause;
            2: mfc0out = status;
            default: mfc0out = 0;
        endcase
        case (CauseSrc)
            0: CauseData = {28'b0, causedata_outside};  // timer, ...
            1: CauseData = 1;                           // syscall
            //2: CauseData = mtc0in;
            default: ;
        endcase
        case (StatusSrc)
            0: StatusData = {status[15:0], 16'b0};  // syscall/interrupt
            1: StatusData = {16'b0, status[31:16]}; // eret
            //2: StatusData = mtc0in;
        endcase
        case (EPCSrc)
            0: EPCData = pc;            // syscall: return to next instr
            1: EPCData = current_pc;    // interrupt: return to current instr
            //2: EPCData = mtc0in;
        endcase
    end
    always @ (posedge clk) begin
        if (rst) begin
            epc <= 0;
            cause <= 0;
            status <= 32'b1111;
        end
        else begin
            if (EPCWrite) epc <= EPCData;
            if (CauseWrite) cause <= CauseData;
            if (StatusWrite) status <= StatusData;
        end
    end
endmodule
