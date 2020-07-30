`timescale 1ns / 1ps
// pComputer interrupt control unit

module interrupt_unit
    (
        input clk,
        input rst,

        input iack,
        output reg irq,
        output reg [3:0]icause,

        input irq_timer,
        input irq_keyboard,
        input irq_sdcard,

        input [31:0]a,
        input [31:0]d,
        input we,
        output reg [31:0]spo
    );


    // 128*32 ISR ROM, address 0x80000000
    wire [31:0]spo_mem;
    isr_memory isr_memory_inst
    (
        .a(a[10:2]),
        .spo(spo_mem)
    );

    // ISR handler address, the data contained is the address
    // of user-writen handler, default is a dummy return(jr $ra)
    // ISR ROM will check the interrupt type and jump to 
    // corresponding locations
    // eret at 0x8000f000
    wire [31:0]instr_eret = 32'h03e00008;
    // timer: 0x80001000, mask 0x80001004
    reg [31:0]isr_timer_addr;
    reg isr_timer_mask;
    // keyboard: 0x80002000, mask 0x80002004
    reg [31:0]isr_keyboard_addr;
    reg isr_keyboard_mask;
    // sdcard: 0x80003000, mask 0x80003004
    reg [31:0]isr_sdcard_addr;
    reg isr_sdcard_mask;
    // syscall: 0x80008000, mask 0x80008004
    reg [31:0]isr_syscall_addr;
    reg isr_syscall_mask;

    // handler & memory control
    always @ (*) begin
        case (a[15:0])
            16'h1000: spo = isr_timer_addr;
            16'h1004: spo = isr_timer_mask;
            16'h2000: spo = isr_keyboard_addr;
            16'h2004: spo = isr_keyboard_mask;
            16'h3000: spo = isr_sdcard_addr;
            16'h3004: spo = isr_sdcard_mask;
            16'h8000: spo = isr_syscall_addr;
            16'h8004: spo = isr_syscall_mask;
            16'hf000: spo = instr_eret;
            default: spo = spo_mem;
        endcase
    end
    always @ (posedge clk) begin
        if (rst) begin
            // default no interrupt
            isr_timer_addr <= 32'h8000f000;
            isr_timer_mask <= 1;
            isr_keyboard_addr <= 32'h8000f000;
            isr_keyboard_mask <= 1;
            isr_sdcard_addr <= 32'h8000f000;
            isr_sdcard_mask <= 1;
            isr_syscall_addr <= 32'h8000f000;
            isr_syscall_mask <= 1;
        end
        else begin
            if (we & (a == 32'h80001000)) isr_timer_addr <= d;
            if (we & (a == 32'h80001004)) isr_timer_mask <= d[0];
            if (we & (a == 32'h80002000)) isr_keyboard_addr <= d;
            if (we & (a == 32'h80002004)) isr_keyboard_mask <= d[0];
            if (we & (a == 32'h80003000)) isr_sdcard_addr <= d;
            if (we & (a == 32'h80003004)) isr_sdcard_mask <= d[0];
            if (we & (a == 32'h80008000)) isr_syscall_addr <= d;
            if (we & (a == 32'h80008004)) isr_syscall_mask <= d[0];
        end
    end

    reg irq_timer_save;
    reg irq_keyboard_save;
    reg irq_sdcard_save;

    // interrupt sendout control
    always @ (posedge clk) begin
        if (rst) begin
            irq <= 0;
            icause <= 0;
            irq_timer_save <= 0;
            irq_keyboard_save <= 0;
            irq_sdcard_save <= 0;
        end
        else begin
            if (irq_timer) irq_timer_save <= 1;
            if (irq_keyboard) irq_keyboard_save <= 1;
            if (irq_sdcard) irq_sdcard_save <= 1;

            if (irq_timer_save & !isr_timer_mask) begin
                icause <= 8;
                irq <= 1;
                if (iack) irq_timer_save <= 0;
            //end else if (irq_keyboard_save & !isr_keyboard_mask) begin
                //icause <= 9;
                //irq <= 1;
                //if (iack) irq_keyboard_save <= 0;
            //end else if (irq_sdcard_save & !isr_sdcard_mask) begin
                //icause <= 10;
                //irq <= 1;
                //if (iack) irq_sdcard_save <= 0;
            end else begin
                icause <= 0;
                irq <= 0;
            end
        end
    end
endmodule
