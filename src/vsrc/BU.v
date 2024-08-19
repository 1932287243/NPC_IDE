module BU(         
    input       [31:0]  rs1,
    input       [31:0] 	rs2,
    input       [2:0]   Type,
    output  reg         BrE
);
    wire signed 	[31:0] signed_rs1;
    wire signed 	[31:0] signed_rs2;
    wire unsigned 	[31:0] unsigned_rs1;
    wire unsigned 	[31:0] unsigned_rs2;

    assign signed_rs1 = rs1;
    assign signed_rs2 = rs2; 
    assign unsigned_rs1 = rs1;
    assign unsigned_rs2 = rs2; 
    always@(*)begin
        case(Type)
            3'b010: BrE = signed_rs1 == signed_rs2;
            3'b011: BrE = signed_rs1 != signed_rs2;
            3'b100: BrE = signed_rs1 < signed_rs2;
            3'b101: BrE = signed_rs1 >= signed_rs2;
            3'b110: BrE = unsigned_rs1 < unsigned_rs2;
            3'b111: BrE = unsigned_rs1 >= unsigned_rs2;
            default:BrE = 1'b0;
        endcase
    end
endmodule