#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for(int i = 0; i < (DIFFTEST_REG_SIZE / sizeof(uint32_t)) - 1; i++) {
    if(ref_r->gpr[i]._32 != cpu.gpr[i]._32) {
      Log("index:%d ref:%x cpu:%x", i, ref_r->gpr[i]._32, cpu.gpr[i]._32);
      return false;
    }
  }
  if(ref_r->pc != pc) {
    Log("pc ref:%x cpu:%x", ref_r->pc, pc);
    return false;
  }
  return true;
}

void isa_difftest_attach(void) {
}
