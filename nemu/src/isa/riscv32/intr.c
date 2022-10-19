#include "rtl/rtl.h"

#define IRQ_TIMER 0x80000005  // for riscv32

void raise_intr(uint32_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  s0 = (cpu.sstatus & SIE) << 4;
  cpu.sstatus = cpu.sstatus | s0;
  cpu.sstatus = cpu.sstatus & ~SIE;

  rtl_mv(&cpu.sepc, &epc);
  rtl_mv(&cpu.scause, &NO);
  rtl_j(cpu.stvec);
}

bool isa_query_intr(void) {
  if (cpu.INTR && (cpu.sstatus & SIE)) {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER, cpu.pc-4);  // for the +4 in sret beacuse now the pc is already the next instr
    return true;
  }
  return false;
}
