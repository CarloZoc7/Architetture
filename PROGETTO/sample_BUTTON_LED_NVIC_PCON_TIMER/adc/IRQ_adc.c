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
int last_note1 = 6; // di default sono settate a 440
int last_note2 = 6; // di default sono settate a 440
char text[10];

void ADC_IRQHandler(void) {

	int value;
	char text[12] = "\0";
	int f[8] = {262, 294, 330, 349, 392, 440, 494, 523};
	char note[8] = {'C', 'D', 'E', 'F', 'G', 'A', 'B', 'C'};
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
	value = AD_current*8/0xFFF;
	
	if(select == 1 && last_note1 != value){
			sprintf(text," %d - %c ", f[value], note[value]);
			GUI_Text(80, 100, (uint8_t *) text , Blue, White);
			last_note1 = value;
	} else if(select == 2 && last_note2 != value){
			sprintf(text," %d - %c ", f[value], note[value]);
			GUI_Text(80, 200, (uint8_t *) text , Blue, White);
			last_note2 = value;
	}
}
