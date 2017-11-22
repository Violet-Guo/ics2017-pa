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
    case CC_O:  //0
			*dest = cpu.eflags.OF;
			break;
		case CC_NO: //1
			*dest = !cpu.eflags.OF;
			break;
    case CC_B:  //2
			*dest = cpu.eflags.CF;
			break;
		case CC_NB: //3
			*dest = !cpu.eflags.CF;
			break; 
    case CC_E:  //4 
			*dest = cpu.eflags.ZF;
			break;
    case CC_NE: //5
			*dest = !cpu.eflags.ZF;
		  break;
		case CC_BE: //6
			*dest = ((cpu.eflags.CF) || (cpu.eflags.ZF));
			break;
		case CC_NBE://7
			*dest = !((cpu.eflags.CF) || (cpu.eflags.ZF));
			break;
    case CC_S:  //8
    case CC_NS: //9
		case CC_L:  //12 c
			*dest = (cpu.eflags.SF == cpu.eflags.OF);
			break;
		case CC_NL: //13 d
			*dest = (cpu.eflags.SF != cpu.eflags.OF);
    case CC_LE: //14 e
			printf("i am here\n");
			*dest = ((cpu.eflags.ZF) || (cpu.eflags.SF != cpu.eflags.OF));
			break;
		case CC_NLE://14 f
			*dest = !((cpu.eflags.ZF) || (cpu.eflags.SF != cpu.eflags.OF));
			break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
