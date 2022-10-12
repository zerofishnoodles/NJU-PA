#include "nemu.h"

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

char csr_name_[10];

void isa_reg_display() {
  for(int i=0; i<32; i++) {
    printf("%s\t0x%x\t%d\n", regsl[i], reg_l(i), reg_l(i));
  }
  printf("%s\t0x%x\t%d\n", "pc", cpu.pc, cpu.pc);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  for(int i=0; i<32; i++) {
    if(strcmp(regsl[i], s) == 0){
      *success = true;
      return reg_l(i);
    }
  }
  if(strcmp("pc", s) == 0) {
    *success = true;
    return cpu.pc;
  }
  *success = false;
  return 0;
}
