#include "cpu/exec.h"

CPU_state cpu;

rtlreg_t s0, s1, t0, t1, ir;

/* shared by all helper functions */
DecodeInfo decinfo;

void decinfo_set_jmp(bool is_jmp, vaddr_t jmp_pc) {
  if(is_jmp) {
    decinfo.is_jmp = is_jmp;
    decinfo.jmp_pc = jmp_pc;
  }
}

void isa_exec(vaddr_t *pc);

vaddr_t exec_once(void) {
  decinfo.seq_pc = cpu.pc;
  isa_exec(&decinfo.seq_pc);
  update_pc();
  if (isa_query_intr()) update_pc();
  return decinfo.seq_pc;
}
