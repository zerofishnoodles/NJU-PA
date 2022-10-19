#include <am.h>
#include <riscv32.h>
#include "klib.h"

static _Context* (*user_handler)(_Event, _Context*) = NULL;
extern void __am_get_cur_as(_Context *c);
extern void __am_switch(_Context *c);

_Context* __am_irq_handle(_Context *c) {
  __am_get_cur_as(c);
  _Context *next = c;
  if (user_handler) {
    _Event ev = {0};
    switch (c->cause) {
      case -1:
        ev.event = _EVENT_YIELD; 
        break;
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
        ev.event = _EVENT_SYSCALL;
        break;
      case IRQ_TIMER:
        ev.event = _EVENT_IRQ_TIMER;
        break;
      default: ev.event = _EVENT_ERROR; break;
    }
    
    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }
  __am_switch(next);
  return next;
}

extern void __am_asm_trap(void);

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  _Context *new_ctx = stack.end - sizeof(_Context);
  memset(new_ctx, 0, sizeof(_Context));
  new_ctx->epc = (uintptr_t)entry;
  
  return new_ctx;
}

void _yield() {
  asm volatile("li a7, -1; ecall");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}

