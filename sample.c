/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: Pong game, Single-Player version.
 * Note(s):
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2022 Politecnico di Torino
 * Author : Luca Tamburo S303907
 * All rights reserved.
 *
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions. */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "adc/adc.h"
#include "GLCD/GLCD.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/

int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL).  */

  BUTTON_init();												/* BUTTON Initialization.              */
	
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec.       	*/
	enable_RIT();													/* RIT enabled.												*/
									
	init_timer(0, 0x000061A8);						/* 25MHz * 0.001s  -> 25000000 * 0.001  ->  25000  -> 0x000061A8 		TIMER DELLO START CONVERSION */
	init_timer(1, 0x000030D4);						/* 25MHz * 0.0005s -> 25000000 * 0.0005 ->  12.500 -> 0x000030D4 		TIMER DELLA PALLINA */
	init_timer(2, 0x000061A8);						/* 25MHz * 0.001s  -> 25000000 * 0.001  ->  25000  -> 0x000061A8 		TIMER IA PADDLE */
	
	LCD_Initialization();									/* LCD Initialization. */
	GUI_Build(0); 												/* GUI Build. */

	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}
