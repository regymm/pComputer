// this is copied from github
/*
 * Hacky baud rate generator to divide a 50MHz clock into a 115200 baud
 * rx/tx pair where the rx clcken oversamples by 16x.
 */
module baud_rate_gen
	#(
		parameter BAUD_RATE = 115200
	)
    (
        input wire clk,
        input rst,
        output wire rxclk_en,
        output wire txclk_en
    );

	`ifndef CLOCK_FREQ
		parameter CLOCK_FREQ = 62500000;
	`endif
    parameter RX_ACC_MAX = CLOCK_FREQ / (BAUD_RATE * 16);
    parameter TX_ACC_MAX = CLOCK_FREQ / BAUD_RATE;
    parameter RX_ACC_WIDTH = 20;
    parameter TX_ACC_WIDTH = 20;
    //parameter RX_ACC_WIDTH = $clog2(RX_ACC_MAX);
    //parameter TX_ACC_WIDTH = $clog2(TX_ACC_MAX);
    reg [RX_ACC_WIDTH - 1:0] rx_acc = 0;
    reg [TX_ACC_WIDTH - 1:0] tx_acc = 0;

    assign rxclk_en = (rx_acc == 0);
    assign txclk_en = (tx_acc == 0);

    always @(posedge clk) begin
        if (rst) rx_acc <= 1;
        else if (rx_acc == RX_ACC_MAX[RX_ACC_WIDTH - 1:0])
            rx_acc <= 0;
        else
            rx_acc <= rx_acc + 1;
    end

    always @(posedge clk) begin
        if (rst) tx_acc <= 1;
        else if (tx_acc == TX_ACC_MAX[TX_ACC_WIDTH - 1:0])
            tx_acc <= 0;
        else
            tx_acc <= tx_acc + 1;
    end

endmodule