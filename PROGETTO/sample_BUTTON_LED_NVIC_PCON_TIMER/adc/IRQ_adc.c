/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "stdio.h"
#include "adc.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../TouchPanel/TouchPanel.h"
#include "../GLCD/GLCD.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

int value;
char text[10];

void ADC_IRQHandler(void) {

	int value;
	char text[12] = "\0";
	int f[8] = {523, 494, 440, 392, 349, 330, 294, 262};
	char note[8] = {'C', 'B', 'A', 'G', 'F', 'E', 'D', 'C'};
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
	value = AD_current/0x200;
	
	if(select == 1 && note1 != value){
			sprintf(text," %d - %c ", f[value], note[value]);
			GUI_Text(80, 100, (uint8_t *) text , Blue, White);
			note1 = value;
	} else if(select == 2 && note2 != value){
			sprintf(text," %d - %c ", f[value], note[value]);
			GUI_Text(80, 200, (uint8_t *) text , Blue, White);
			note2 = value;
	}
}
