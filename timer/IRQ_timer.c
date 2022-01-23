/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  09-01-2022
** Last Version:        V1.00
** Descriptions:        functions to manage T0, T1 and T2 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <stdio.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h"
#include "../ADC/adc.h"
#include "../RIT/RIT.h"

/******************************************************************************
** Function name:			Timer0_IRQHandler
**
** Descriptions:			Timer/Counter 0 interrupt handler
**
** parameters:				None
** Returned value:		None
**
******************************************************************************/

static double valore_tan_15 = 0.2679;						/* Tangent value at 15 degrees. */
static double valore_tan_45 = 1;								/* Tangent value at 45 degrees */
int primo_tocco = 1;														
static double tan = -1;													

/* Ball variables. */

static uint16_t ball_side = 158;								/* Bottom of the ball */
static uint16_t ball_side_left = 229;						/* Left side of the ball */
static uint16_t ball_side_right = 233;					/* Right side of the ball. */
static int ball = 1;														/* Ball = 1 goes downwards, otherwise it goes upwards. */
static int move_ball = 0;												/* move_ball = 0 the ball goes from right to left, otherwise the opposite. */

/* Player 1 paddle variables. */

extern uint16_t start_paddle;		  							/* Paddle start. */
extern uint16_t stop_paddle;										/* End of paddle. */
extern uint16_t paddle_base;										/* Paddle base. */

/* IA paddle variables. */

uint16_t IA_start_paddle = 100;		  						/* IA_Paddle start. */
uint16_t IA_stop_paddle = 140;									/* IA_End of paddle. */
uint16_t IA_paddle_base = 32;										/* IA_Paddle base. */	
uint16_t IA_new_start_paddle = 100;

/* Border variables. */

uint16_t x_right = 234;													/* Right border. */
uint16_t x_left = 6;														/* Left border. */			

/* Player 1 score variables. */

extern int game_status;																		
extern int current_score;																			 
extern char score[];

static int score_left = 10;										
static int score_right = 25;
static int score_up = 145;
static int score_down = 170;

/* IA 1 score variables. */

extern int IA_current_score;																			 
extern char IA_score[];

static int IA_score_left = 210;										
static int IA_score_right = 230;
static int IA_score_up = 145;
static int IA_score_down = 180;

static int update_element = 0;								
static int choose_element = 0;								

/******************************************************************************
** Function name:			Timer0_IRQHandler
**
** Descriptions:			Timer/Counter 0 interrupt handler
**
** parameters: 				None
** Returned value:		None
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	ADC_start_conversion();
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
** Function name:			Timer1_IRQHandler
**
** Descriptions:			Timer/Counter 1 interrupt handler
**
** parameters: 				None
** Returned value:		None
******************************************************************************/

