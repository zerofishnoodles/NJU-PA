#include "cpu/exec.h"
#include "all-instr.h"

static OpcodeEntry load_table [8] = {
  EXW(ild, 1), EXW(ild, 2), EXW(ld, 4), EMPTY, EXW(ld, 1), EXW(ld, 2), EMPTY, EMPTY
};

static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry store_table [8] = {
  EXW(st, 1), EXW(st, 2), EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry branch_table [8] = {
  EX(beq), EX(bne) , EMPTY, EMPTY, EX(blt), EX(bge), EX(bltu), EX(bgeu)
};

static make_EHelper(branch) {
  idex(pc, &branch_table[decinfo.isa.instr.funct3]);
}

static make_EHelper(add_sub_mul) {
  if(decinfo.isa.instr.funct7 == 0b0000000) { // add 
    OpcodeEntry e = EX(add);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0100000) { // sub
    OpcodeEntry e = EX(sub);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0000001) { // mul
    OpcodeEntry e = EX(mul);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }
}

static make_EHelper(xor_div) {
  if(decinfo.isa.instr.funct7 == 0b0000000) { // xor
    OpcodeEntry e = EX(xor);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0000001) { // div
    OpcodeEntry e = EX(div);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }
}

static make_EHelper(srl_sra_divu) {
  if(decinfo.isa.instr.funct7 == 0b0000000) { // srl
    OpcodeEntry e = EX(srl);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0100000) { // sra
    OpcodeEntry e = EX(sra);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0000001) { // divu
    OpcodeEntry e = EX(divu);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }
}

static make_EHelper(or_rem) {
  if(decinfo.isa.instr.funct7 == 0b0000000) { // or
    OpcodeEntry e = EX(or);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0000001) { // rem
    OpcodeEntry e = EX(rem);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }
}

static make_EHelper(sll_mulh) {
  if(decinfo.isa.instr.funct7 == 0b0000000) { // sll
    OpcodeEntry e = EX(sll);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0000001) { // mulh
    OpcodeEntry e = EX(mulh);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }
}

static make_EHelper(and_remu) {
  if(decinfo.isa.instr.funct7 == 0b0000000) { // and
    OpcodeEntry e = EX(and);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0000001) { // remu
    OpcodeEntry e = EX(remu);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }

}

static make_EHelper(slt_mulhsu) {
  if(decinfo.isa.instr.funct7 == 0b0000000) { // slt
    OpcodeEntry e = EX(slt);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0000001) { // mulhsu
    OpcodeEntry e = EX(mulhsu);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }

}

static make_EHelper(sltu_mulhu) {
  if(decinfo.isa.instr.funct7 == 0b0000000) { // sltu
    OpcodeEntry e = EX(sltu);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0000001) { // mulhu
    OpcodeEntry e = EX(mulhu);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }

}


static OpcodeEntry Rcompute_table [8] = {
  /* b0 */  EX(add_sub_mul), EX(sll_mulh), EX(slt_mulhsu), EX(sltu_mulhu), EX(xor_div), EX(srl_sra_divu), EX(or_rem), EX(and_remu),
};

static make_EHelper(Rcompute) {
  idex(pc, &Rcompute_table[decinfo.isa.instr.funct3]);
}

static make_EHelper(srli_srai) {
  if(decinfo.isa.instr.funct7 == 0b0000000) {// srli
    OpcodeEntry e = EX(srli);
    idex(pc, &e);
  }else if(decinfo.isa.instr.funct7 == 0b0100000) { // srai
    OpcodeEntry e = EX(srai);
    idex(pc, &e);
  }else {
    OpcodeEntry e = EMPTY;
    idex(pc, &e);
  }
}

static OpcodeEntry Icompute_table [8] = {
  /* b0 */  EX(addi), EX(slli), EX(slti), EX(sltiu), EX(xori), EX(srli_srai), EX(ori), EX(andi)
};

static make_EHelper(Icompute) {
  idex(pc, &Icompute_table[decinfo.isa.instr.funct3]);
}

static make_EHelper(ecall_sret) {
  if(decinfo.isa.instr.csr == 0) {
    OpcodeEntry e = EX(ecall);
    idex(pc, &e);
  }else{
    OpcodeEntry e = EX(sret);
    idex(pc, &e);
  }
}

static OpcodeEntry csr_table [8] = {
  EX(ecall_sret), EX(csrrw), EX(csrrs), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(csr) {
  idex(pc, &csr_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I, Icompute), IDEX(auipc,auipc), EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R,Rcompute), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(branch,branch), IDEX(jalr,jalr), EX(nemu_trap), IDEX(J, jal), IDEX(csr, csr), EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}
