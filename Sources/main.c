#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "dev.h"

//Program FLASH  128K
//RAM              8K
//DATA FLASH       4K
//V0.0.1 测试成功
/*
  V0.0.2 写入前对比flash内已经有的内容如果内容相等直接返回成功



*/

const unsigned char flash_array[] @0xFF08 = {0xFF,0xFF,0xFF,0xFF,0xCF,0xFF,0xFF,0xFE};
#pragma CODE_SEG RAM_CODE
static void main_loop(void){
  for(;;) {
    _FEED_COP(); /* feeds the dog */
    uart0_work();
  } /* loop forever */
}
#pragma CODE_SEG DEFAULT
void start_user_app(void){
  uint16_t *src = (uint16_t *)0xEF00;
  if(*src ++ != 0x55AA)return;
  sys_start_app(0xEF00);
}
void main(void) {
  board_init();
  uart0_init();
  IRQCR_IRQEN=0x80;
  EnableInterrupts;
  main_loop();
}