void TIMER1_IRQHandler (void)	
{  
	int i = 0;
	
	/* Variables used to calculate the angle after the ball hits the paddle. */

	double relative_intersect_x;
	double normalized_relative_intersect_x;
	
	/* New ball value variables. */
	
	uint16_t new_ball_side_left;																		/* New value of the left side of the ball. */
	uint16_t new_ball_side_right;																		/* New value of the right side of the ball. */
	uint16_t new_ball_side;																					/* New ball base value. */

	/* Initial case, i.e. at the beginning of the game. */
	
	if(primo_tocco == 1){
		ball = 1;
		move_ball = 0;
		tan = -valore_tan_45;
		
		ball_side = 158;
		ball_side_left = 228;
		ball_side_right = 233;
	}

	/* Post game start case. */
	
	if(!primo_tocco){
		
		/* Ball touches the border. */
		
		/* Ball goes down and touches the left wall. */
		if(ball && ball_side_left <= (x_left + 2)){														
			move_ball = 1;
			if(tan < 0){																									
				tan = -tan;
			}
		/* Ball goes down and touches the right wall. */
		} else if (ball && ball_side_right >= (x_right - 2)){ 								
			move_ball = 0;
			if(tan > 0){
				tan = -tan;
			}
		/* Ball goes up and touches the right side of the board. */
		} else if (!ball && ball_side_right >= (x_right - 2)){ 			
			move_ball = 0;
			if(tan < 0){
				tan = -tan;
			}
		/* Ball goes up and touches the left side of the board. */
		} else if (!ball && ball_side_left <= (x_left + 2)){ 					
			move_ball = 1;
			if(tan > 0){
				tan = -tan;
			}
			
		/* Ball touches player 1 paddle. */
			
		} else if (ball && (ball_side + 5) == paddle_base && ball_side_left >= start_paddle && ball_side_right <= stop_paddle) {
			/* The direction of the ball is decided when the ball hits the paddle. */
			ball = 0;
			/* Relative position of the ball. Indicates the point where the paddle and ball meet.*/
			relative_intersect_x = (start_paddle + (stop_paddle - start_paddle)/2) - (ball_side_left);	
			normalized_relative_intersect_x = (relative_intersect_x/(stop_paddle - start_paddle)/2);
			/* Bounce angle takes values from -45 to 45 degrees. */
			tan = normalized_relative_intersect_x * valore_tan_45;
			/* If the bounce angle is less than zero, the direction tends to be horizontal, otherwise it will be 15 degrees. */
			if(tan > -0.25 && tan <= 0){ 																	
				tan = - valore_tan_15;
			} else if(tan < 0.25 && tan >= 0){ 
				tan = valore_tan_15;
			}	
			if(tan < 0){
				move_ball = 1;
			} else {
				move_ball = 0;
			}

			/* Ball touches IA paddle. */
			
		} else if (!ball && (ball_side - 5) == (IA_paddle_base + 10) && ball_side_left >= IA_start_paddle && ball_side_right <= IA_stop_paddle) {
			/* Relative position of the ball. Indicates the point where the paddle and ball meet.*/
			relative_intersect_x = (IA_start_paddle + (IA_stop_paddle - IA_start_paddle)/2) - (ball_side_left);	
			normalized_relative_intersect_x = (relative_intersect_x/(IA_stop_paddle - IA_start_paddle)/2);
			/* Bounce angle takes values from -45 to 45 degrees. */
			tan = normalized_relative_intersect_x * valore_tan_45;
			/* If the bounce angle is less than zero, the direction tends to be horizontal, otherwise it will be 15 degrees. */
			if(tan > -0.25 && tan <= 0){ 																	
				tan = - valore_tan_15;
			} else if(tan < 0.25 && tan >= 0){ 
				tan = valore_tan_15;
			}	
			if(tan < 0){
				move_ball = 0;
			} else {
				move_ball = 1;
			}

			ball = 1;
			
		} else if(ball_side > paddle_base){
				for(i = 0; i < 5; i++){
					LCD_DrawLine(ball_side_left, ball_side, ball_side_right, ball_side, Black);
					ball_side++;
				}
				ball_side = 158;
				ball_side_left = 228;
				ball_side_right = 233;
				
				ball = 1;
				move_ball = 0;
				tan = -valore_tan_45;
				primo_tocco = 1;
				
				IA_current_score += 1;
				sprintf(IA_score, "%d", IA_current_score);
				GUI_TextReverse(230, 173, (uint8_t *) IA_score, White, Black);
				
				if(IA_current_score == 5){
					game_status = 3;
					GUI_Build(3);
					NVIC_DisableIRQ(TIMER0_IRQn);
					NVIC_DisableIRQ(TIMER1_IRQn);
					NVIC_DisableIRQ(TIMER2_IRQn);
					return;
				}
			} else if (ball_side < IA_paddle_base){
				for(i = 0; i < 5; i++){
					LCD_DrawLine(ball_side_left, ball_side, ball_side_right, ball_side, Black);
					ball_side++;
				}
				ball_side = 158;
				ball_side_left = 228;
				ball_side_right = 233;
				
				ball = 1;
				move_ball = 0;
				tan = -valore_tan_45;
				primo_tocco = 1;
							
				current_score += 1;
				sprintf(score, "%d", current_score);
				GUI_Text(10, 160, (uint8_t *) score, White, Black);

				if(current_score == 5){
					game_status = 3;
					GUI_Build(3);
					NVIC_DisableIRQ(TIMER0_IRQn);
					NVIC_DisableIRQ(TIMER1_IRQn);
					NVIC_DisableIRQ(TIMER2_IRQn);
					return;
			}
		}
	}

	/*If the ball drops increase the side value, otherwise decrease it. */
	if(ball == 1){																							
		new_ball_side = ball_side + 1;
	} else {
		new_ball_side = ball_side - 1;
	}
	
	/* Angular coefficient */
	new_ball_side_left = (new_ball_side + (tan * ball_side_left) - ball_side) / tan;
	new_ball_side_right = new_ball_side_left + 4;
	
	/* Case where the ball passes over the core and clears it. */
	if(!update_element && new_ball_side_left >= score_left && new_ball_side_right <= score_right && new_ball_side <= score_down && (new_ball_side - 5) >= score_up){
		choose_element = 1;
		update_element = 1;
	} else if (choose_element == 1 && update_element == 2 && (new_ball_side_right > score_right || (new_ball_side > score_down || (new_ball_side - 5) < score_up))){
		update_element = 3;
	}
	
	if(!update_element && new_ball_side_left >= IA_score_left && new_ball_side_right <= IA_score_right && new_ball_side <= IA_score_down && (new_ball_side - 5) >= IA_score_up){
		choose_element = 2;
		update_element = 1;
	} else if (choose_element == 2 && update_element == 2 && (new_ball_side_right > IA_score_right || (new_ball_side > IA_score_down || (new_ball_side - 5) < IA_score_up))){
		update_element = 3;
	}
	
	if(update_element == 1){
		switch(choose_element){
			case 1:
				GUI_Text(10, 160, (uint8_t *) score, Black, Black);
				break;
			case 2:
				GUI_TextReverse(230, 173, (uint8_t *) IA_score, Black, Black);
				break;
			default:
				break;
		}
		update_element = 2;
	} else if(update_element == 3){
			switch(choose_element){
				case 1:
					GUI_Text(10, 160, (uint8_t *) score, White, Black);
					break;
				case 2:
					GUI_TextReverse(230, 173, (uint8_t *) IA_score, White, Black);
					break;
				default:
					break;
			}
		update_element = 0;
	}
	
	for(i = 0; i < 5; i++){
		LCD_DrawLine(ball_side_left, ball_side, ball_side_right, ball_side, Black);
		ball_side++;
	}
	
	for(i = 0; i < 5; i++){
		LCD_DrawLine(new_ball_side_left, new_ball_side, new_ball_side_right, new_ball_side, Green);
		new_ball_side++;
	}
	
	ball_side_left = new_ball_side_left;
	ball_side_right = new_ball_side_right;
	ball_side = new_ball_side - 5;

	if(primo_tocco){
		primo_tocco = 0;
	}
		
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:			Timer2_IRQHandler
**
** Descriptions:			Timer/Counter 2 interrupt handler
**
** parameters: 				None
** Returned value:		None
******************************************************************************/

void TIMER2_IRQHandler (void)
{	
	int i = 0;
	/* Automatic paddle movement. */
	
	if(move_ball == 1 && (IA_start_paddle + 5) < 194){
		IA_new_start_paddle = IA_start_paddle + 5;
		
		for(i = 0; i < 10; i++){																					
			LCD_DrawLine(IA_start_paddle, IA_paddle_base, IA_new_start_paddle , IA_paddle_base, Black);					
			IA_paddle_base++;
		}
		IA_paddle_base = 32;
		
		for(i = 0; i < 10; i++){																					
			LCD_DrawLine(IA_stop_paddle, IA_paddle_base, IA_new_start_paddle + 40, IA_paddle_base, Green);					
			IA_paddle_base++;
		}
	
		IA_start_paddle = IA_new_start_paddle;
		IA_stop_paddle = IA_new_start_paddle + 40;
		IA_paddle_base = 32;
	} else if(!move_ball && (IA_start_paddle + 5) > 55){
		IA_new_start_paddle = IA_start_paddle - 5;
		
		for(i = 0; i < 10; i++){																					
			LCD_DrawLine(IA_new_start_paddle + 40, IA_paddle_base, IA_stop_paddle, IA_paddle_base, Black);					
			IA_paddle_base++;
		}
		IA_paddle_base = 32;
		
		for(i = 0; i < 10; i++){																					
			LCD_DrawLine(IA_new_start_paddle, IA_paddle_base, IA_start_paddle, IA_paddle_base, Green);					
			IA_paddle_base++;
		}
	
		IA_start_paddle = IA_new_start_paddle;
		IA_stop_paddle = IA_new_start_paddle + 40;
		IA_paddle_base = 32;
	}
	
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
** Function name:			Timer3_IRQHandler
**
** Descriptions:			Timer/Counter 3 interrupt handler
**
** parameters: 				None
** Returned value:		None
******************************************************************************/

void TIMER3_IRQHandler (void)
{	
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
