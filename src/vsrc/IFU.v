import "DPI-C" function void trap(input int flag);
module IFU(
	input   [31:0]  inst_in,
	output  [31:0]  inst_out);

	assign inst_out = inst_in;
    always@(*)begin
        if(inst_out == 32'b000000000001_00000_000_00000_1110011)
            trap(1);
        else
            trap(0);
    end
endmodule    //fetch