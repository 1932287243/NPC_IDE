module ALU(
    input       [31:0]  rs1,rs2,
    input       [3:0]   func,
    output reg  [31:0]  ALUout
);

    wire signed [31:0] signed_a;
    wire signed [31:0] signed_b;
    wire signed [31:0] signed_1;
    wire signed [31:0] signed_0;
    assign signed_a = rs1;
    assign signed_b = rs2;
    assign signed_1 = 32'h1;
    assign signed_0 = 32'h0;

    always@(*)begin
        case(func)
            4'b0000:  ALUout = rs1 + rs2;
            4'b1000:  ALUout = rs1 - rs2;
            4'b0001:  ALUout = rs1 << rs2[4:0];
            4'b0010:  ALUout = signed_a < signed_b ? 1 : 0;
            4'b0011:  ALUout = rs1 < rs2 ? 1 : 0;
            4'b0100:  ALUout = rs1 ^ rs2;
            4'b0101:  ALUout = rs1 >> rs2[4:0];
            4'b1101:  ALUout = signed_a >>> rs2[4:0];
            4'b0110:  ALUout = rs1 | rs2;
            4'b0111:  ALUout = rs1 & rs2;
            4'b1001:  ALUout = rs2;
            4'b1010:  ALUout = rs1;
            4'b1011:  ALUout = 32'h0;
            4'b1100:  ALUout = 32'h0;
            4'b1110:  ALUout = rs2;
            4'b1111:  ALUout = 32'h0;
        endcase
    end 
endmodule