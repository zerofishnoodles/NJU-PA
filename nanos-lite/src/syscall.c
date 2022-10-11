#include "common.h"
#include "syscall.h"

int32_t sys_yield() {
  _yield();
  return 0;
}

size_t sys_write(int fd, char *buf, size_t count) {
  if(fd == 1 || fd == 2) {
    for(int i=0; i<count; i++) {
      _putc(buf[i]);
    }
  }
  return count;
}

int sys_brk(uintptr_t pb) {
  return 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:
      c->GPRx = sys_yield();
      return c;
    case SYS_exit:
      _halt(a[1]);
      return c;
    case SYS_write:
      c->GPRx = sys_write(a[1], (char *)a[2], a[3]);
      return c;
    case SYS_brk:
      c->GPRx = sys_brk((uintptr_t)a[1]);
      return c;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
