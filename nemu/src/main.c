#include <nemu.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *e, bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  // /* Receive commands from user. */
  // ui_mainloop(is_batch_mode);

  FILE *fp;
  bool success = true;
  fp = fopen("/home/hust/ics2019_1/nemu/tools/gen-expr/input", "r");
  if(fp == NULL) panic("file open failed!");
  while(!feof(fp)) {
    int res;
    char exp[65535] = {0};
    fscanf(fp, "%d %s\n", &res, exp);
    printf("%d %s  ", res, exp);
    int res_t = expr(exp, &success);
    if(success == false) panic("make token failed!");
    else if (res == res_t) {printf("success!\n");}
    else panic("expr wrong!");
  }

  return 0;
}
