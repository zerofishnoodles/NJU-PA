#include "cpu/exec.h"

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(addi) {
  rtl_addi(&s0, &id_src->val, id_src2->simm);
  rtl_sr(id_dest->reg, &s0, decinfo.width);

  print_asm_template3(addi);
}

make_EHelper(auipc) {
  rtl_addi(&s0, &id_src->val, id_src2->imm);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template2(auipc);
}
