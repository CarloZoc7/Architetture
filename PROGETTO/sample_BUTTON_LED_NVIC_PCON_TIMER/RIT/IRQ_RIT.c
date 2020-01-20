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

#define simulazione 0

#define BLINKING_3s 60 // 3/0.05 = 60 --> 0x03C 60 interrupt i 3 sec 
#define TOT_TIME 144 // totale tempo di spostamento 7.2sec/0.05 = 144
#define INACTIVITY_TIME 1200 // 60/0.05 = 1200 --> 0x4B0
#define TIME_MOVING 0xABA9500// 7.2sec * 25e6 = 18e6 --> 0xABA9500
#define FREQ_SPEAKER 1263// 440--> (1/440*45) *25e6 = 1263

#if simulazione == 0
	#define BLINKING_MOVING 5 // 2Hz -> (0.5/2)/0.05 = 5 
	#define BLINKING_ARRIVING 0x2625A0 // 5Hz -> (0.2/2)sec * 25e6 = 2.5e6 --> 0x2625A0
	#define ALARM_BLINKING 3 // 4hz --> (1/4=0.25 / 2)/0.05= 2.5 all'incirca 3 
#else
	#define BLINKING_MOVING 0x7D0 // 2e3
	#define BLINKING_ARRIVING 0x1388 // 5e3
#endif


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
float arrived = 0; // variabile per il conteggio del led in caso di arrivo 
/*
	- (-1) sono in movimento
	- >0 sono arrivato a destinazione e parte il conteggio del tempo per il blinking del led di 3s
	- 0 
*/
int enable = 0; // variabile per attivare il joystick
volatile int down = 0; // variabile per la pressione dei pulsanti KEY1
volatile int down2 = 0;
volatile int down3 = 0;

int led_blinking = 0;
int led_blinking_alarm = 0;
int alarm_case = 0;
int emergency_case = 0;
int counter_note = 0;

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
		- (-1) situazione di alarm attiva
*/
int floor_prec = -1; // variabile per gestire il piano di partenza in modo che posso cambiare piano tramite ascensore
int freq_notes[2] = {2, 2}; // default settati alla nota A

/* FUNZIONI */
void status_led(void);
void status_led_alarm(void);
void alarm_leds(int);
void loudspeaker(void);


