module CPU(
    input           clk,
    input           rst,
    input   [31:0]  inst,
    output  [31:0]  pc
);

wire    [1:0]   rf_wr_sel;
reg     [31:0]  rf_wd;  
wire            rf_wr_en;
wire            csrf_wr_en;
wire    [1:0]   csr_inst_sel;
reg    [31:0]  csr_in_val;
wire    [4:0]   rs1,rs2,rd;
wire    [31:0]  rf_rd1,rf_rd2,csrv;
wire    [31:0]  rf_rd2_temp;
wire [31:0] pc_plus4;
wire do_jump;
wire JUMP;
  
wire    [31:0]  imm_out;
  
wire    [2:0]   comp_ctrl;
wire		BrE;

wire            alu_a_sel;
wire            alu_b_sel;
wire    [31:0]  alu_a,alu_b,alu_out; 
wire    [3:0]   alu_ctrl;

wire    [2:0]   dm_rd_ctrl;
wire    [1:0]   dm_wr_ctrl;
wire    [31:0]  dm_dout;

always@(*)
begin
    case(csr_inst_sel)
        2'b00:  csr_in_val = 32'h0;
        2'b01:  csr_in_val = rf_rd1;
        2'b10:  csr_in_val = alu_out;
        2'b11:  csr_in_val = pc;
        default:csr_in_val = 32'h0;
    endcase
end

always@(*)
begin
    case(rf_wr_sel)
    2'b00:  rf_wd = 32'h0;
    2'b01:  rf_wd = pc_plus4;
    2'b10:  begin if(csr_inst_sel==2'b10) rf_wd = csrv; else rf_wd = alu_out; end
    2'b11:  rf_wd = dm_dout;
    default:rf_wd = 32'h0;
    endcase
end

assign		pc_plus4 = pc + 32'h4;
assign		JUMP = BrE || do_jump;
assign      alu_a = alu_a_sel ? rf_rd1 : pc ;
assign      alu_b = alu_b_sel ? imm_out : rf_rd2_temp ;  //gai
assign      rf_rd2_temp = rf_rd2 | csrv; 


IFU inst_fetch_unit(
    .inst_in    (inst),
    .inst_out   ()
);

IDU idu(
    .inst   (inst),
    .is_csr (csrf_wr_en),
    .rd     (rd),
    .rs1    (rs1),
    .rs2    (rs2),
    .imm    (imm_out)
);

REF  #(5,32) reg_file(
    .clk    (clk),
    .wdata  (rf_wd),
    .rd     (rd),
    .wen    (rf_wr_en),
    .rs1    (rs1),
    .rs2    (rs2),
    .rd1    (rf_rd1),
    .rd2    (rf_rd2)
);

CSR  #(5,32) csr_file(
    .clk    (clk),
    .inst   (inst),
    .wdata  (csr_in_val),
    .wen    (rf_wr_en),
    .imm    (imm_out),
    .csrv   (csrv)
);

ALU alu(
    .rs1    (alu_a),
    .rs2    (alu_b),
    .func   (alu_ctrl),
    .ALUout (alu_out)
);

BU bu(
    .rs1    (rf_rd1),
    .rs2    (rf_rd2),
    .Type   (comp_ctrl),
    .BrE    (BrE)
);

PC pc0(
    .clk        (clk),
    .rst        (rst),
    .JUMP       (JUMP),
    .JUMP_PC    (alu_out),
    .pc         (pc)
);

CU cu(
	.inst           (inst),
	.rf_wr_en       (rf_wr_en),
    .csrf_wr_en     (csrf_wr_en),
    .csr_inst_sel   (csr_inst_sel),
	.rf_wr_sel      (rf_wr_sel),
	.do_jump        (do_jump),
	.BrType		    (comp_ctrl),
	.alu_a_sel      (alu_a_sel),
	.alu_b_sel      (alu_b_sel),
	.alu_ctrl       (alu_ctrl),
	.dm_rd_ctrl     (dm_rd_ctrl),
	.dm_wr_ctrl     (dm_wr_ctrl)
);

MEM mem0(
	.clk        (clk),
	.dm_rd_ctrl (dm_rd_ctrl),
	.dm_wr_ctrl (dm_wr_ctrl),
	.dm_addr    (alu_out),
	.dm_din     (rf_rd2),
	.dm_dout    (dm_dout)
);

endmodule