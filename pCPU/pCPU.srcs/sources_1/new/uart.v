`timescale 1ns / 1ps
// pComputer FIFO UART output
// input XXMHz, 16x oversampling
// warning: not very reliable: read/write together case, ...
// so need special software care(write one value and wait for queue empty)

module uart
    (
        input clk_50M,
        input rxclk_en,
        input txclk_en,
        input rst,

        input [1:0]a,
        input [31:0]d,
        input we,
        output reg [31:0]spo,
        
        input rx,
        output reg tx = 1
    );

    wire rxclk_en;
    wire txclk_en;

    reg [7:0]fifo_in = 0;
    reg fifo_dequeue = 0;
    reg fifo_enqueue = 0;
    wire fifo_empty;
    wire fifo_full;
    wire [7:0]fifo_out;
    fifo fifo_inst
    (
        .clk(clk_50M),
        .rst(rst),
        .din(fifo_in),
        .enqueue(fifo_enqueue),
        .dequeue(fifo_dequeue),

        .dout(fifo_out),
        .full(fifo_full),
        .empty(fifo_empty)
    );

    reg [7:0]data = 8'h00;
    reg [2:0]bitpos = 0;

    localparam IDLE = 3'b000;
    localparam PREPARE1 = 3'b001;
    localparam PREPARE2 = 3'b010;
    localparam START = 3'b011;
    localparam DATA = 3'b100;
    localparam STOP = 3'b101;
    reg [2:0]state = IDLE;

    reg tx_busy;

    always @ (*) begin
        if (a == 2'b01) spo = {31'b0, fifo_full};
        else if (a == 2'b10) spo = {31'b0, fifo_empty};
        else if (a == 2'b11) spo = {31'b0, tx_busy};
        else spo = 32'b0;
    end

    always @ (posedge clk_50M) begin
        if (rst) begin
            tx_busy <= 0;
            tx <= 1'b1;
            state <= IDLE;
            fifo_enqueue <= 0;
            fifo_dequeue <= 0;
        end
        else begin
            if (we & !fifo_full) begin
                if (a == 2'b00) begin
                    fifo_enqueue <= 1;
                    fifo_in <= d[7:0];
                end
                else begin
                    fifo_enqueue <= 0;
                    fifo_in <= 8'b0;
                end
            end
            else begin
                fifo_enqueue <= 0;
                fifo_in <= 8'b0;

                //// debug
                //case (state) 
                    //IDLE: begin // this means not empty
                        //state <= START;
                    //end
                    //START: if (txclk_en) begin
                        //bitpos <= 0;
                        //data <= 65;
                        //tx <= 1'b0;
                        //state <= DATA;
                    //end
                    //DATA: if (txclk_en) begin
                        //if (bitpos == 7) state <= STOP;
                        //else bitpos <= bitpos + 1;
                        //tx <= data[bitpos];
                    //end
                    //STOP: if (txclk_en) begin
                        //tx <= 1'b1;
                        //state <= IDLE;
                    //end
                    //default: ;
                //endcase
                case (state) 
                    IDLE: if (fifo_empty == 0) begin // this means not empty
                        tx_busy <= 1;
                        fifo_dequeue <= 1;
                        state <= PREPARE1;
                    end
                    // bug here
                    PREPARE1: begin
                        fifo_dequeue <= 0;
                        state <= PREPARE2;
                    end
                    PREPARE2: begin
                        data <= fifo_out;
                        state <= START;
                    end
                    START: if (txclk_en) begin
                        bitpos <= 0;
                        tx <= 1'b0;
                        state <= DATA;
                    end
                    DATA: if (txclk_en) begin
                        if (bitpos == 7) state <= STOP;
                        else bitpos <= bitpos + 1;
                        tx <= data[bitpos];
                    end
                    STOP: if (txclk_en) begin
                        tx_busy <= 0;
                        tx <= 1'b1;
                        state <= IDLE;
                    end
                    default: ;
                endcase
            end
        end
    end
endmodule
