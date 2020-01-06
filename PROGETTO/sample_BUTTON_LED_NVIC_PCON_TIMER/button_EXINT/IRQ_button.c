#include "button.h"
#include "lpc17xx.h"

#include "../led/led.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"

extern int down;
extern int down2;
/*
REQUEST PANEL PIANO 0:
	- button --> KEY 1 pin numero 11
	- request led --> LED 2
	- alarm led --> LED 3

REQUEST PANEL PIANO 1:
	- button --> KEY 2 pin numero 12
	- request led --> LED 0
	- alarm led --> LED 1
*/

void EINT1_IRQHandler (void)	  	/* KEY1														 */
{																	/* REQUEST FOR FLOOR 0						 */
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	down=1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{																	/* REQUEST FOR FLOOR 1 						 */
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	down2=1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


