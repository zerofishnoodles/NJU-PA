#include "cpu/exec.h"

make_EHelper(jal) {
    rtl_addi(&s0, &cpu.pc, 4); 
    rtl_sr(id_dest->reg, &s0, 4);
    rtl_add(&s1, &cpu.pc, &id_src->val);
    rtl_j(s1);
    print_asm_template2(jal);
}

make_EHelper(jalr) {
#if defined(DIFF_TEST)
    difftest_skip_dut(1,2);
#endif
    rtl_addi(&s0, &cpu.pc, 4);
    rtl_add(&s1, &id_src->val, &id_src2->val);
    rtl_andi(&s1, &s1, (~1U));
    rtl_j(s1);
    rtl_sr(id_dest->reg, &s0, 4);
    print_asm_template2(jalr);
}

make_EHelper(bne) {
    rtl_setrelop(RELOP_NE, &s0, &id_src->val, &id_src2->val);
    if(s0) {
        rtl_add(&s0, &cpu.pc, &id_dest->val);
        rtl_j(s0);
    }
    print_asm_template3(bne);
}

make_EHelper(beq) {
    rtl_setrelop(RELOP_EQ, &s0, &id_src->val, &id_src2->val);
    if(s0) {
        rtl_add(&s0, &cpu.pc, &id_dest->val);
        rtl_j(s0);
    }
    print_asm_template3(beq);
}

make_EHelper(bge) {
    rtl_setrelop(RELOP_GE, &s0, &id_src->val, &id_src2->val);
    if(s0) {
        rtl_add(&s0, &cpu.pc, &id_dest->val);
        rtl_j(s0);
    }
    print_asm_template3(bge);
}

make_EHelper(bgeu) {
    rtl_setrelop(RELOP_GEU, &s0, &id_src->val, &id_src2->val);
    if(s0) {
        rtl_add(&s0, &cpu.pc, &id_dest->val);
        rtl_j(s0);
    }
    print_asm_template3(bge);
}

make_EHelper(blt) {
    rtl_setrelop(RELOP_LT, &s0, &id_src->val, &id_src2->val);
    if(s0) {
        rtl_add(&s0, &cpu.pc, &id_dest->val);
        rtl_j(s0);
    }
    print_asm_template3(blt);
}

make_EHelper(bltu) {
    rtl_setrelop(RELOP_LTU, &s0, &id_src->val, &id_src2->val);
    if(s0) {
        rtl_add(&s0, &cpu.pc, &id_dest->val);
        rtl_j(s0);
    }
    print_asm_template3(bltu);
}