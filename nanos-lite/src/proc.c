#include "proc.h"

#define MAX_NR_PROC 4

extern void naive_uload(PCB*, const char*);
extern void context_kload(PCB *pcb, void *entry);
extern void context_uload(PCB *pcb, void *entry);
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
static int choice;
PCB *current = NULL;

extern void naive_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");
  // naive_uload(NULL, "/bin/pal");
  // context_kload(&pcb[0], (void *)hello_fun);
  context_uload(&pcb[0], "/bin/pal");
  context_uload(&pcb[1], "/bin/hello");
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  // current = &pcb[0];
  for(int i=(choice+1)%MAX_NR_PROC;i<MAX_NR_PROC;){
    if(pcb[i].as.ptr != 0) {
      choice = i;
      break;
    }
    i = (i+1) % MAX_NR_PROC;
  }
  current = &pcb[choice];
  printf("schedule choice:%d \n",choice);
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;
}
