/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../led/led.h"
#include "../RIT/RIT.h"
#include "../button_EXINT/button.h"
#include "../dac/dac.h"
#include "../adc/adc.h"
#include "../TouchPanel/TouchPanel.h"
#include "../GLCD/GLCD.h"
#include "stdio.h"

int i = 0;
extern int elevator_floor;
int f[8] = {523, 494, 440, 392, 349, 330, 294, 262};
char note[8] = {'C', 'B', 'A', 'G', 'F', 'E', 'D', 'C'};
char text_note1[10] = "\0";
char text_note2[10] = "\0";
int enable_screen = 0;
int note1 = 2;
int note2 = 2;
int select =0;

uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


void TIMER0_IRQHandler (void) // timer usato per il loudspaker
{	static int ticks = 0;
	int value30 = SinTable[ticks]*0.30; // considero il 30%
	DAC_convert(value30<<6);
	ticks++;
	if(ticks==45) ticks=0;
	LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER1_IRQHandler (void) // timer usato per il tragitto in caso di prenotazione da parte dell'utente
{	
	// setto arrived = 1 ARRIVED  per il blinking del led in stato di arrivo, in questa maniera dovrei attivare anche il time
	arrived = 1;
	LED_Off(7);
	reserved = 0; // libero lo stato di occupato
	inactivity_joystick = 0; // resetto il timer dato che riparte dallo stato precendete in partenza
	alarm_case = 0;
  LPC_TIM1->IR = 1;			/* clear interrupt flag */ 
  return;
}

void TIMER2_IRQHandler (void){ // timer utilizzato per il blinking in arrivo
  
	if(i%2==0){
		LED_On(7);
	}
	else{
		LED_Off(7);
	}
	i++;
	if( i >= 1000)
		i = 0;
	
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler(void){
	
	getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;
	
	sprintf(text_note1, " %d - %c ", f[note1], note[note1]);
	sprintf(text_note2, " %d - %c ", f[note2], note[note2]);
	
	if(display.x <=220 && display.x >=180 && display.y <=70 && display.y >=30){
		LCD_Clear(Blue);
		enable_screen = 1;
		GUI_Text(80, 20, (uint8_t *) "Maintenance", Yellow, Blue);
		GUI_Text(80, 60, (uint8_t *) " Select note 1 ", White, Blue);
		GUI_Text(80, 100, (uint8_t *) text_note1, White, Blue);
		
		GUI_Text(80, 120, (uint8_t *) "____________", White, Blue);
		
		GUI_Text(80, 160, (uint8_t *) " Select note 2 ", White, Blue);
		GUI_Text(80, 200, (uint8_t *) text_note2, White, Blue);
		
	
		GUI_Text(60, 280, (uint8_t *) " Save ", White, Green);
		GUI_Text(180, 280, (uint8_t *)" quit ", White, Red);
	}
	
	// selezione zone di note
	if(display.x < 200 && display.x >=70 && enable_screen == 1){
		if(display.y < 130 && display.y >=10){
			GUI_Text(80, 60, (uint8_t *) " Select note 1 ", Blue, White);
			GUI_Text(80, 100, (uint8_t *) text_note1, Blue, White);
			GUI_Text(80, 160, (uint8_t *) " Select note 2 ", White, Blue);
			GUI_Text(80, 200, (uint8_t *) text_note2, White, Blue);
			select = 1;
		}
		else if(display.y<290 && display.y>=130){				
			GUI_Text(80, 60, (uint8_t *) " Select note 1 ", White, Blue);
			GUI_Text(80, 100, (uint8_t *) text_note1, White, Blue);
			GUI_Text(80, 160, (uint8_t *) " Select note 2 ", Blue, White);
			GUI_Text(80, 200, (uint8_t *) text_note2, Blue, White);
			select = 2;
		}
	}
	
	// selezione tasti 
	if(display.y>=240 && display.y<=290 && enable_screen == 1){
		if(display.x >=50 && display.x <90){ // tasto save
			freq_notes[0] = note1;
			freq_notes[1] = note2;
			GUI_Text(80, 60, (uint8_t *) " Select note 1 ", White, Blue);
			GUI_Text(80, 100, (uint8_t *) text_note1, White, Blue);
		
			GUI_Text(80, 120, (uint8_t *) "____________", White, Blue);
		
			GUI_Text(80, 160, (uint8_t *) " Select note 2 ", White, Blue);
			GUI_Text(80, 200, (uint8_t *) text_note2, White, Blue);
			sprintf(text_note1, " %d - %c ", f[note1], note[note1]);
			sprintf(text_note2, " %d - %c ", f[note2], note[note2]);
			// salvo ed esco 
		}
		else if(display.x >= 180 && display.x <=200){ // tasto quit
			enable_screen = 0;
			select = 0;
			LCD_Clear(Black);
			GUI_Text(200, 50, (uint8_t *) " ON ", White, Black);
		}
	}
	ADC_start_conversion();
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
