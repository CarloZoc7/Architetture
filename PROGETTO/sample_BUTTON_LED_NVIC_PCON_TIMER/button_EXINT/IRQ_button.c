#include "button.h"
#include "lpc17xx.h"

#include "../led/led.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"

#define MIN_VALUE 0x47868C0	// tempo settato per 3s
#define T_TRAGITTO 0xABA9500 // tempo dei 7.2s per il tragitto 
// 0x59682F00 tempo per 60secondi

//int elevator_floor = 0;	// variabile globale che indica il piano a cui è presente l'ascensore
int reserved =0; // indica lo stato dell'ascensore se è libero o occupato
extern int down;
/*
REQUEST PANEL PIANO 0:
	- button --> KEY 1 pin numero 11
	- request led --> LED 2
	- alarm led --> LED 3

REQUEST PANEL PIANO 1:
	- button --> KEY 2 pin numero 10
	- request led --> LED 0
	- alarm led --> LED 1
*/



void EINT1_IRQHandler (void)	  	/* KEY1														 */
{																	/* REQUEST FOR FLOOR 0						 */
	// accendo i RESERVED LED
	/*
	if ( elevator_floor == 0 && movement == 0 && reserved == 0){ // ascendore allo stesso piano del chiamante
		LED_On(2);// accendo il RESERVED LED del piano corrispondente
		reserved = 1;
	} else if ( movement == 0 && reserved == 0){
		LED_On(2);
		reserved = 1;
		// avvio la procedure per arrivare al piano opposto
		init_timer(1, 0xABA9500); // tempo di spostamento pari a 7.2sec * 25e6 = 180e6 --> ABA9500
		enable_timer(1);
	}
	*/
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	down=1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{																	/* REQUEST FOR FLOOR 1 						 */
	// accendo i RESERVED LED 
	/*
	if ( elevator_floor == 1 && movement == 0 && reserved == 0 ){ // accendo il RESERVED LED del piano corrispondente
		LED_On(0);
		reserved = 1;
	} else if ( movement == 0 && reserved == 0){ // se è al piano opposto
		LED_On(0);
		reserved = 1;
		// avvio la procedura del piano sottostante
		init_timer(1, 0x2FAF080); // tempo di spostamento pari a 7.2sec * 25e6 = 180e6 --> ABA9500
		// momentaneamente metto a  sec --> 0.5 * 25e6 =  --> 1036640
		enable_timer(1);
	}
	*/
	
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	down=1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


