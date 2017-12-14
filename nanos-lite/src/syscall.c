#include "common.h"
#include "syscall.h"

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4], result;
  a[0] = SYSCALL_ARG1(r);
	a[1] = SYSCALL_ARG2(r);

  switch (a[0]) {
		case SYS_none:
			result = 1;
			break;
    case SYS_exit:
			_halt(a[1]);
			break;
		default: panic("Unhandled syscall ID = %d", a[0]);
  }
	
	SYSCALL_ARG1(r) = result;

  return NULL;
}
