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

int i = 0;
extern int elevator_floor;
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


void TIMER0_IRQHandler (void) // timer usato per il blinking del led alle varie frequenze
{	
	if(i%2==0){
		LED_On(7);
	}
	else{
		LED_Off(7);
	}
	i++;
	if( i >= 1000)
		i = 0;
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
	
	reserved = 0; // libero lo stato di occupato
	inactivity_joystick = 0; // resetto il timer dato che riparte dallo stato precendete in partenza
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
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
