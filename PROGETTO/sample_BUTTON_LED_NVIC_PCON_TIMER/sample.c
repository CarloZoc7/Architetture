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
#include "dac/dac.h"
#include "adc/adc.h"
#include "TouchPanel/TouchPanel.h"
#include "GLCD/GLCD.h"

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
	DAC_init();
	ADC_init();
	LCD_Initialization();
		TP_Init();
		TouchPanel_Calibrate();
		LCD_Clear(Black);
		
	// ricorda grandezza dello schermo pari a: 320x240
		GUI_Text(200, 50, (uint8_t *) " ON ", White, Black);

		init_timer(3, 0x4E2);  /* 500us * 25MHz = 1.25*10^3 = 0x4E2 */
		enable_timer(3);
		
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= 0xFFFFFFFFD;						
	
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}
