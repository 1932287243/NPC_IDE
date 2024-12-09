import "DPI-C" function void npc_state(input int flag, input int inst_to_sdb);
module CU(
    input       [31:0]  inst,
    output              rf_wr_en,
    output              csrf_wr_en,
    output  reg [1:0]   csr_inst_sel,
    output  reg [1:0]   rf_wr_sel,
    output              do_jump,
    output  reg [2:0]   BrType,
    output              alu_a_sel,
    output              alu_b_sel,
    output  reg [3:0]   alu_ctrl,
    output  reg [2:0]   dm_rd_ctrl,
    output  reg [1:0]   dm_wr_ctrl
);

    wire    [6:0]   opcode;
    wire    [2:0]   funct3;
    wire    [6:0]   funct7;

    wire    is_lui;
    wire    is_auipc;
    wire    is_jal;
    wire    is_jalr;
    wire    is_beq;
    wire    is_bne;
    wire    is_blt;
    wire    is_bge;
    wire    is_bltu;
    wire    is_bgeu;
    wire    is_lb;
    wire    is_lh;
    wire    is_lw;
    wire    is_lbu;
    wire    is_lhu;
    wire    is_sb;
    wire    is_sh;
    wire    is_sw;
    wire    is_addi;
    wire    is_slti;
    wire    is_sltiu;
    wire    is_xori;
    wire    is_ori;
    wire    is_andi;
    wire    is_slli;
    wire    is_srli;
    wire    is_srai;
    wire    is_add;
    wire    is_sub;
    wire    is_sll;
    wire    is_slt;
    wire    is_sltu;
    wire    is_xor;
    wire    is_srl;
    wire    is_sra;
    wire    is_or;
    wire    is_and;

    wire    is_add_type;
    wire    is_u_type;
    wire    is_jump_type;
    wire    is_b_type;
    wire    is_r_type;
    wire    is_i_type;
    wire    is_s_type;

    wire    is_csrrw;
    wire    is_csrrs;
    wire    is_csrrc;  
    wire    is_csrwi; 
    wire    is_csrsi; 
    wire    is_csrci; 
    wire    is_csr;

    wire    is_ecall;
    wire    is_mret;
    assign  opcode  = inst[6:0];
    assign  funct7  = inst[31:25];
    assign  funct3  = inst[14:12];

    assign  is_lui  = (opcode == 7'h37) ;
    assign  is_auipc= (opcode == 7'h17) ;
    assign  is_jal  = (opcode == 7'h6F) ;
    assign  is_jalr = (opcode == 7'h67) && (funct3 ==3'h0) ;
    assign  is_beq  = (opcode == 7'h63) && (funct3 ==3'h0) ;
    assign  is_bne  = (opcode == 7'h63) && (funct3 ==3'h1) ;
    assign  is_blt  = (opcode == 7'h63) && (funct3 ==3'h4) ;
    assign  is_bge  = (opcode == 7'h63) && (funct3 ==3'h5) ;
    assign  is_bltu = (opcode == 7'h63) && (funct3 ==3'h6) ;
    assign  is_bgeu = (opcode == 7'h63) && (funct3 ==3'h7) ;
    assign  is_lb   = (opcode == 7'h03) && (funct3 ==3'h0) ;
    assign  is_lh   = (opcode == 7'h03) && (funct3 ==3'h1) ;
    assign  is_lw   = (opcode == 7'h03) && (funct3 ==3'h2) ;
    assign  is_lbu  = (opcode == 7'h03) && (funct3 ==3'h4) ;
    assign  is_lhu  = (opcode == 7'h03) && (funct3 ==3'h5) ;
    assign  is_sb   = (opcode == 7'h23) && (funct3 ==3'h0) ;
    assign  is_sh   = (opcode == 7'h23) && (funct3 ==3'h1) ;
    assign  is_sw   = (opcode == 7'h23) && (funct3 ==3'h2) ;
    assign  is_addi = (opcode == 7'h13) && (funct3 ==3'h0) ;
    assign  is_slti = (opcode == 7'h13) && (funct3 ==3'h2) ;
    assign  is_sltiu= (opcode == 7'h13) && (funct3 ==3'h3) ;
    assign  is_xori = (opcode == 7'h13) && (funct3 ==3'h4) ;
    assign  is_ori  = (opcode == 7'h13) && (funct3 ==3'h6) ;
    assign  is_andi = (opcode == 7'h13) && (funct3 ==3'h7) ;
    assign  is_slli = (opcode == 7'h13) && (funct3 ==3'h1) && (funct7 == 7'h00);
    assign  is_srli = (opcode == 7'h13) && (funct3 ==3'h5) && (funct7 == 7'h00);
    assign  is_srai = (opcode == 7'h13) && (funct3 ==3'h5) && (funct7 == 7'h20);
    assign  is_add  = (opcode == 7'h33) && (funct3 ==3'h0) && (funct7 == 7'h00);
    assign  is_sub  = (opcode == 7'h33) && (funct3 ==3'h0) && (funct7 == 7'h20);
    assign  is_sll  = (opcode == 7'h33) && (funct3 ==3'h1) && (funct7 == 7'h00);
    assign  is_slt  = (opcode == 7'h33) && (funct3 ==3'h2) && (funct7 == 7'h00);
    assign  is_sltu = (opcode == 7'h33) && (funct3 ==3'h3) && (funct7 == 7'h00);
    assign  is_xor  = (opcode == 7'h33) && (funct3 ==3'h4) && (funct7 == 7'h00);
    assign  is_srl  = (opcode == 7'h33) && (funct3 ==3'h5) && (funct7 == 7'h00);
    assign  is_sra  = (opcode == 7'h33) && (funct3 ==3'h5) && (funct7 == 7'h20);
    assign  is_or   = (opcode == 7'h33) && (funct3 ==3'h6) && (funct7 == 7'h00);
    assign  is_and  = (opcode == 7'h33) && (funct3 ==3'h7) && (funct7 == 7'h00);

    assign  is_csrrw  = (opcode == 7'h73) && (funct3 ==3'h1);
    assign  is_csrrs  = (opcode == 7'h73) && (funct3 ==3'h2);
    assign  is_csrrc  = (opcode == 7'h73) && (funct3 ==3'h3);  
    assign  is_csrwi  = (opcode == 7'h73) && (funct3 ==3'h5); 
    assign  is_csrsi  = (opcode == 7'h73) && (funct3 ==3'h6); 
    assign  is_csrci  = (opcode == 7'h73) && (funct3 ==3'h7); 

    assign  is_ecall = (inst == 32'b00000000000000000000000001110011);
    assign  is_mret  = (inst == 32'b00110000001000000000000001110011);

    assign  is_csr = is_csrrw | is_csrrs | is_csrrc | is_csrwi | is_csrsi | is_csrci;

    assign  is_add_type = is_auipc | is_jal | is_jalr | is_b_type | is_s_type 
                        | is_lb | is_lh | is_lw | is_lbu | is_lhu | is_add | is_addi ;
    assign  is_u_type   = is_lui | is_auipc ;
    assign  is_jump_type= is_jal ;
    assign  is_b_type   = is_beq | is_bne | is_blt | is_bge | is_bltu | is_bgeu ;
    assign  is_r_type   = is_add | is_sub | is_sll | is_slt | is_sltu | is_xor 
                        | is_srl | is_sra | is_or | is_and ;
    assign  is_i_type   = is_jalr | is_lb | is_lh | is_lw | is_lbu | is_lhu 
                        | is_addi | is_slti | is_sltiu | is_xori | is_ori | is_andi
                        | is_slli | is_srli | is_srai ;
    assign  is_s_type   = is_sb | is_sh | is_sw ;
    always @(*)begin
        if(is_u_type|is_jump_type|is_i_type|is_r_type|is_s_type|is_b_type|is_csr|is_ecall|is_mret)
            npc_state(0, inst);
        else
            npc_state(1, inst);
    end
    assign csrf_wr_en = is_csr | is_mret | is_ecall;

    always@(*)
    begin
        if(is_csrrw)
            csr_inst_sel = 2'b01;
        else if(is_csrrs)
            csr_inst_sel = 2'b10;
        else if(is_ecall)
            csr_inst_sel = 2'b11;
        else   
            csr_inst_sel = 2'b00;
    end
    
    //rf_wr_en  
    assign rf_wr_en     =   is_u_type | is_jump_type | is_i_type| is_r_type|is_csr|is_ecall|is_mret;  
    //[1:0]rf_wr_sel
    always@(*)
    begin
        if(is_jal | is_jalr)
            rf_wr_sel = 2'b01;
        else if(is_r_type | is_u_type | is_addi | is_slti | is_sltiu | is_xori | is_ori | is_andi
                    | is_slli | is_srli | is_srai | is_csr)
            rf_wr_sel = 2'b10;
        else if(is_lb | is_lh | is_lw | is_lbu | is_lhu )
            rf_wr_sel = 2'b11;
        else 
            rf_wr_sel = 2'b00;
    end  
  
    //do_jump
    assign do_jump      =  (is_jal|is_jalr|is_ecall|is_mret)?1:0;
  
    //[2:0]BrType
    always@(*)
    begin
        if(is_beq)
            BrType = 3'b010;
        else if(is_bne)
            BrType = 3'b011;
        else if(is_blt)
            BrType = 3'b100;
        else if(is_bge)
            BrType = 3'b101;
        else if(is_bltu)
            BrType = 3'b110;
        else if(is_bgeu)
            BrType = 3'b111;
        else 
            BrType = 3'b000;
    end
  
    //alu_a_sel
    assign alu_a_sel    = (is_r_type|is_i_type|is_s_type|is_csrrs)?1:0;

    //alu_b_sel  
    assign alu_b_sel    =  (is_r_type|is_csrrs|is_ecall|is_mret)?0:1;
  
    //alu_ctrl
    always@(*)
    begin
        if(is_add_type)
            alu_ctrl = 4'b0000;
        else if(is_sub)
            alu_ctrl = 4'b1000;
        else if(is_sll|is_slli)
            alu_ctrl = 4'b0001;
        else if(is_srl|is_srli)
            alu_ctrl = 4'b0101;
        else if(is_sra|is_srai)
            alu_ctrl = 4'b1101;
        else if(is_slt|is_slti)
            alu_ctrl = 4'b0010;
        else if(is_sltu|is_sltiu)
            alu_ctrl = 4'b0011;
        else if(is_xori|is_xor)
            alu_ctrl = 4'b0100;
        else if(is_ori|is_or|is_csrrs)
            alu_ctrl = 4'b0110;
        else if(is_andi|is_and)
            alu_ctrl = 4'b0111;
        else if(is_lui)
            alu_ctrl = 4'b1110;
        else if(is_ecall | is_mret)
            alu_ctrl = 4'b1001;
        else if(is_csrrw)
            alu_ctrl = 4'b1010;
        else
            alu_ctrl = 4'b1111;
    end
  
    //[2:0]dm_rd_ctrl
    always@(*)
    begin
        if(is_lb)
            dm_rd_ctrl = 3'b001;
        else if(is_lbu)
            dm_rd_ctrl = 3'b010;
        else if(is_lh)
            dm_rd_ctrl = 3'b011;
        else if(is_lhu)
            dm_rd_ctrl = 3'b100;
        else if(is_lw)
            dm_rd_ctrl = 3'b101;
        else
            dm_rd_ctrl = 3'b000;
    end

    //[1:0]dm_wr_ctrl
    always@(*)
    begin
        if(is_sb)
            dm_wr_ctrl = 2'b01;
        else if(is_sh)
            dm_wr_ctrl = 2'b10;
        else if(is_sw)
            dm_wr_ctrl = 2'b11;
        else
            dm_wr_ctrl = 2'b00;
    end  
endmodule