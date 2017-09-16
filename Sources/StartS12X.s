                 ;********************************************************************************
;* Freescale Semiconductor Inc.
;* (c) Copyright 2004-2005 Freescale Semiconductor, Inc.
;* ALL RIGHTS RESERVED.
;********************************************************************************
;Services performed by FREESCALE in this matter are performed AS IS and without 
;any warranty. CUSTOMER retains the final decision relative to the total design 
;and functionality of the end product. FREESCALE neither guarantees nor will be 
;held liable by CUSTOMER for the success of this project.
;FREESCALE DISCLAIMS ALL WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY INCLUDING, 
;BUT NOT LIMITED TO, IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR 
;A PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE OR ADVISE SUPPLIED TO THE PROJECT
;BY FREESCALE, AND OR NAY PRODUCT RESULTING FROM FREESCALE SERVICES. IN NO EVENT
;SHALL FREESCALE BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
;THIS AGREEMENT.
;
;CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or 
;actions by anyone on account of any damage, or injury, whether commercial, 
;contractual, or tortuous, rising directly or indirectly as a result of an advise
;or assistance supplied CUSTOMER in connection with product, services or goods 
;supplied under this Agreement.
;********************************************************************************
;* File      StartS12X.s
;* Owner     b01802
;* Version   1.0   
;* Date      Dec-02-2010
;* Classification   General Business Information
;* Brief     Startup file
;********************************************************************************
;* Detailed Description:
;*
;*    Tested on: DEMO9S12XSFAME, EVB9S12XEP100
;*    
;*    The bootloder handles all reset vectors. This startup file makes a decision
;*    whether the bootloader or user application is executed. 
;********************************************************************************
;Revision History:
;Version   Date          Author    Description of Changes
;1.0       Dec-02-2010  b01802    Initial version
;********************************************************************************
;
  xref  main
;
  xdef  _BootStart
  xref  start_user_app
;
 _BootStart:
       LDD   $2000
       CPD   #$55AA
       BEQ   _BootApp
       lds   #$2200
       JSR  start_user_app
 _BootApp:
       lds   #$2200
       JSR   main 
      