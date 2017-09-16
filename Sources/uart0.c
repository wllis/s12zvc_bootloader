#include "dev.h"

static uint8_t uart0_flag;
static uint8_t uart0_rcv_tick;
static uint8_t uart0_cmd[120];
static uint8_t uart0_pos;
static uint8_t uart0_crc;




void uart0_init(void){
  SCI0BD = BUS_CLOCK/16/9600;   //
  SCI0CR1 = 0x00;       //
  
  SCI0CR2 = 0x2C;       //
  uart0_pos = 0x00;
  uart0_flag = 0x00;
  uart0_rcv_tick = 0x00;
}
static uint8_t uart0_res_crc;

#pragma CODE_SEG RAM_CODE 
static void uart0_work_ver(void){
   uart0_res_start(0x20,0x08);
   uart0_res_write_u8(0x00);
   uart0_res_write_u16(0x00);
   uart0_work_write_crc();
}
static void uart0_reboot(uint16_t u16mode){
  uint16_t *src;
  DisableInterrupts;
  src = (uint16_t *)0x2000;
  *src = u16mode;
  COPCTL = 0x01;        //enable watchdog
  ARMCOP = 0x00;  
}
static void uart0_write_succeed(uint8_t u8cmd,uint8_t u8Ret){
  uart0_res_start(u8cmd,0x06);
  uart0_res_write_u8(u8Ret);
  uart0_work_write_crc();
}
static void uart0_work_erase(void){
  uint8_t u8val;
  //DisableInterrupts;
  u8val = pflash_erase(0x7E0000UL, 0x7FF000UL);
 // EnableInterrupts;
  uart0_write_succeed(0x21,0x00);
}
static void uart0_rewrite_flash(uint32_t addr,uint8_t *src){
  // uint16_t *u16src = (uint16_t *)src;
   switch(addr){
    case 0x7FEF00:
     // u16src[0x0B] = (uint16_t)uart0_app_vect;
      break;
   }  
}
static uint8_t uart0_work_write_equal(uint32_t u32addr, uint8_t *src,uint8_t u8size){
  uint8_t *dst;
  uint8_t i;
  uint8_t u8ret = 0x00;
  if( (u32addr & 0x07) != 0x00)return 0x01;
  if( (u8size & 0x07) != 0x00)return 0x01;
  dst = (uint8_t *)u32addr;
  for(i=0;i<u8size;i++){
    if(dst[i] != src[i])break;
  }
  if(i>=u8size)return 0x00;
  u8ret = pflash_write(u32addr,src,u8size);
  return u8ret;
}
static void uart0_work_write(void){
  uint32_t u32low,u32high;
  uint8_t u8size;
  uint8_t u8ret;
  u8size = uart0_cmd[3];
  if(u8size != 0x29)return;
  u8size -= (5+4);
  u32low = *((uint32_t *)(uart0_cmd + 4));
  u32high = u32low +  u8size;
  if( (u32low >= 0x7E0000UL)  && (u32high <= 0x7FF000UL)){
      uart0_rewrite_flash(u32low,uart0_cmd + 8);
      u8ret = uart0_work_write_equal(u32low,uart0_cmd + 8,u8size);
     // u8ret = pflash_write(u32low,uart0_cmd + 8,u8size);
      if(u8ret != 0x00)return;
  }
  uart0_write_succeed(0x22,0x00);
}

/*
static void uart0_work_read(void){
  uint32_t u32addr;
  uint8_t u8size;
  uint8_t u8page;
  uint8_t *src;
  uint8_t i;
  uint16_t u16addr;
    u8size = uart0_cmd[8];
    
    u32addr = *((uint32_t *)(uart0_cmd+4));
    if(u32addr < 0x7E0000 || u32addr >= 0x800000){
       uart0_write_succeed(0x22,0x00);
       return;
    }
    u32addr = (u32addr - 0x7E0000UL);
    u8page =  (uint8_t)(u32addr >> 14);
    
    u8page += 0xF8;

    u16addr = ((uint16_t)u32addr) & 0x3FFF;

    
    PPAGE  = u8page;
    
    src = (uint8_t *)(0x8000 + u16addr);
    
    uart0_res_start(0x23,u8size+5);
    for(i=0;i<u8size;i++){
      uart0_res_write_u8(*src++);
    }
    uart0_work_write_crc(); 
    return;
}   */

static void uart0_work_mode(void){
    if(uart0_cmd[4] == 0x01){//boot mode
      uart0_write_succeed(0x24,0x00);
    }else if(uart0_cmd[4] == 0x02){
      uart0_write_succeed(0x24,0x01);
      uart0_reboot(0x0000);
    }
}
void uart0_work(void){
  if(uart0_flag == 0x00)return;
  switch(uart0_cmd[2]){
   case 0x00:
      uart0_reboot(0x00);break;
    case 0x20:
      uart0_work_ver();break;
    case 0x21:
      uart0_work_erase();break;
    case 0x22:
      uart0_work_write();break;
    case 0x24:
      uart0_work_mode();break;
  } 
  uart0_pos = 0x00; 
  uart0_flag = 0;
}
void uart0_res_start(uint8_t u8Cmd,uint8_t u8size){
   uart0_write(0x55);
   uart0_write(0xAA);
   uart0_res_crc = 0xFF; 
   uart0_write(u8Cmd);
   uart0_res_crc += u8Cmd;
   uart0_write(u8size);
   uart0_res_crc += u8size; 
}
void uart0_res_write_u8(uint8_t u8){
  uart0_res_crc += u8;
  uart0_write(u8); 
  
}
void uart0_res_write_u16(uint16_t u16){
  uart0_res_write_u8((uint8_t)u16);
  uart0_res_write_u8((uint8_t)(u16 >> 8));
  
}
void uart0_work_write_crc(void){
   uart0_write(uart0_res_crc) ;
}
void uart0_write(uint8_t u8val){
  while(!SCI0SR1_TDRE);      
    SCI0DRL = u8val;
}
static void uart0_rcv(uint8_t ch){
  if(uart0_flag == 0x01)return;
  uart0_rcv_tick = 13;
  switch(uart0_pos){
    case 0x00:
      if(ch != 0x55)return;
      uart0_cmd[uart0_pos++] = ch;
      break;
    case 0x01:
      uart0_cmd[uart0_pos++] = ch;
      if(ch != 0xAA){uart0_pos = 0x00;}
      uart0_crc = 0xFF;
      break;
    case 0x02:
    case 0x03:
       uart0_crc += ch;
      uart0_cmd[uart0_pos++] = ch;
      break;
    default:
       uart0_cmd[uart0_pos++] = ch;
       if(uart0_cmd[3] == uart0_pos){
            if(uart0_crc == ch)
              uart0_flag = 0x01;
            else
              uart0_pos  = 0x00;
       }else{
          uart0_crc += ch;
       }  
  }
}
void uart0_tick(void){
  if(uart0_flag == 0x01)return;
  if(uart0_rcv_tick == 1){
      uart0_pos = 0x00;
      uart0_rcv_tick = 0x00;
  }else if(uart0_rcv_tick > 1){
    uart0_rcv_tick--;
  }
}

void interrupt  uart0_Vectch0(void){
      if(SCI0SR1_RDRF)
    uart0_rcv(SCI0DRL);      
}
#pragma CODE_SEG DEFAULT





