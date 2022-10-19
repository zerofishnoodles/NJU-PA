#include <am.h>
#include <riscv32.h>
#include <nemu.h>
#include "klib.h"

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0x80000000u, .end = (void*)(0x80000000u + PMEM_SIZE)},
  {.start = (void*)MMIO_BASE,   .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

static inline void set_satp(void *pdir) {
  // printf("pdir: %x", 0x80000000 | ((uintptr_t)pdir >> 12));
  asm volatile("csrw satp, %0" : : "r"(0x80000000 | ((uintptr_t)pdir >> 12)));
}

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  // make all PDEs invalid
  int i;
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = ((uintptr_t)ptab >> PGSHFT << 10) | PTE_V;
      // printf("kpde: %x %x\n", &kpdirs[pdir_idx], kpdirs[pdir_idx]);

      // fill PTE
      PTE pte = (PGADDR(pdir_idx, 0, 0) >> PGSHFT << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
      PTE pte_end = (PGADDR(pdir_idx + 1, 0, 0) >> PGSHFT << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
      for (; pte < pte_end; pte += (1 << 10)) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_satp(kpdirs);
  vme_enable = 1;

  return 0;
}

int _protect(_AddressSpace *as) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable) {
    set_satp(c->as->ptr);
    cur_as = c->as;
    // printf("switch satp to %x\n", c->as->ptr);
  }
}

int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  // printf("map va: %x pa: %x as-ptr: %x %d\n", va, pa, as->ptr, (PDX(va) << 2));

  PTE pte = (((paddr_t)pa >> PGSHFT) << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
  PDE pde;
  paddr_t pte_addr;
  paddr_t pde_addr = (paddr_t)as->ptr + (PDX(va) << 2);

  //check exist and get pte addr
  memcpy(&pde, pde_addr, sizeof(PDE));
  if((pde & PTE_V) != 1){
    paddr_t t_pa = pgalloc_usr(1);
    pde = ((t_pa >> PGSHFT) << 10) | PTE_V;
    memcpy(pde_addr, &pde, sizeof(PDE));
  }

  pte_addr = PTE_ADDR(pde) | (PTX(va) << 2);
  memcpy(pte_addr, &pte, sizeof(PTE));

  // printf("pte: %x %x\n" ,pte_addr, pte);
  // printf("pde: %x %x\n" ,pde_addr, pde);

  return 0;
}

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  _Context *new_ctx = ustack.end - 3 * sizeof(uint32_t) - sizeof(_Context);  // there are 3 args of _start
  memset(new_ctx, 0, sizeof(_Context) + 3 * sizeof(uint32_t));

  new_ctx->as = as;
  new_ctx->epc = (uintptr_t)entry;
  return new_ctx;
}
