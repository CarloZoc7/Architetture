/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        Atomic joystick init functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "joystick.h"

/*----------------------------------------------------------------------------
  Function that initializes joysticks and switch them off
 *----------------------------------------------------------------------------*/
 
/*
	pin da attivare per UP e DOWN
	UP 1.26 --> DOWN
	DOWN 1.29 --> UP
*/

void joystick_init(void){
	
	LPC_PINCON->PINSEL3 &= ~(3<<18);	// SELECT
	LPC_GPIO1->FIODIR   &= ~(1<<25);	
	
	LPC_PINCON->PINSEL3 &= ~(3<<20);	// DOWN
	LPC_GPIO1->FIODIR   &= ~(1<<26);	
  
	LPC_PINCON->PINSEL3 &= ~(3<<26);
	LPC_GPIO1->FIODIR   &= ~(1<<29);	// UP 
  
}
