module REF #(ADDR_WIDTH = 1, DATA_WIDTH = 1) (
    input                         clk,
    input     [DATA_WIDTH-1:0]    wdata,
    input     [ADDR_WIDTH-1:0]    rd,
    input                         wen,
    input     [ADDR_WIDTH-1:0]    rs1,rs2,
    output    [DATA_WIDTH-1:0]    rd1, rd2
);

    reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];
    always @(posedge clk) begin
        if (wen && (rd!=5'd0)) rf[rd] <= wdata;
    end
    
    assign rd1 = rf[rs1];
    assign rd2 = rf[rs2];
endmodule
