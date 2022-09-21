#include "cpu/exec.h"

make_EHelper(jal) {
    rtl_addi(&s0, &id_src2->val, 4);
    rtl_sr(id_dest->reg, &s0, 4);
    rtl_add(&id_src2->val, &id_src2->val, &id_src->val);
    decinfo.seq_pc = id_src2->val;

    print_asm_template2(jal);
}

make_EHelper(jalr) {
    rtl_addi(&s0, &decinfo.seq_pc, 4);
    rtl_add(&s1, &id_src->val, &id_src2->val);
    rtl_andi(&s1, &s1, (~1U));
    decinfo.seq_pc = s1;
    rtl_sr(id_dest->reg, &s0, 4);
}