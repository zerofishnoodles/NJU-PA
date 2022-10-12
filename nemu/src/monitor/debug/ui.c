#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display(void);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  char *ptr;
  uint64_t step_num = strtoul(args, &ptr, 10);
  cpu_exec(step_num);
  return 0;
}

static int cmd_info(char *args) {
  if(strcmp(args, "r") == 0) {
    isa_reg_display();
    return 0;
  }else if(strcmp(args, "w") == 0) {
    WP* cur = get_head();
    while(cur) {
      printf("watchpoint %d: %s\n", cur->NO, cur->str);
      cur = cur->next;
    }
    return 0;
  }
  return -1;
}

static int cmd_x(char *args) {
  char *token1 = strtok(args, " ");
  char *token2 = strtok(NULL, " ");
  char *ptr1, *ptr2;
  // for now it only accept fixed hex address instead of an expr
  int num_bytes = strtoul(token1, &ptr1, 10);
  vaddr_t addr = strtoul(token2+2, &ptr2, 16);
  for(int i=0;i<num_bytes;i++) {
    printf("0x%08x ", vaddr_read(addr+i*4, 4));
  }
  return 0;
}

static int cmd_watch(char *args) {
  bool success;
  uint32_t res = expr(args, &success);
  if(success == false) panic("expr wrong!");
  WP* wp = new_wp();
  wp->res = res;
  strcpy(wp->str, args);
  printf("Set watchpoint %d: %s\n", wp->NO, wp->str);
  return 0;
}

static int cmd_d(char *args) {
  int wp_no = strtol(args, NULL, 10);
  WP* cur = get_head();
  int flag = -1;
  while(cur) {
    if(cur->NO == wp_no){
      free_wp(cur);
      flag = 0;
    }
    cur = cur -> next;
  }
  if(flag == 0) printf("delete watchpoint %d\n", wp_no);
  return flag;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "step into [N] steps of the program", cmd_si },
  { "info", "get the info of [arg]", cmd_info},
  { "x", "dispaly the [N] bytes content of the address [expr]", cmd_x},
  { "w", "set watchpoint of an [expr]", cmd_watch},
  { "d", "delete watchpoint of [NO]", cmd_d},

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
