#ifndef _DEV_H
#define _DEV_H

#include "derivative.h"
#include <hidef.h>      /* common defines and macros */



typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned long  uint32_t;
typedef signed char int8_t;
typedef short int int16_t;
typedef long int32_t;


//Òý½Å¶¨Òå
#define BATTERY_LED0          PORTB_PB3
#define BATTERY_LED1          PORTB_PB5
#define BATTERY_LED2          PORTB_PB4
#define BATTERY_LED3          PORTB_PB1

#define BATTERY_LED0_DIR          DDRB_DDRB3
#define BATTERY_LED1_DIR          DDRB_DDRB5
#define BATTERY_LED2_DIR          DDRB_DDRB4
#define BATTERY_LED3_DIR          DDRB_DDRB1



//CD4052
#define BOARD_CD4052_CA PTM_PTM3
#define BOARD_CD4052_CB PTM_PTM2
#define BOARD_CD4052_INH PTM_PTM5
#define BOARD_CD4052_CA_DIR DDRM_DDRM3
#define BOARD_CD4052_CB_DIR DDRM_DDRM2
#define BOARD_CD4052_INH_DIR DDRM_DDRM5
#define BOARD_CD4052_CA_PULL PERM_PERM3
#define BOARD_CD4052_CB_PULL PERM_PERM2
#define BOARD_CD4052_INH_PULL PERM_PERM5


#define LTC2950_KILL        PORTB_PB2
#define LTC2950_KILL_DIR    DDRB_DDRB2


void start_user_app(void);
void sys_start_app(uint16_t u16addr);
void uart0_app_command(uint8_t *src,uint8_t size);

#pragma CODE_SEG RAM_CODE 
void interrupt  uart0_Vectch0(void);
void uart0_write(uint8_t u8val);
void uart0_res_start(uint8_t u8Cmd,uint8_t u8size);
void uart0_res_write_u8(uint8_t u8);
void uart0_res_write_u16(uint16_t u16);
void uart0_work_write_crc(void);
void uart0_tick(void);
void uart0_work(void);
#pragma CODE_SEG DEFAULT
void uart0_init(void);


#pragma CODE_SEG RAM_CODE
void board_battery_led(uint8_t u8);
#pragma CODE_SEG DEFAULT
void board_init(void);

#pragma CODE_SEG RAM_CODE
uint8_t  pflash_erase(uint32_t u32low,uint32_t u32high);
uint8_t pflash_write(uint32_t u32addr, uint8_t *src,uint8_t u8size);
#pragma CODE_SEG DEFAULT

#define  BUS_CLOCK          32000000
#define  VECTOR_BASE        0x3F00


#define __SEG_START_REF(a)  __SEG_START_ ## a
#define __SEG_END_REF(a)    __SEG_END_ ## a
#define __SEG_SIZE_REF(a)   __SEG_SIZE_ ## a
#define __SEG_START_DEF(a)  extern uint8_t __SEG_START_REF (a) []
#define __SEG_END_DEF(a)    extern uint8_t __SEG_END_REF   (a) []
#define __SEG_SIZE_DEF(a)   extern uint8_t __SEG_SIZE_REF  (a) []

__SEG_START_DEF (RAM_CODE);
__SEG_END_DEF   (RAM_CODE);
__SEG_SIZE_DEF  (RAM_CODE);


//uart0


#endif