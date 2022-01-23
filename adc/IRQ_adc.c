/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;    																		/* Last converted value.	*/

/* Player 1 paddle variables. */

uint16_t start_paddle = 100;																					/* Paddle start. */
uint16_t stop_paddle = 140;																						/* Paddle end. */
uint16_t paddle_base = 278;																						/* Paddle base. */																			

int check_adc = 0;

void ADC_IRQHandler(void) {
  
	int i = 0;
	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);													/* Read Conversion Result. */  
		
	if(check_adc == 0){
		check_adc++;
	} else if (check_adc == 1){
		uint16_t new_start_paddle = AD_current*190/0xFFF;	
		/* Player 1 paddle creation. */
		if(new_start_paddle < 5){
			new_start_paddle = 5;
		}
		for(i = 0; i < 10; i++){																					
			LCD_DrawLine(new_start_paddle, paddle_base, new_start_paddle + 40, paddle_base, Green);					
			paddle_base++;
		}
		check_adc++;
		start_paddle = new_start_paddle;
		stop_paddle = new_start_paddle + 40;
		paddle_base = 278;
		
	} else if(check_adc == 2 && AD_current != AD_last && ((AD_last - AD_current > 75 && AD_last - AD_current < 260) || (AD_last - AD_current < -75 && AD_last - AD_current > -260))){
		uint16_t new_start_paddle = AD_current*190/0xFFF;	
		if(new_start_paddle > 5){
			for(i = 0; i < 10; i++){
				/* Moving the player 1 paddle to the right. */
				if(new_start_paddle > start_paddle){ 																									
					LCD_DrawLine(start_paddle, paddle_base, new_start_paddle, paddle_base, Black);	
					LCD_DrawLine(stop_paddle, paddle_base, new_start_paddle + 40, paddle_base, Green);
				} else { 																												
				/* Moving the player 1 paddle to the left. */
					LCD_DrawLine(new_start_paddle + 40, paddle_base, stop_paddle, paddle_base, Black);	
					LCD_DrawLine(new_start_paddle, paddle_base, start_paddle, paddle_base, Green);
				}
					paddle_base++;
				}	
			start_paddle = new_start_paddle;
			stop_paddle = new_start_paddle + 40;
			paddle_base = 278;
		}
	}
			AD_last = AD_current;
}
