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

void isa_difftest_attach(long img_size) {
  // ref_difftest_memcpy_from_dut(PC_START, guest_to_host(IMAGE_START), img_size);
  // char *mainargs = guest_to_host(0);
  // ref_difftest_memcpy_from_dut(PC_START - IMAGE_START, mainargs, strlen(mainargs) + 1);
  // ref_difftest_setregs(&cpu);
}
