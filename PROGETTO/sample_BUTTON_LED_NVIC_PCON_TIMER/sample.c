/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through EINT buttons
 * Note(s):
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "joystick/joystick.h"
#include "RIT/RIT.h"

/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in funct_led								*/

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	

	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* JOYSTICK Initialization						*/

	// impostato a 50ms --> 50e-3 * 100e6 = 5e6 --> 0x004C4B40
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= 0xFFFFFFFFD;						
	
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}
