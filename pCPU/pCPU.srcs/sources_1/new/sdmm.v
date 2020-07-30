`timescale 1ns / 1ps
// pComputer memory mapped SD card
// sdcard.v is connected to this
// 0x00000000 to 0x0ffffffc: mapped memory
// read/write 0x9600(0|1|2)xxx: pass to sdcard.v
// read/write 0x96003000: MM start sector
// read/write 0x96003004: MM size(in sector)

module sdmm
    (
        input clk,
        input rst,

        input [31:0]a,
        input [31:0]d,
        input we,
        input rd,
        output reg [31:0]spo,
        output wire ready,

        output reg [15:0]sddrv_a,
        output reg [31:0]sddrv_d,
        output reg sddrv_we,
        input [31:0]sddrv_spo,

        output reg irq = 0
    );

    wire control_addr = (a[31:16] == 16'h9600);
    reg [31:0]mm_start_sector = 0;
    reg [31:0]mm_size = 0;
    wire ncontrol_addr_legal = (a[31:28] == 4'b0) & (a < (mm_size << 9)); // this means not control_addr

    localparam IDLE = 4'h0;
    localparam READ = 4'h1;
    localparam WRITE = 4'h2;
    localparam FLUSH_CHECK = 4'h3;
    localparam DIRTY_CHECK = 4'h4;
    localparam SYNC = 4'h5;
    localparam SET_NEW_SECTOR = 4'h6;
    localparam LOAD_NEW_SECTOR = 4'h7;
    localparam WAIT_READY = 4'hf;
    reg [3:0]state = IDLE;
    reg [3:0]state_return = IDLE; // return to after SD becomes ready
    reg [3:0]state_todo = IDLE; // READ or WRITE
    reg [31:0]target_addr = 0;
    wire [15:0]cache_rw_addr = {4'hf, 3'h0, target_addr[8:0]};
    reg [31:0]target_data = 0;
    reg need_flush; // whether the target_addr falls into the current loaded SD sector
    reg is_dirty;
    reg is_ready;
    assign ready = (state == IDLE | state == READ | state == WRITE);

    always @ (posedge clk) begin
        if (rst) begin
            mm_start_sector <= 0;
            mm_size <= 0;
            state <= IDLE;
        end
        else begin
            if (control_addr & we & ready)
                case (a[15:0])
                    16'h3000: mm_start_sector <= d;
                    16'h3004: mm_size <= d;
                    default: ;
                endcase

            case (state)
                IDLE: begin
                    if (rd & ncontrol_addr_legal) begin
                        target_addr <= a;
                        target_data <= d;
                        state_todo <= READ;
                        state_return <= FLUSH_CHECK;
                        state <= WAIT_READY;
                    end
                    else if (we & ncontrol_addr_legal) begin
                        target_addr <= a;
                        target_data <= d;
                        state_todo <= WRITE;
                        state_return <= FLUSH_CHECK;
                        state <= WAIT_READY;
                    end
                end
                FLUSH_CHECK: begin
                    if (need_flush) state <= DIRTY_CHECK;
                    else state <= state_todo; // we have a "cache hit"
                end
                DIRTY_CHECK: begin
                    if (is_dirty) state <= SYNC;
                    else state <= LOAD_NEW_SECTOR;
                end
                SYNC: begin // write back dirty sector
                    state <= WAIT_READY;
                    state_return <= SET_NEW_SECTOR;
                end
                SET_NEW_SECTOR: begin
                    state <= LOAD_NEW_SECTOR;
                end
                LOAD_NEW_SECTOR: begin
                    state_return <= state_todo;
                    state <= WAIT_READY;
                end
                READ: begin
                    state <= IDLE;
                end
                WRITE: begin
                    state <= IDLE;
                end
                WAIT_READY: begin
                    if (is_ready) state <= state_return;
                    else state <= WAIT_READY;
                end
                default: begin
                    state <= IDLE;
                end
            endcase
        end
    end

    always @ (*) begin
        spo = 0;
        sddrv_a = 0;
        sddrv_d = 0;
        sddrv_we = 0;
        need_flush = 0;
        is_dirty = 0;
        is_ready = 0;
        case (state)
            IDLE: begin
                if (control_addr)
                    case (a[15:0])
                        16'h3000: spo = mm_start_sector;
                        16'h3004: spo = mm_size;
                        default: begin
                            sddrv_a = a[15:0];
                            sddrv_d = d;
                            sddrv_we = we;
                            spo = sddrv_spo;
                        end
                    endcase
            end
            FLUSH_CHECK: begin
                sddrv_a = 16'h1000;
                need_flush = (sddrv_spo == (target_addr>>9) + mm_start_sector);
            end
            DIRTY_CHECK: begin
                sddrv_a = 16'h2014;
                is_dirty = sddrv_spo[0];
            end
            SYNC: begin
                sddrv_a = 16'h1008;
                sddrv_d = 32'h1;
                sddrv_we = 1;
            end
            SET_NEW_SECTOR: begin
                sddrv_a = 16'h1000;
                sddrv_d = (target_addr>>9) + mm_start_sector;
                sddrv_we = 1;
            end
            LOAD_NEW_SECTOR: begin
                sddrv_a = 16'h1004;
                sddrv_d = 32'b1;
                sddrv_we = 1;
            end
            WAIT_READY: begin
                sddrv_a = 16'h2010;
                is_ready = sddrv_spo[0];
            end
            READ: begin
                sddrv_a = cache_rw_addr;
                spo = sddrv_spo;
            end
            WRITE: begin
                sddrv_a = cache_rw_addr;
                sddrv_d = target_data;
                sddrv_we = 1;
            end
            default: ;
        endcase
    end

    //always @ (*) begin
        //spo = 0;
        //if (control_addr) case (a)
            //16'h2000: spo = sd_ncd;
            //16'h2004: spo = sd_wp;
            //16'h2010: spo = sd_ready_real;
            //16'h2014: spo = dirty;
            //16'h3000: spo = mm_start_sector;
            //16'h3004: spo = mm_end_sector;
            //default: ;
        //endcase
    //end
endmodule
