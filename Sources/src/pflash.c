#include "dev.h"

#define FLASH_SECTOR_SIZE   0x400

/******************************************************************************/
#pragma CODE_SEG RAM_CODE
static uint8_t pflash_write_block(uint32_t address, uint16_t *ptr){
  uint8_t i;
  
  while((FSTAT & FSTAT_CCIF_MASK) == 0);  //wait if command in progress
  FSTAT = 0x30;       //clear ACCERR and PVIOL
  
  FCCOBIX = 0x00;
  FCCOB = 0x0600 | ((address & 0x007F0000)>>16);
  
  FCCOBIX = 0x01;
  FCCOB = (address & 0x0000FFFF);
  
  for(i=2; i<6; i++)  //fill data (4 words) to FCCOB register
  {
    FCCOBIX = i;
    FCCOB = *ptr;
    ptr++;
  }
  
  FSTAT = 0x80;       //launch command
  while((FSTAT & FSTAT_CCIF_MASK) == 0);  //wait for done
  
  if((FSTAT & (FSTAT_ACCERR_MASK | FSTAT_FPVIOL_MASK)) != 0)
    return 0x01;
  else
    return 0x00;
}
uint8_t pflash_write(uint32_t u32addr, uint8_t *src,uint8_t u8size){
  uint8_t u8ret = 0x00;
  uint8_t u8left = u8size;
  if( (u32addr & 0x07) != 0x00)return 0x01;
  if( (u8size & 0x07) != 0x00)return 0x01;
  while(u8left >= 8){
      u8ret = pflash_write_block(u32addr,(uint16_t *)src);
      if(u8ret)return u8ret;
      src += 8;
      u8left -= 8;
      u32addr += 8;
  }
  return 0x00;
}
#pragma CODE_SEG DEFAULT

/******************************************************************************/
#pragma CODE_SEG RAM_CODE
uint8_t  pflash_erase(uint32_t u32low,uint32_t u32high){
  while(u32low < u32high){
    while((FSTAT & FSTAT_CCIF_MASK) == 0);  //wait if command in progress
    FSTAT = 0x30;       //clear ACCERR and PVIOL
    
    FCCOBIX = 0x00;
    FCCOB = 0x0A00 | ((u32low & 0x007F0000)>>16);
    
    FCCOBIX = 0x01;
    FCCOB = (u32low & 0x0000FFF8);
    
    FSTAT = 0x80;       //launch command
    while((FSTAT & FSTAT_CCIF_MASK) == 0);  //wait for done
    
    if((FSTAT & (FSTAT_ACCERR_MASK | FSTAT_FPVIOL_MASK)) != 0)
      return 0x01;

    u32low += FLASH_SECTOR_SIZE;
  }
 
}
#pragma CODE_SEG DEFAULT
