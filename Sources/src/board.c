#include "dev.h"



uint8_t board_u8_led;
uint16_t board_tick_u16;
#pragma CODE_SEG RAM_CODE
void board_battery_led(uint8_t u8){
  switch(u8){
    case 0x00:
       BATTERY_LED0=0x01;BATTERY_LED1=0x01;BATTERY_LED2=0x01;BATTERY_LED3=0x01;break;
    case 0x01:
       BATTERY_LED0=0x0;BATTERY_LED1=0x01;BATTERY_LED2=0x01;BATTERY_LED3=0x01;break;
    case 0x02:
      BATTERY_LED0=0x0;BATTERY_LED1=0x0;BATTERY_LED2=0x01;BATTERY_LED3=0x01;break;
    case 0x03:
      BATTERY_LED0=0x0;BATTERY_LED1=0x0;BATTERY_LED2=0x0;BATTERY_LED3=0x01;break;
    case 0x04:
      BATTERY_LED0=0x0;BATTERY_LED1=0x0;BATTERY_LED2=0x0;BATTERY_LED3=0x0;break;
  }
}

void interrupt  board_interrupt_rti(void){
  if(CRGFLG_RTIF == 1) CRGFLG_RTIF = 1;
  board_tick_u16++;
  if( board_tick_u16 % 200 == 0){
    board_battery_led(board_u8_led);
    board_u8_led++;
    if(board_u8_led > 4)board_u8_led = 0;
  }
  uart0_tick();
}
void interrupt board_unknow_interrupt(void){
    for(;;){
    }
}

#pragma CODE_SEG DEFAULT



static void board_init_gpio(void){
  PUCR_PUPBE = 0x01;
  LTC2950_KILL_DIR = 0x01;
  LTC2950_KILL = 0x01;
  board_u8_led = 0x00;
  
  
  
  BOARD_CD4052_CA_PULL = 0x01;
  BOARD_CD4052_CB_PULL = 0x01;
  BOARD_CD4052_INH_PULL = 0x01;
  
  BOARD_CD4052_CA_DIR = 0x01;
  BOARD_CD4052_CB_DIR = 0x01;
  BOARD_CD4052_INH_DIR = 0x01;

  BOARD_CD4052_CA = 0x01;
  BOARD_CD4052_CB = 0x00;
  BOARD_CD4052_INH = 0x00;
  
  BATTERY_LED0_DIR = 0x01;
  BATTERY_LED1_DIR = 0x01;
  BATTERY_LED2_DIR = 0x01;
  BATTERY_LED3_DIR = 0x01;
}
static void board_init_pll(void) 
{             
    CLKSEL &= 0x7f;       //set OSCCLK as sysclk
    PLLCTL &= 0x8F;       //Disable PLL circuit
    CRGINT &= 0xDF;
    
    #if(BUS_CLOCK == 40000000) 
      SYNR = 0x44;
    #elif(BUS_CLOCK == 32000000)
      SYNR = 0x43;     
    #elif(BUS_CLOCK == 24000000)
      SYNR = 0x42;
    #endif 

    REFDV = 0x81;         //PLLCLK=2×OSCCLK×(SYNR+1)/(REFDV+1)＝64MHz ,fbus=32M
    PLLCTL =PLLCTL|0x70;  //Enable PLL circuit
    asm NOP;
    asm NOP;
    while(!(CRGFLG&0x08)); //PLLCLK is Locked already
    CLKSEL |= 0x80;        //set PLLCLK as sysclk
    
    FCLKDIV = 0x0F;
}
void board_init_rti(void){
  CRGINT = 0x80;       //使能实时中断
  RTICTL = 0x97;       //10ms
}
static void board_copy_ram_code(void){
  static uint8_t *src;
  uint8_t *dst;
  uint16_t segSize;
  uint16_t i;

  src = (uint8_t *)__SEG_START_REF(RAM_CODE);
  segSize = (uint16_t)__SEG_SIZE_REF(RAM_CODE);
  dst = (uint8_t *)0x3A00;   
  for(i=0;i<segSize;i++){
    *dst++ = *src++;
  }
}

void board_init_interrupt(void){
  uint8_t i;
  uint16_t *src = (uint16_t *)0x3F10;
  for(i=0;i<VectorNumber_Vsi;i++){
    *src = (uint16_t)board_unknow_interrupt;
    src ++;
  }
  IVBR = (VECTOR_BASE >> 8);
  *(uint16_t *)(VECTOR_BASE + (Vrti & 0xFF)) = (uint16_t)board_interrupt_rti;
  *(uint16_t *)(VECTOR_BASE + (Vsci0 & 0xFF)) = (uint16_t)uart0_Vectch0;
}
void board_init(void){
  board_init_gpio();
  board_init_pll();
  board_init_rti();
  board_copy_ram_code();
  board_init_interrupt();
}