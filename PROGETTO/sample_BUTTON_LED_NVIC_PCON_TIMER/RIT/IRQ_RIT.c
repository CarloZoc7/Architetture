/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../button_EXINT/button.h"
#include "../timer/timer.h"


#define BLINKING_3s 60 // 3/0.05 = 60 --> 0x03C 60 interrupt i 3 sec 
#define TOT_TIME 144 // totale tempo di spostamento 7.2sec/0.05 = 144
#define TIME_MOVING 0xABA9500// 7.2sec * 25e6 = 18e6 --> 0xABA9500
#define INACTIVITY_TIME 1200 // 60/0.05 = 1200 --> 0x4B0
#define BLINKING_MOVING 0xBEBC20 // 2Hz -> 0.5sec *25e6 = 12.5e6 --> 0xBEBC20
#define BLINKING_ARRIVING 0x4C4B40 // 5Hz -> 0.2sec * 25e6 = 5e6 --> 0x4C4B40

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


int movement = 0;	// indica la direzione in cui si sta muovendo l'ascendore, può assumere tali valori
	/*
		-(-1) ascensore sta scendendo
		-(0) ascendore fermo
		-(1) ascensore sta salendo
	*/
float tot_time_movement = 0; // tempo totale in movimento
int inactivity_joystick = 0; // variable per indicare l'avvio del timer per l'inutilizzo o meno
int arrived = 0; // variabile per il conteggio del led in caso di arrivo 
/*
	- (-1) sono in movimento
	- >0 sono arrivato a destinazione e parte il conteggio del tempo per il blinking del led di 3s
	- 0 
*/
int enable = 0; // variabile per attivare il joystick
volatile int down = 0; // variabile per la pressione dei pulsanti KEY1
volatile int down2 = 0;
volatile int elevator_floor = 0;	// variabile globale che indica dove si trova l'ascensore
	/*
		-(0) floor 0
		-(1) floor 1
		-(2) between the two floors
	*/
// differenza tra elavator floor e movement è che movement da la direzione in cui si muove l'ascensore
int reserved = 0;
/* variabile reserved per la gestione dello stato libero/occupato dell'ascensore, può assumere i seguenti valori:
		- 1 per avere l'ascensore occupato tramite pulsante da uno dei 2 piani
		- 0 libero 
		- 2 occupato e ascensore presente al piano richiesto
*/
int floor_prec = -1; // variabile per gestire il piano di partenza in modo che posso cambiare piano tramite ascensore

