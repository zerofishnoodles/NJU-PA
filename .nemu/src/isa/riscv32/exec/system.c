#include "cpu/exec.h"

extern void raise_intr(uint32_t NO, vaddr_t epc);
extern void difftest_skip_ref();

make_EHelper(ecall) {
#if defined(DIFF_TEST)
    difftest_skip_dut(1,2);
#endif
  raise_intr(reg_l(17), cpu.pc);
  print_asm_template1(ecall);
}

make_EHelper(sret) {
  rtl_j(cpu.sepc+4);
  print_asm_template1(sret);
}

make_EHelper(csrrs) {
#if defined(DIFF_TEST)
    difftest_skip_ref();
#endif
  rtl_mv(&s0, &id_src2->val);
  rtl_or(&s1, &s0, &id_src->val);
  rtl_scsr(id_src2->reg, &s1, 4);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(csrrs);
}

make_EHelper(csrrw) {
#if defined(DIFF_TEST)
    difftest_skip_ref();
#endif
  rtl_mv(&s0, &id_src2->val);
  rtl_scsr(id_src2->reg, &id_src->val, 4);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template3(csrrw);
}