void RIT_IRQHandler (void)
{					
	static int selectUP=0;	
	static int selectDOWN=0;

	
	if ((LPC_GPIO1->FIOPIN & (1<<25)) == 0 && (reserved == 2 || reserved == 0) && arrived<=0 ) { // attivo il joystick prima dell'utilizzo, e se è stato risarvato ed è allo stesso piano dell'utente
		enable = 1;
		inactivity_joystick = 0;
		// eventualmente spengo gli ALARM LED
		alarm_leds(0);
		LED_On(7); // accendo lo status led
	
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
				// blinking status led 
				status_led();
				
				if ( movement == -1 && tot_time_movement >= TOT_TIME ){
					// arrivato al piano di destinazione
					//elevator_floor = 0; // destinazione raggiunta
					
					// gestione di STATUS LED all'arrivo dell'ascensore
					// 3 seconds at 5 Hz --> 5 Hz => 0.2sec -> 0.2 * 25e6 = 0x4C4B40 
					init_timer(0, BLINKING_ARRIVING); // aggiungo frequenza del LED al al timer 2
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
				// blinking dello status led 
				status_led();
				if ( movement == 1 && tot_time_movement >= TOT_TIME){
					// arrivato al piano
					// elevator_floor = 1; arrivato a destinazione

					init_timer(2, BLINKING_ARRIVING);
					enable_timer(2);
			
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
			
			if (alarm_case == 1){
					loudspeaker();
					status_led_alarm();
				}
				else
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
				LED_Off(0); // spengo gli status led 
				LED_Off(2);
				
				if(arrived == -1) // se non sono arrivato a uno dei due piani accendo l'ALARM LED  
					alarm_leds(1);
				
				disable_timer(2); // spengo il timer per il blinking del led
			
				if ( elevator_floor == 2){
					status_led_alarm();
					alarm_case = 1;
					emergency_case = 1;
				}
				inactivity_joystick = 0;
				enable = 0; // disattivo il joystick per inattività
				
				reserved = -1;
				
				if(arrived != -1)
					reserved = 0; // rendo lo stato libero 
			}
	}
	
	if( enable == 0 && reserved == 0 && elevator_floor != 2)
		LED_Off(7);
	
	if( arrived > 0){
			arrived++;
			
			disable_timer(1); // disattivo il timer 1 
		
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
				tot_time_movement = 0;
				arrived = 0;
				enable = 0; // disattivo il joystick
				 // avvio il lampeggiare nella situazione di arrivo dell'ascensore
				disable_timer(2);
				LED_Off(7); // spengo lo status LED
				// reserved led verranno spenti dopo un minuto che l'ascensore è arrivato al piano
			}		
	}
	if (movement != 0 && reserved == 1 && alarm_case == 0)
		status_led(); // blinking status led in caso l'ascensore si muova tramite prenotazione da reserved panel

// ---> BUTTON MANAGEMENT <---
	/* button management */
	if(down!=0 && arrived <= 0 && (reserved == 0 || reserved == -1)){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed _ FLOOR 1 - ascensore libero e non in movimento tramite joystick e non riservato*/
			down++;				
			switch(down){
				case 2:
					// here my code
					// accendo i RESERVED LED di entrambi i pannelli
					LED_On(0);
					LED_On(2);
					disable_timer(0);
					reserved = 2; // in questa maniera sono tranquillo che posso usare il joystick e sono allo stesso piano dell'ascensore
					if ( elevator_floor != 1 && alarm_case == 0){ // se l'ascensore è al piano opposto del 1° piano, va in movimento
							// avvio la procedure per arrivare al piano opposto
							reserved = 1; // finchè non arriva al piano indicato non posso utilizzare il joystick 
							movement = 1; // ascensore in salita dal piano 0 al piano 1
						
							init_timer(1, TIME_MOVING); // tempo di spostamento pari a 7.2sec * 25e6 = 180e6 --> ABA9500
							enable_timer(1);
							
							// accendo lo status led
							status_led();
							
					} else if (elevator_floor == 2){ // situazione di transito tra una parte e l' altra
							reset_timer(0);
							disable_timer(0); // spengo il loudspeaker
	
							status_led();
							// sblocco la situazione di alarm 
							alarm_leds(0);
						
						if( movement == 1 && floor_prec == 1){
							init_timer(1, tot_time_movement*25000000*0.05);
							enable_timer(1);
						} else{
							init_timer(1, (7.2-tot_time_movement*0.05)*25000000);
							enable_timer(1);
						}
							
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
	if(down2 != 0 && arrived <= 0 && (reserved == 0 || reserved == -1)){
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){ /* KEY2 pressed _ FLOOR 0 - ascensore libero e non in movimento tramite joystick*/
			down2++;				
			switch(down2){
				case 2:
					// here my code
					// accendo i RESERVED LED di entrambi i pannelli
					LED_On(2);
					LED_On(0);
					disable_timer(0);
					reserved = 2;
					if (elevator_floor != 0 && alarm_case == 0){ // se l'ascensore è al piano opposto al piano terra, avvio la procedura in movimento
					// avvio la procedura del piano sottostante
						
							reserved = 1;
							movement = -1; // ascensore in discesa dato cha sará dal piano 1 al piano 0
						
							init_timer(1, TIME_MOVING); // tempo di spostamento pari a 7.2sec * 25e6 = 180e6 --> ABA9500
							enable_timer(1);
							
							status_led();
					} else if (elevator_floor == 2){ // ascensore in movimento
						disable_timer(0);
						alarm_leds(0); // spengo la situazione di allarme
						reset_timer(1);
						if( movement == 1 && floor_prec == 0){
							init_timer(1, tot_time_movement*25000000*0.05);
							enable_timer(1);
						} else{
							init_timer(1, (7.2-tot_time_movement*0.05)*25000000);
							enable_timer(1);
						}
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
	
	if(down3 != 0)
	{
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){ // KEY INT0 pressed
			down3++;
			if(down3 == 2){
				if (alarm_case == 1){
						// sblocco la situazione di alarm, altrimenti nulla 
						disable_timer(0); // spengo loudspeaker
						
						alarm_case = 0;
						inactivity_joystick = 0;
						alarm_leds(0);
						
						NVIC_EnableIRQ(EINT0_IRQn);
				}
			} else if(down3>=40){
						emergency_case = 1;
						status_led_alarm();
						enable = 0; // disattivo il joystick per bloccare la posizione dell'ascensore
						alarm_leds(1);
						
						alarm_case = 1;
						reserved = 0;
						
						NVIC_EnableIRQ(EINT0_IRQn);
					}
	}
		else{
			down3 = 0;
			NVIC_EnableIRQ(EINT0_IRQn);
			LPC_PINCON->PINSEL4     |= (1 << 20); 
		}
	}

  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
}

void status_led(){
		// ascendore in movimento
	if( led_blinking == BLINKING_MOVING || led_blinking == 0 )
			LED_Off(7);
	else if( led_blinking == BLINKING_MOVING*2)
			LED_On(7);
	

	if(led_blinking == 10)
		led_blinking = 0;
	led_blinking++;
}

void status_led_alarm(){
	if( led_blinking_alarm == ALARM_BLINKING || led_blinking_alarm == 0)
		LED_Off(7);
	else if(led_blinking_alarm == ALARM_BLINKING*2)
		LED_On(7);

	if(led_blinking_alarm == ALARM_BLINKING*2)
		led_blinking_alarm = 0;
	led_blinking_alarm++;
}

void alarm_leds(int on){
	// spengo gli status led e accendo gli alarm led
	if (on == 1){
		LED_On(1); // accendo gli alarm led
		LED_On(3);
	
		LED_Off(0); // spengo gli status led
		LED_Off(2);
		LED_Off(7);
	}
	else {
		LED_On(0); // accendo gli status led
		LED_On(2);
	
		LED_Off(1); // spengo gli alarm led
		LED_Off(3);
	}
}

void loudspeaker(){
	uint16_t freq_values[8] ={1062, 1125, 1263, 1417, 1592, 1684, 1890, 2120};
	int tmp;
	if(counter_note==10 || counter_note == 0){
		tmp = freq_notes[0];
		reset_timer(0);
		init_timer(0, freq_values[tmp]);
		enable_timer(0);
		counter_note = 1;
	}
	else if(counter_note == 20){
		tmp = freq_notes[1];
		reset_timer(0);
		init_timer(0, freq_values[tmp]);
		enable_timer(0);
		counter_note = 0;
	}
	
	if(counter_note == 20)
		counter_note = 0;
	counter_note++;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
