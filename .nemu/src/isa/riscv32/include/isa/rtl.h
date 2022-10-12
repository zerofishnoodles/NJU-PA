#ifndef __RISCV32_RTL_H__
#define __RISCV32_RTL_H__

#include "rtl/rtl.h"

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  if (r != 0) { rtl_mv(dest, &reg_l(r)); }
  else { rtl_li(dest, 0); }
}

static inline void rtl_sr(int r, const rtlreg_t *src1, int width) {
  if (r != 0) { rtl_mv(&reg_l(r), src1); }
}

static inline void rtl_lcsr(rtlreg_t* dest, uint32_t r, int width) {
  switch (r)
  {
  case SEPC:
    rtl_mv(dest, &cpu.sepc);
    break;
  case SCAUSE:
    rtl_mv(dest, &cpu.scause);
    break;
  case SSTATUS:
    rtl_mv(dest, &cpu.sstatus);
    break;
  case STVEC:
    rtl_mv(dest, &cpu.stvec);
    break;
  case 0:  // ecall
  case SRET: // sret
    break;
  default:
    panic("unknown csr");
  }
}

static inline void rtl_scsr(uint32_t r, rtlreg_t* src, int width) {
  switch (r)
  {
  case SEPC:
    rtl_mv(&cpu.sepc, src);
    break;
  case SCAUSE:
    rtl_mv(&cpu.scause, src);
    break;
  case SSTATUS:
    rtl_mv(&cpu.sstatus, src);
    break;
  case STVEC:
    rtl_mv(&cpu.stvec, src);
    break;
  case 0:  //ecall
  case SRET: // sret
    break;
  default:
    panic("unknown csr");
  }
}



#endif
