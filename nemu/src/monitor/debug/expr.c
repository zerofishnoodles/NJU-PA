#include <nemu.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

uint32_t isa_reg_str2val(const char *s, bool *success);


enum {
  TK_NOTYPE = 256, TK_EQ, 

  /* TODO: Add more token types */
  TK_INT, TK_HEX, TK_REG, TK_NEQ, TK_AND, TK_OR, TK_POINT
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"-", '-'},         // minus
  {"\\*", '*'},         // mutiply
  {"\\/", '/'},           // divide
  {"\\(", '('},           // left parentheses
  {"\\)", ')'},           // right parentheses
  {"(0x[1-9a-fA-f][0-9a-fA-F]*)|0x0", TK_HEX},  // HEX
  {"([1-9][0-9]*)|0", TK_INT},   // integer
  {"\\$(\\$0|ra|sp|gp|tp|t[0-6]|s[0-9]|a[0-7]|s10|s11)", TK_REG}, // reg
  {"\\!=", TK_NEQ},  // not equal
  {"&&", TK_AND},  // and
  {"\\|\\|", TK_OR},  // or

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

void print_tokens(void) {
  for(int i=0;i<nr_token;i++){
    printf("token %d:%c %s\n",i, tokens[i].type == TK_INT ? 'd' : 
      tokens[i].type == TK_HEX ? 'x': 
      tokens[i].type == TK_AND ? '&':
      tokens[i].type == TK_OR ? '|':
      tokens[i].type == TK_EQ ? '=':
      tokens[i].type == TK_NEQ ? '!': 
      tokens[i].type == TK_REG ? 'r': 
      tokens[i].type, 
      tokens[i].str);
  }
}

void clean_tokens(void) {
  for(int i=0;i<nr_token;i++){
    tokens[i].type = 0;
    memset(tokens[i].str, 0, 32);
  }
  nr_token = 0;
}

static bool if_certain_op(int p) {
  switch(tokens[p].type){
    case '+':
    case '-':
    case '*':
    case '/':
    case '(':
      return true;
    default:
      return false;
  }
}

static void convert_neg_int(void) {
  for(int i=0;i<nr_token;i++) {
    if(tokens[i].type == '-' && (i == 0 || if_certain_op(i-1) == true)){
      if(i+1>=nr_token || (tokens[i+1].type != TK_INT && tokens[i+1].type != TK_HEX)) continue;
      else {
        tokens[i+1].str[0] = '-';
        // delete '-' token
        for(int j=i+1;j<nr_token;j++) tokens[j-1] = tokens[j];
        nr_token--;
      }
    }
  }
  // add 0 brefor --1
  if(tokens[0].type == '-') {
    for(int j=nr_token-1;j>=0;j--) tokens[j+1] = tokens[j];
    tokens[0].type = TK_INT;
    strcpy(tokens[0].str, "0");
    nr_token++;
  }
  return; 
}

static bool is_point(int p) {
  if((p > 0 && if_certain_op(p-1) == true) || p == 0) return true;
  return false;
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    // printf("pos: %d\n", position);
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case '*':
            if(is_point(nr_token)){
              tokens[nr_token++].type = TK_POINT;
            }else{
              tokens[nr_token++].type = '*';
            }
            break;
          case '+':
          case '-':
          case '/':
          case '(':
          case ')':
          case TK_AND:
          case TK_EQ:
          case TK_NEQ:
          case TK_OR:
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case TK_NOTYPE:
            break;
          case TK_INT:
            tokens[nr_token].type = TK_INT;
            Assert(substr_len <= 10, "Integer too big");  // 2^31 ~= 2*10^9 which is 10 chars 
            strncpy(tokens[nr_token].str+1, substr_start, substr_len);  // str[0] save for sign
            tokens[nr_token].str[0] = '+';
            nr_token++;
            break;
          case TK_HEX:
            tokens[nr_token].type = TK_HEX;
            Assert(substr_len <= 10, "Integer too big");  // assume the same as dec
            strncpy(tokens[nr_token].str+1, substr_start, substr_len);  // str[0] save for sign
            tokens[nr_token].str[0] = '+';
            nr_token++;
            break;
          case TK_REG:
            tokens[nr_token].type = TK_REG;
            strncpy(tokens[nr_token].str, substr_start+1, substr_len-1);
            nr_token++;
            break;
          default: 
            panic("Unknown Token!");
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  convert_neg_int();
  return true;
}

