
#include "dev.h"


void sys_start_app(uint16_t u16addr){
  u16addr = u16addr & 0xFF00;
  IVBR = (u16addr >> 8);
  u16addr += 0xFE;
  DisableInterrupts;
  u16addr = *(uint16_t *) u16addr;
  __asm  ldd #0x55AA;
  __asm  ldx u16addr;
  __asm  jmp 0,x;
}