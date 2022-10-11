#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  uint32_t res;
  char str[32];

} WP;

WP* new_wp(void);
void free_wp(WP *wp);
WP* get_head();

#endif
