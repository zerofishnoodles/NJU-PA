#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
static int buf_num;

static void gen(char c) {
  buf[buf_num++] = c;
}
static void gen_num(void) {
  for(int i=0;i < 1+rand()%3; i++){
    if(i==0) {
      buf[buf_num++] = '0'+1+rand()%2;
    }else{
      buf[buf_num++] = '0'+rand()%10;
    }
  }
}

static void gen_rand_op(void) {
  switch (rand()%4)
  {
  case 0:
    buf[buf_num++] = '+';
    break;
  case 1:
    buf[buf_num++] = '-';
    break;
  case 2:
    buf[buf_num++] = '*';
    break;
  // division will be zero
  case 3:
    buf[buf_num++] = '/';
    break;
  default:
    break;
  }
}

static inline void gen_rand_expr() {
  if(buf_num > 65535){
    printf("expr too long!");
    exit(0);
  }  
  switch (rand()%3) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"#include <signal.h>\n"
"#include <stdlib.h>\n"
"void handler(int s) {exit(1);}"
"int main() { "
"  signal(SIGFPE, handler);"
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  buf_num = 0;
  for (i = 0; i < loop; i ++) {
    memset(buf, 0, 65536);
    buf_num = 0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    ret = pclose(fp);
    if(WEXITSTATUS(ret) == 1) continue;


    printf("%u %s\n", result, buf);
  }
  return 0;
}
