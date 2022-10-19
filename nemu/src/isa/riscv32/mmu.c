#include "nemu.h"


static inline void check_vaddr_bound(vaddr_t vaddr) {
  if(OFF(vaddr) >= PAGE_SIZE) { // will not happen in riscv32
    panic("check vaddr bound failed");
  }
}

static inline paddr_t page_walk(vaddr_t addr, int len) {
  if(cpu.satp == 0) return addr;
  paddr_t pde_addr = (cpu.satp << 12) | (PDX(addr) << 2); 
  PDE pde = paddr_read(pde_addr, len);
  if((pde & PTE_V )== 0) panic("invalid pde! %x, satp: %x, pdr_addr: %x, addr: %x", cpu.pc, cpu.satp,  pde_addr, addr);
  paddr_t pte_addr = PTE_ADDR(pde) | (PTX(addr) << 2);
  PTE pte = paddr_read(pte_addr, len);
  if((pte & PTE_V) == 0) panic("invalid pte! %x, satp: %x, pte_addr: %x, addr: %x pte: %x, pde_addr: %x, pde: %x", cpu.pc, cpu.satp,  pte_addr, addr, pte, pde_addr, pde);
  paddr_t pa = PTE_ADDR(pte) | OFF(addr);
  return pa;
}


uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  check_vaddr_bound(addr);
  paddr_t paddr = page_walk(addr, 4);
  return paddr_read(paddr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  check_vaddr_bound(addr);
  paddr_t paddr = page_walk(addr, 4);
  return paddr_write(paddr, data, len);
}
