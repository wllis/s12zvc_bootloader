#include "dev.h"

static uint8_t uart0_flag;
static uint8_t uart0_rcv_tick;
static uint8_t uart0_cmd[120];
static uint8_t uart0_pos;
static uint8_t uart0_crc;




void uart0_init(void){
  uart0_flag = 0x00;
  uart0_rcv_tick = 0x00;
  SCI0BD = BUS_CLOCK/16/9600;   //
  SCI0CR1 = 0x00;       //
  SCI0CR2 = 0x2C;       //
}
static void uart0_switch_mode(uint8_t u8mode){
  switch(u8mode){
    case 0x00:
        start_user_app();
        break;
    case 0x01:
        sys_start_app(0xFF00);
        break;
  }
}
void uart0_app_command(uint8_t *src,uint8_t size){
  //size = size;
     switch(src[2]){
      case 0x01:
        uart0_switch_mode(src[3]);
        break;
     }
}
static uint8_t uart0_res_crc;

static void uart0_work_ver(void){
   uart0_res_start(0x20,0x08);
   uart0_res_write_u8(0x00);
   uart0_res_write_u16(0x00);
   uart0_work_write_crc();
}
static void uart0_reboot(void){
  COPCTL = 0x01;        //enable watchdog
  ARMCOP = 0x00;
}
static void uart0_write_succeed(uint8_t u8cmd,uint8_t u8Ret){
    uart0_res_start(u8cmd,0x06);
  uart0_res_write_u8(u8Ret);
  uart0_work_write_crc();
}
void uart0_work(void){
  if(uart0_flag == 0x00)return;
  switch(uart0_cmd[2]){
   case 0x00:
      uart0_reboot();break;
    case 0x20:
      uart0_work_ver(); break;
    case 0x21:
    //  uart0_work_erase();break;
    case 0x22:
    //  uart0_work_write();break;
    case 0x23:
     // uart0_work_read();break;
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
  uart0_rcv_tick = 33;
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


