#include "cpu/exec.h"

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(auipc) {
  rtl_addi(&s0, &id_src->val, id_src2->imm);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template2(auipc);
}

make_EHelper(add) {
  rtl_add(&s0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(add);
}

make_EHelper(addi) {
  rtl_addi(&s0, &id_src->val, id_src2->simm);
  rtl_sr(id_dest->reg, &s0, decinfo.width);
  // printf("%d + %d = %d\n", id_src->val, id_src2->val, s0);
  print_asm_template3(addi);
}


make_EHelper(sub) {
  rtl_sub(&s0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &s0, 4);
  // printf("%d - %d = %d\n", id_src->val, id_src2->val, s0);

  print_asm_template3(sub);
}

make_EHelper(sll) {
  rtl_li(&s1, 31);
  rtl_and(&s0, &id_src2->val, &s1);
  rtl_shl(&s0, &id_src->val, &s0);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(sll);
}

make_EHelper(slli) {
  rtl_li(&s1, 31);
  rtl_and(&s0, &id_src2->val, &s1);
  rtl_shl(&s0, &id_src->val, &s0);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(slli);
}

make_EHelper(slt) {
  rtl_setrelop(RELOP_LT, &s0, &id_src->val, &id_src2->val);
  if(s0) {
    rtl_li(&s0, 1);
  }else{
    rtl_li(&s0, 0);
  }
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(slt);
}

make_EHelper(slti) {
  rtl_setrelop(RELOP_LT, &s0, &id_src->val, &id_src2->val);
  if(s0) {
    rtl_li(&s0, 1);
  }else{
    rtl_li(&s0, 0);
  }
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(slti);
}

make_EHelper(sltu) {
  rtl_setrelop(RELOP_LTU, &s0, &id_src->val, &id_src2->val);
  if(s0) {
    rtl_li(&s0, 1);
  }else{
    rtl_li(&s0, 0);
  }
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(sltu);
}

make_EHelper(sltiu) {
  rtl_setrelop(RELOP_LTU, &s0, &id_src->val, &id_src2->val);
  if(s0) {
    rtl_li(&s0, 1);
  }else{
    rtl_li(&s0, 0);
  }
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(sltiu);
}

make_EHelper(xor) {
  rtl_xor(&s0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(xor);
}

make_EHelper(xori) {
  rtl_xor(&s0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(xori);
}

make_EHelper(srl) {
  rtl_li(&s1, 31);
  rtl_and(&s0, &id_src2->val, &s1);
  rtl_shr(&s0, &id_src->val, &s0);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(srl);
}

make_EHelper(srli) {
  rtl_li(&s1, 31);
  rtl_and(&s0, &id_src2->val, &s1);
  rtl_shr(&s0, &id_src->val, &s0);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(srli);
}

make_EHelper(sra) {
  rtl_li(&s1, 31);
  rtl_and(&s0, &id_src2->val, &s1);
  rtl_sar(&s0, &id_src->val, &s0);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(sra);
}

make_EHelper(srai) {
  rtl_li(&s1, 31);
  rtl_and(&s0, &id_src2->val, &s1);
  rtl_sar(&s0, &id_src->val, &s0);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(srai);
}

make_EHelper(or) {
  rtl_or(&s0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(or);
}

make_EHelper(ori) {
  rtl_or(&s0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(ori);
}

make_EHelper(and) {
  rtl_and(&s0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(and);
}

make_EHelper(andi) {
  rtl_and(&s0, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(andi);
}







