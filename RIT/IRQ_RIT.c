/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  10-01-2022
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <stdio.h>
#include "lpc17xx.h"
#include "RIT.h"
#include "../GLCD/GLCD.h"
#include "../button_EXINT/button.h"
#include "../ADC/adc.h"
#include "../timer/timer.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
******************************************************************************/

int down_INT0 = 0;
int down_KEY1 = 0;
int down_KEY2 = 0;

int game_status = 0;

extern int check_adc;
extern int primo_tocco;
extern int current_score;
extern char score[];
extern int IA_current_score;
extern char IA_score[];

void RIT_IRQHandler (void)
{						
	/* KEY1 */
	if(down_KEY1 != 0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){											/* KEY1 pressed. */
			down_KEY1++;				
			switch(down_KEY1){
				case 2:
						if(game_status == 0){
							game_status = 1;																	/* Start. */
							GUI_Build(game_status);
							ADC_init();
							enable_timer(0);
							enable_timer(1);		
							enable_timer(2);
							NVIC_EnableIRQ(TIMER0_IRQn); 
							NVIC_EnableIRQ(TIMER1_IRQn);
							NVIC_EnableIRQ(TIMER2_IRQn);
						}
					break;
				default:
					break;
			}
		}
		else {																											/* Button released. */
			down_KEY1 = 0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 									/* Enable Button interrupts.	*/
			LPC_PINCON->PINSEL4 |= (1 << 22);     										/* External interrupt 0 pin selection. */
		}
	}
		
	/* KEY2 */
	if(down_KEY2 != 0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){											/* KEY2 pressed. */
			down_KEY2++;				
			switch(down_KEY2){
				case 2:
						if(game_status == 1){
							game_status = 2;																	/* Pause. */
							GUI_Build(game_status);
							NVIC_DisableIRQ(TIMER0_IRQn); 										
							NVIC_DisableIRQ(TIMER1_IRQn);
							NVIC_DisableIRQ(TIMER2_IRQn);
						} else if (game_status == 2){												/* Resume. */
							game_status = 1;	
							GUI_Build(game_status);
							check_adc = 0;
							NVIC_EnableIRQ(TIMER0_IRQn); 
							NVIC_EnableIRQ(TIMER1_IRQn);
							NVIC_EnableIRQ(TIMER2_IRQn);							
						}
					break;
				default:
					break;
			}
		}
		else {																											/* Button released. */
			down_KEY2 = 0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 									/* Enable Button interrupts. */
			LPC_PINCON->PINSEL4 |= (1 << 24);     										/* External interrupt 0 pin selection. */
		}
	}
		
	/* INT0 */
	if(down_INT0 != 0){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){											/* INT0 pressed. */
			down_INT0++;				
			switch(down_INT0){
				case 2:																									/* You Lose. */
					current_score = 0;
					sprintf(score, "%d", current_score);
					IA_current_score = 0;
					sprintf(IA_score, "%d", IA_current_score);						
					game_status = 0;
					GUI_Build(game_status);
					check_adc = 0;
					primo_tocco = 1;
					NVIC_DisableIRQ(TIMER0_IRQn);
					NVIC_DisableIRQ(TIMER1_IRQn);
					NVIC_DisableIRQ(TIMER2_IRQn);
					break;
				default:
					break;
			}
		}
		else {																											/* Button released .*/
			down_INT0 = 0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 									/* Enable Button interrupts.		*/
			LPC_PINCON->PINSEL4 |= (1 << 20);     										/* External interrupt 0 pin selection. */
		}
	}
  LPC_RIT->RICTRL |= 0x1;																				/* Clear interrupt flag. */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
