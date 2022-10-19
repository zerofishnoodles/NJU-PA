#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  _yield();
  for(int i=0;i<len;i++) _putc(((char *)buf)[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  _yield();
  int key = read_key();
  int down = 0;
  if(key&0x8000){
    key^=0x8000;
    down=1;
  }
  if(key != _KEY_NONE) len = sprintf(buf, "%s %s\n", down ? "kd": "ku", keyname[key]);
  else len = sprintf(buf, "t %d\n",uptime());
  return len;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  len = sprintf(buf, dispinfo+offset);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  _yield();
  int off = offset >> 2;
  int length = len >> 2;

  int x = off % screen_width();
  int y = off / screen_width();
  int w,h;
  if(screen_width() - x >= length){
    w = length;
    h = 1;
    draw_rect((uint32_t*)buf, x, y, w, h);
  }else {
    w = screen_width() - x;
    h = length / w;
    draw_rect((uint32_t*)buf, x, y, w, h);
    draw_rect((uint32_t*)buf+(4*w*h), x, y+h, length % w, 1);
  }
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\n",screen_width(),screen_height());
}



