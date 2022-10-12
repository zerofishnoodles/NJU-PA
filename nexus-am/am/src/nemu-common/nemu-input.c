#include <am.h>
#include <amdev.h>
#include <nemu.h>


size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      uint32_t key = inl(KBD_ADDR);
      kbd->keydown = (uint32_t)(key&0x8000) >> 15;
      kbd->keycode = key;
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
