#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(void) {
  WP* ret = free_;
  ret->next = NULL;
  if(ret == NULL) panic("no free watchpoint");
  free_ = free_->next;
  if(head == NULL) {
    head = ret;
  }else{
    ret->next = head;
    head = ret;
  }
  return ret;
}

void free_wp(WP* wp) {
  if(head == wp) {
    head = head->next;
    wp->next = free_;
    free_ = wp;
  }else{
    for(int i=0;i<NR_WP;i++){
      if(wp_pool[i].next == wp){
        wp_pool[i].next = wp->next;
      }
    }
    wp->next = free_;
    free_ = wp;
  }
  return;
}

WP* get_head(void) {
  return head;
}

