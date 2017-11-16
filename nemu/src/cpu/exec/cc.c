#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
			*dest = cpu.eflags.OF;
			break;
		case CC_NO:
			*dest = !cpu.eflags.OF;
			break;
    case CC_B:
			*dest = cpu.eflags.CF;
			break;
		case CC_NB:
			*dest = !cpu.eflags.CF;
    case CC_E: 
			printf("in the e i\n");
			printf("%x\n", subcode & 0xe); 
			printf("%d\n", cpu.eflags.ZF);
			*dest = cpu.eflags.ZF;
		  printf("dest = %d\n", *dest);
			break;
    case CC_NE: 
			*dest = !cpu.eflags.ZF;
		  break;
		case CC_BE:
    case CC_S:
    case CC_L:
    case CC_LE: 
			*dest = ((cpu.eflags.ZF) || (cpu.eflags.SF != cpu.eflags.OF));
			break;
		case CC_NLE:
			*dest = ((!cpu.eflags.ZF) && (cpu.eflags.SF == cpu.eflags.OF));
			break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