void RIT_IRQHandler (void)
{					
	static int selectUP=0;	
	static int selectDOWN=0;
	
	if ((LPC_GPIO1->FIOPIN & (1<<25)) == 0 && reserved == 2 && arrived<=0 ) { // attivo il joystick prima dell'utilizzo, e se è stato risarvato ed è allo stesso piano dell'utente
		enable = 1;
		inactivity_joystick = 0;
		// eventualmente spengo gli ALARM LED
		LED_Off(1); // spengo gli alarm LED 
		LED_Off(3); // spengo gli alarm LED
		
		LED_On(0); // accendo i reserved LED
		LED_On(2);
		
		LED_On(7); // accendo lo STATUS LED 
		if ( floor_prec == -1)
			floor_prec = elevator_floor;
	}
	else if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && arrived <= 0 && enable == 1 && elevator_floor!=0){	
		/* Joystick DOWN */
		/*
		- elevato_floor != 0 in questo modo non posso scnedere più in basso del piano terra
		- enable == 1 che sia attivato il joystick 
		- arrived == 0 che l'ascensore non sia arrivato a destinazione
		*/
		
		selectDOWN++;
		inactivity_joystick = 0;
		elevator_floor = 2; // ascensore in movimento
		
		if(selectDOWN > 0){
				/* here your action */
				init_timer(0, BLINKING_MOVING);
				enable_timer(0);
			
				// accendo i RESERVED LED poiché sono in movimento e l'ascensore è occupato
				LED_On(0);
				LED_On(2);
			
				if (movement == 0 && tot_time_movement == 0){
					movement = -1; // inizio del movimento e sono in discesa.
					tot_time_movement++;
					arrived = -1;
				}
				else if (movement == -1){
					tot_time_movement++;
					arrived = -1;
				}
				else if ( movement == 1){ // cambiamento di direzione
					tot_time_movement--;
					arrived = -1;
				}
				
				if ( movement == -1 && tot_time_movement >= TOT_TIME ){
					// arrivato al piano di destinazione
					//elevator_floor = 0; // destinazione raggiunta
					
					// gestione di STATUS LED all'arrivo dell'ascensore
					// 3 seconds at 5 Hz --> 5 Hz => 0.2sec -> 0.2 * 25e6 = 0x4C4B40 
					init_timer(0, 0x4C4B40); // aggiungo frequenza del LED al al timer 2
					enable_timer(0);
					
					movement = 0;
					tot_time_movement = 0;
					arrived = arrived + 2; // così posso azzerarlo e iniziarlo ad usare come contatore
				} else if(movement == 1 && tot_time_movement <=0 ){
					// cambio direzione, ritorno al piano terra
					movement = 0;
					tot_time_movement = 0;
					elevator_floor = 0;
					arrived = 0;
				}
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && arrived <= 0 && enable == 1 && elevator_floor != 1){	
		/* Joytick UP*/
		/*
		- elevator_floor != 1 in questo modo non posso salire più del piano del primo piano 
		- enable == 1 che sia attivato il joystick 
		- arrived == 0 che l'ascensore non sia arrivato a destinazione
		- reserved == 0 l'ascensore non sia stato riservato da uno dei 2 piani
		*/
		selectUP++;
		inactivity_joystick = 0;
		elevator_floor = 2; // ascensore in movimento
		
		// accendo i RESERVED LED poiché sono in movimento e l'ascensore è occupato
		LED_On(0);
		LED_On(2);
		
		if(selectUP > 0){
				/* here your action */
				init_timer(0, BLINKING_MOVING); // 2Hz -> 0.5s --> 0.5*25*1e6 = 12.5e6 --> 0xBEBC20
				enable_timer(0);
					
				if (movement == 0 && tot_time_movement == 0){
					movement = 1;
					tot_time_movement++;
					arrived = -1;
				}
				else if ( movement == 1){
					tot_time_movement++;
					arrived = -1;
				}
				else if (movement == -1){ // cambio di direzione in movimento
					tot_time_movement--;
					arrived = -1;
				}
				if ( movement == 1 && tot_time_movement >= TOT_TIME){
					// arrivato al piano
					// elevator_floor = 1; arrivato a destinazione

					init_timer(0, BLINKING_ARRIVING);
					enable_timer(0);
			
					movement = 0; // ascensore non più in movimento 
					tot_time_movement = 0; 
					arrived = arrived + 2; // così posso azzerarlo e usarlo come contatore
				} else if ( movement == -1 && tot_time_movement <= 0){
					// ritornato al piano 1
					elevator_floor = 1;
					
					movement = 0; // non più in movimento
					tot_time_movement = 0;
					arrived = 0;
				}
		}
	}
	else{
			selectUP=0;
			selectDOWN=0;
			// stacco il timer per il blinking di status Led
			if (reserved != 1) // se non è stato riservato da qualcuno
				disable_timer(0);
			
			// conteggio per inutilizzo del joystick
			if ( arrived <= 0 && (reserved == 2 || reserved == 0)){
				/*
					- arrived == 0 non sono arrivato a destinazione 
					- reserved  non ho nessuna prenotazione dell'ascensore o è utilizzata dal joystick
					- movement != 0 sono in movimento 
				*/
				inactivity_joystick++;
				if(arrived == -1 && enable == 1) 
					LED_On(7); // accendo lo status LED poichè NON sono arrivato a destinazione e sono fermo tra i due piani 
			}
			
			if( inactivity_joystick >= INACTIVITY_TIME && (reserved == 2 || reserved == 0)){
				if(arrived == -1){ // se non sono arrivato a uno dei due piani accendo l'ALARM LED  
						LED_On(1); // accendo ALARM LED
						LED_On(3); // accendo ALARM LED
				}
				
				disable_timer(2); // spengo il timer per il blinking del led
				disable_timer(0);
				LED_Off(7); // spengo STATUS LED
				LED_Off(0); // spengo RESERVED LED
				LED_Off(2); // spengo RESERVED LED
				
				inactivity_joystick = 0;
				enable = 0; // disattivo il joystick per inattività
				
				if(arrived != -1)
					reserved = 0; // rendo lo stato libero 
			}
	}
	
	if( arrived > 0){
			arrived++;
			
			disable_timer(1); // disattivo il timer 1 
			disable_timer(0); // disattivo il timer 0 per il blinking del timer in movimento
		
			init_timer(2, BLINKING_ARRIVING); // attivo il timer 2 per il blinking in arrivo dell'ascensore
			enable_timer(2);

			if ( arrived >= BLINKING_3s){
				
				if (elevator_floor == 0)
					elevator_floor = 1;
				else if ( elevator_floor == 1)
					elevator_floor = 0;
				else{
					if (floor_prec == 0)
						elevator_floor = 1;
					else 
						elevator_floor = 0;
				}
				floor_prec = -1;
				movement = 0;
				arrived = 0;
				enable = 0; // disattivo il joystick
				 // avvio il lampeggiare nella situazione di arrivo dell'ascensore
				disable_timer(2);
				LED_Off(7); // spengo lo status LED
				// reserved led verranno spenti dopo un minuto che l'ascensore è arrivato al piano
			}
			
	}

// ---> BUTTON MANAGEMENT <---
	/* button management */
		if(down!=0 && arrived <= 0 && reserved == 0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed _ FLOOR 1 - ascensore libero e non in movimento tramite joystick e non riservato*/
			down++;				
			switch(down){
				case 2:
					// here my code
					// accendo i RESERVED LED di entrambi i pannelli
					LED_On(0);
					LED_On(2);
					reserved = 2; // in questa maniera sono tranquillo che posso usare il joystick e sono allo stesso piano dell'ascensore
					if ( elevator_floor != 1){ // se l'ascensore è al piano opposto del 1° piano, va in movimento
							// avvio la procedure per arrivare al piano opposto
							reserved = 1; // finchè non arriva al piano indicato non posso utilizzare il joystick 
							movement = 1; // ascensore in salita dal piano 0 al piano 1
						
							init_timer(1, TIME_MOVING); // tempo di spostamento pari a 7.2sec * 25e6 = 180e6 --> ABA9500
							enable_timer(1);
							
							init_timer(0, BLINKING_MOVING);
							enable_timer(0);
							
					}
					break;
				default:
					break;
			}
		}
		else{ /* button release*/
			down = 0;
			NVIC_EnableIRQ(EINT1_IRQn);	
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	if(down2 != 0 && arrived <= 0 && reserved == 0){
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){ /* KEY2 pressed _ FLOOR 0 - ascensore libero e non in movimento tramite joystick*/
			down2++;				
			switch(down2){
				case 2:
					// here my code
					// accendo i RESERVED LED di entrambi i pannelli
					LED_On(2);
					LED_On(0);
					reserved = 2;
					if (elevator_floor != 0){ // se l'ascensore è al piano opposto al piano terra, avvio la procedura in movimento
					// avvio la procedura del piano sottostante
						
							reserved = 1;
							movement = -1; // ascensore in discesa dato cha sará dal piano 1 al piano 0
						
							init_timer(1, TIME_MOVING); // tempo di spostamento pari a 7.2sec * 25e6 = 180e6 --> ABA9500
							enable_timer(1);
							
							init_timer(0, BLINKING_MOVING);
							enable_timer(0);
					}
					break;
				default:
					break;;
			}
		}
		else {	/* button released */
			down2=0;			
								 /* enable Button interrupts			*/
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
