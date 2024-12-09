module imm(
	input 	    [31:0] inst,
	output reg	[31:0] out
);
	wire	[6:0] opcode;
	assign	opcode= inst[6:0];
	//立即数扩展
	always@(*)
	begin
		case(opcode)
			7'b0010111: out = {inst[31:12],{12{1'b0}}};
			7'b0110111: out = {inst[31:12],{12{1'b0}}};
			7'b1100011: out = {{19{inst[31]}},inst[31],inst[7],inst[30:25],inst[11:8],1'b0};
			7'b1101111: out = {{11{inst[31]}},inst[31], inst[19:12], inst[20], inst[30:21], {1'b0}};
			7'b1100111: out = {{20{inst[31]}}, inst[31:20]};
			7'b0000011: out = {{20{inst[31]}}, inst[31:20]};
			7'b0100011: out = {{20{inst[31]}}, inst[31:25], inst[11:7]};
			7'b0010011: out = {{20{inst[31]}}, inst[31:20]};
			7'b1110011: out = {{20{1'b0}}, inst[31:20]};
			default: out = 32'h0;
		endcase
	end
endmodule

module IDU(
	input       [31:0]  inst,
	input			    is_csr,	
	output  reg [4:0]   rd,
	output  reg [4:0]   rs1,
	output  reg [4:0]   rs2,
	output      [31:0]  imm);

	always@(*)begin
		rd = inst[11:7];
		rs1 = inst[19:15];
		if(is_csr)
			rs2 = 5'h0;
		else
			rs2 = inst[24:20];
	end
	
	imm imm1(
		.inst(inst),
		.out(imm));

endmodule    //decode