#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int trans(char *e);
void cpu_exec(uint64_t);
void init_regex();
void display_wp();
void insert_wp(char *args);
void delete_wp(int no);
uint32_t expr(char *e, bool *success);
uint32_t vaddr_read(vaddr_t addr, int len);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
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
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Let the program execute n steps", cmd_si },
  { "info", "Display the register status and the watchpoint information", cmd_info},
  { "x", "Caculate the value of expression and display the content of the address", cmd_x},
  { "p","Calculate an expression", cmd_p},
  { "w", "Create a watchpoint", cmd_w},
  { "d", "Delete a watchpoint", cmd_d},
  /* TODO: Add more commands */
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  //printf("111%s\n%s\n", args, arg);
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

static int cmd_si(char *args) {
  /*get the steps number*/
  int steps;
  if (args == NULL){
    steps = 1;
  }
  else{
    steps = atoi(strtok(NULL, " "));
  }

  cpu_exec(steps);
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) {
    printf("Please input the info r or info w\n");
  }
  else {
    if (strcmp(args, "r") == 0) {
      printf("eax:  0x%-10x    %-10d\n", cpu.eax, cpu.eax);
      printf("edx:  0x%-10x    %-10d\n", cpu.edx, cpu.edx);
      printf("ecx:  0x%-10x    %-10d\n", cpu.ecx, cpu.ecx);
      printf("ebx:  0x%-10x    %-10d\n", cpu.ebx, cpu.ebx);
      printf("ebp:  0x%-10x    %-10d\n", cpu.ebp, cpu.ebp);
      printf("esi:  0x%-10x    %-10d\n", cpu.esi, cpu.esi);
      printf("esp:  0x%-10x    %-10d\n", cpu.esp, cpu.esp);
      printf("eip:  0x%-10x    %-10d\n", cpu.eip, cpu.eip);
    }
    else if (strcmp(args, "w") == 0) {
      display_wp();
    }
    else {
      printf("The info command need a parameter 'r' or 'w'\n");
    }
  }
  return 0;
}

static int cmd_x(char *args) {
  if (args == NULL) {
    printf("Input invalid command!\n");
  }
  else {
    int num, addr, i;
    char *exp;
    num = atoi(strtok(NULL, " "));
    exp = strtok(NULL, " ");
    addr = trans(exp);

    for (i = 0; i < num; i++) {
      printf("0x%x\n", vaddr_read(addr, 4));
      addr += 4;
    }

  }
  return 0;
}

static int cmd_p(char *args) {
  if (args == NULL) {
    printf("Input invalid command! Please input the expression.\n");
  }
  else {
    init_regex();

    bool success = true;
    //printf("args = %s\n", args);
    int result = expr(args, &success);

    if (success) {
      printf("result = %d\n", result);
    }
    else {
      printf("Invalid expression!\n");
    }
  }
  return 0;
}

static int cmd_w(char *args) {
  if (args == NULL) {
    printf("Input invalid command! Please input the expression.\n");
  }
  else {
    insert_wp(args);
  }
  return 0;
}

static int cmd_d(char *args) {
  if (args == NULL) {
    printf("Input invalid command! Please input the NO.\n");
  }
  else {
    int no = atoi(args);
    delete_wp(no);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
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

int trans(char *e) {
  int len, num, i, j;
  len = strlen(e);
  num = 0;
  j = 1;

  for (i = len-1; i > 1; i--) {
    num += (e[i]-'0')*j;
    j *= 16;
  }
//  printf("num = %d\n", num);

  return num;
}
