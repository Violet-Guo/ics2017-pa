#include "nemu.h"
#include "monitor/monitor.h"

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

int nemu_state = NEMU_STOP;

void exec_wrapper(bool);
int * haschanged();

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  //printf("n = %lu\n", n);
  if (nemu_state == NEMU_END) {
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  }
  nemu_state = NEMU_RUNNING;

  bool print_flag = n < MAX_INSTR_TO_PRINT;
  //printf("i am out the for\n");
  for (; n > 0; n --) {
    /* Execute one instruction, including instruction fetch,
     * instruction decode, and the actual execution. */
    //printf("%lu\ni am in the cpu exec\n", n);
    exec_wrapper(print_flag);

#ifdef DEBUG
    /* TODO: check watchpoints here. */
  int *no = haschanged();
  if (*no != -1) {
    int i;
    for (i = 0; *(no + i) != -1; i++) {
      printf("NO.%d ", *(no + i));
    }
    printf("watchpoint has been changed\n");
    nemu_state = NEMU_STOP;
  }
#endif

#ifdef HAS_IOE
    extern void device_update();
    device_update();
#endif

    if (nemu_state != NEMU_RUNNING) { return; }
  }

  if (nemu_state == NEMU_RUNNING) { nemu_state = NEMU_STOP; }
}
