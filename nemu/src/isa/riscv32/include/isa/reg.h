#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include "common.h"

#define PC_START (0x80000000u + IMAGE_START)

#define SEPC 0x141
#define SSTATUS 0x100
#define SCAUSE 0x142
#define STVEC 0x105
#define SRET 0x102

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  vaddr_t pc;
  rtlreg_t sepc;
  rtlreg_t sstatus;
  rtlreg_t scause;
  rtlreg_t stvec;

} CPU_state;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 32);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  assert(index >= 0 && index < 32);
  return regsl[index];
}

static inline const char* csr_name(uint32_t index, int width) {
  extern char csr_name_[10];
  switch (index)
  {
    case SEPC:
    strcpy(csr_name_, "sepc");
    return csr_name_;
    break;
  case SCAUSE:
    strcpy(csr_name_, "scause");
    return csr_name_;
    break;
  case SSTATUS:
    strcpy(csr_name_, "sstatus");
    return csr_name_;
    break;
  case STVEC:
    strcpy(csr_name_, "stvec");
    return csr_name_;
    break;
  case 0:
  case SRET:
    return csr_name_;
  default:
    panic("unknown csr!");
  }
}

#endif
