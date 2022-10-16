#include "common.h"
#include "syscall.h"
#include "fs.h"

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
      // naive_uload(NULL,"/bin/init");
      return c;
    case SYS_write:
      c->GPRx = fs_write((int)a[1], (const char *)a[2], (size_t)a[3]);
      return c;
    case SYS_brk:
      c->GPRx = sys_brk((uintptr_t)a[1]);
      return c;
    case SYS_open:
      c->GPRx = fs_open((const char *)a[1], (int)a[2], (int)a[3]);
      return c;
    case SYS_read:
      c->GPRx = fs_read((int)a[1], (void *)a[2], (size_t)a[3]);
      return c;
    case SYS_close:
      c->GPRx = fs_close((int)a[1]);
      return c;
    case SYS_lseek:
      c->GPRx = fs_lseek((int)a[1], (size_t)a[2], (int)a[3]);
      return c;
    case SYS_execve:
      naive_uload(NULL, (const char*)a[1]);
      c->GPRx = 0;
      return c;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
