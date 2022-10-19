#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  if(current->max_brk == 0) current->max_brk = brk;
  intptr_t new_brk = brk + increment;
  int nr_pg = (((int32_t)new_brk & ~0xfff) - ((int32_t)current->max_brk & ~0xfff)) >> 12;
  // char buf[256];
  // sprintf(buf, "brk: %x increment: %x max_brk: %x pgsize: %d\n", brk, increment, current->max_brk, nr_pg);
  // int i=0;
  // while (buf[i] !=0 )
  // {
  //   _putc(buf[i]);
  //   i++;
  // }
  
  for(int i=0; i<nr_pg;i++){
    uint32_t paddr = new_page(1);
    _map(&current->as, current->max_brk + (i) * PGSIZE, paddr, 0);
  }
  current->max_brk = new_brk;
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %x", pf);

  _vme_init(new_page, free_page);
}