static int op_prior(int type) {
  // this priority is opposite to the linux priority
  int prior;
  switch (type)
  {
  case TK_POINT:
    prior = 2;
    break;
  case '*':
  case '/':
    prior = 3;
    break;
  case '+':
  case '-':
    prior = 4;
    break;
  case TK_EQ:
  case TK_NEQ:
    prior = 7;
    break;
  case TK_AND:
    prior = 11;
  case TK_OR:
    prior = 12;
  default:
    panic("Unkonwn operation!");
  }
  return prior;
}

static bool check_parentheses(int p, int q) {
  int st[32];
  int top = 0;
  int flag = 0;
  // check wether legal --- simple 
  for(int i=p;i<=q;i++){
    // ()()-- not allowed
    if(i > p && tokens[i].type == '(' && tokens[i-1].type == ')') panic("Bad expression!");

    // stack
    if(i == p) {st[top++] = i; continue;}
    if(tokens[i].type == '(') {st[top++] = i; continue;}
    if(tokens[i].type == ')') {
      if(top > 0 && tokens[st[top-1]].type == '(') {
        // check wether erase parentheses
        if(i == q && st[top-1] == p) flag = 1;

        top--; 
        continue;
      }
      else panic("Bad expression!");
    }
  }

  if(flag) return true;
  else return false;
}

static int find_main_token(int p, int q) {
  // printf("p:%d q: %d", p,q);
  int flag = 0;
  int cand = -1;
  for(int i=p; i <= q; i++) {
    if(tokens[i].type == TK_INT || tokens[i].type == TK_HEX || tokens[i].type == TK_REG) continue;
    if(tokens[i].type == '(') {flag++; continue;}
    if(tokens[i].type == ')') {flag--; continue;}
    if(flag == 0) {
      if(cand == -1) cand = i;
      else if(op_prior(tokens[i].type) >= op_prior(tokens[cand].type)) cand = i;
    }
  }
  return cand;
}

static int eval(int p, int q) {
  if (p > q) {
    /* Bad expression */
    panic("Bad expression!");
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    char **temp = NULL;
    bool success;
    int res;
    switch(tokens[p].type){
      case TK_INT:
        res =  strtol(tokens[p].str, temp, 10);
        break;
      case TK_HEX:
        res = strtol(tokens[p].str, temp, 16);
        break;
      case TK_REG:
        res = isa_reg_str2val(tokens[p].str, &success);
        if(success == false) panic("read reg failed!");
        break;
      default:
        panic("Unknown single token!");
    }
    return res;
    
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    int op = find_main_token(p,q);
    int val1, val2;
    if(tokens[op].type == TK_POINT) {
      val1 = 0;
    }else{
      val1 = eval(p, op - 1);
    }
    val2 = eval(op + 1, q);
    // printf("op: %d val1: %d val2: %d\n", op, val1, val2);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/':
        if(val2 == 0) panic("divide by zero!");
        return val1 / val2;
      case TK_POINT: return isa_vaddr_read(val2, 4);
      case TK_EQ: return val1==val2;
      case TK_NEQ: return val1!=val2;
      case TK_AND: return val1&&val2;
      case TK_OR: return val1||val2;
      default: panic("Unkonwn operation!");;
    }
  }
}

uint32_t expr(char *e, bool *success) {
  clean_tokens();
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // print_tokens();

  /* TODO: Insert codes to evaluate the expression. */
  uint32_t ret = eval(0, nr_token-1);
  // printf("res: %d\n", ret);

  return ret;
}
