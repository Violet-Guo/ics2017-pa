#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);
	Log("filename = %s", filename);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  //_switch(&pcb[i].as);
  //current = &pcb[i];
  //((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

int cnt = 0;

_RegSet* schedule(_RegSet *prev) {
	// save the context pointer
	current->tf = prev;
	cnt++;

	// always select pcb[0] as the new process
	//current = &pcb[0];
	//current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
	if (cnt % 100 == 0) {
		current = &pcb[1];
	}
	else {
		current = &pcb[0];
	}

	// TODO: switch to the new address space,
	// then return the new context
	//Log("current->as = %x, current->tf = %x", current->as, current->tf);
	_switch(&current->as);
	//Log("current->as = %x, current->tf = %x", current->as, current->tf);
  return current->tf;
}
