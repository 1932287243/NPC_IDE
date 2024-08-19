#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "VCPU___024root.h"
#include "VCPU.h"

const char *regs[] = {
  "$0", "ra", "tp", "sp", "a0", "a1", "a2", "a3",
  "a4", "a5", "a6", "a7", "t0", "t1", "t2", "t3",
  "t4", "t5", "t6", "t7", "t8", "rs", "fp", "s0",
  "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8"
};

static int is_batch_mode = false;
int skip_batch_flag = 0;

void init_regex();
void init_wp_pool();


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
  w->mNPC->exec(-1);
  return 0;
}

static int cmd_si(char *args) {
  /* extract the first argument */
  char *arg = strtok(args, " ");
  int step_num;

  if (arg == NULL) 
  {
    step_num = 1;
    printf("USAGE: si N\n");
  }
  else
  {
    step_num = atoi(args);
    w->mNPC->exec(step_num);
  }
  return 0;
}

static int cmd_info(char *args) {
  /* extract the first argument */
  char *arg = strtok(args, " ");
  if (arg == NULL) 
  {
    printf("info SUBCMD\n");
    return 0;
  }
  else if(*arg == 'r')
  {
    extern VCPU* top;
    for(int i=0; i<4; i++)
    {
      for(int j=0; j<8; j++)
      {
        printf("%s=0x%08x\t", regs[8*i+j], w->mNPC->top->rootp->CPU__DOT__reg_file__DOT__rf[8*i+j]);
      }
      printf("\n");
    }
  }
  else if(*arg == 'w')
  {
    watch_info();
  }
  return 0;
}

static int cmd_x(char *args) {
  char *arg = strtok(args, " ");
  int num = atoi(arg);
  uint32_t pmem_val;
  uint32_t pmem_addr;
  char *expr_ptr = arg+strlen(arg)+1;
  // printf("expr=%s\n", expr_ptr);
  bool success;
  pmem_addr = expr(expr_ptr, &success);
  // printf("\033[41;31mpmem_addr=0x%08x\033[0m\n", pmem_addr);
  // char *expr = strtok(arg+strlen(arg)+1, " ");
  // uint32_t val = (uint32_t)strtol(expr, NULL, 16);
  
  if(arg == NULL || expr_ptr == NULL)
  {
    printf("Usage:x N EXPR\n");
    return 0;
  }
  else
  {
    uint32_t i;
    uint32_t j;
    for(i=0; i<num; i++)
    {
        printf("0x%08x:\t", pmem_addr);
        for(j=0; j<4; j++)
        {
          printf("show memory value\n");
          printf("%02x\t", pmem_val);
        }
        printf("\n");
    }
  }
  return 0;
}

static int cmd_p(char *args) {
  uint32_t val;
  if(args == NULL)
  {
    printf("Usage:p EXPR\n");
  }
  else
  {
    bool success;
    val = expr(args, &success);
    printf("\033[41;31mval=0x%08x\033[0m\n", val);
  }
  return 0;
}

static int cmd_w(char *args) {
  // char *arg = strtok(args, " ");
  // uint32_t val;
  if(args == NULL)
  {
    printf("Usage:w EXPR\n");
  }
  else
  {
    WP *wp;
    wp = new_wp();
    strcpy(wp->expr, args);
    // printf("\033[41mID=%d\033[0m\n", wp->NO);
    // bool success;
    // val = expr(args, &success);
    // printf("\033[41;31mval=0x%08x\033[0m\n", val);
  }
  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(args, " ");
  
  int w_num = atoi(arg);
  if(arg == NULL)
  {
    printf("Usage:d N\n");
  }
  else
  {
    free_wp_no(w_num);
  }
  return 0;
}


static int cmd_q(char *args) {
  return -1;
  // exit(0);
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Pause the program after stepping through N instructions, \
  When N is not given, it defaults to 1", cmd_si },
  { "info", "Print register status.Print monitoring point information", cmd_info },
  { "x", "Find the value of the expression EXPR and use the result as the starting memory\
    Address, output N consecutive 4-byte addresses in hexadecimal format", cmd_x },
  { "p", "Find the value of the expression EXPR", cmd_p },
  { "w", "Pause program execution when the value of expression EXPR changes", cmd_w },
  { "d", "Delete monitoring point with serial number N", cmd_d },
  /* TODO: Add more commands */

};

#define NR_CMD (int)(sizeof(cmd_table) / sizeof(cmd_table[0]))
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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode && skip_batch_flag==0) {
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
    char *args = str + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
    skip_batch_flag = 0;
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
