/*
 * counter.c
 *
 *  Created on: Sep 23, 2020
 *      Author: quartus
 */

#include "io.h"
#include <stdio.h>
#include "system.h"
#include "altera_avalon_timer.h"
#include "alt_types.h"
#include "sys/alt_irq.h"


/*void handle_timer_interrupt(void*p, alt_u32 param)
{
   // clear irq status in order to prevent retriggering
   //IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);


}*/

// OFFSET Parallel port
#define DIR_OFFSET 0x00
#define PORT_OFFSET 0x02
#define SET_OFFSET 0x04
#define CLR_OFFSET 0x06

// OFFSET Timer interval
#define STATUS_OFFSET 0x00
#define CONTROL_OFFSET 0x02

#define START 0x04
#define IRQ_ENABLE 0x01

int counter = 0;

void timer_interrupt (void* context, alt_u32 id) {
	 // increase the counter
	counter++;

	// write counter value on the parallel port
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PORT_OFFSET, counter );

	// acknowledge IRQ on the timer
	IOWR_16DIRECT(TIMER_0_BASE,STATUS_OFFSET,0x1);

}

int main(){

	alt_irq_register(TIMER_0_IRQ,(void*)2,(alt_isr_func)timer_interrupt);

	// Read controle state
	short control = IORD_32DIRECT(TIMER_0_BASE,CONTROL_OFFSET);

	// Start timer
	IOWR_16DIRECT(TIMER_0_BASE, CONTROL_OFFSET, control | START );

	// Enable IRQs
	IOWR_16DIRECT(TIMER_0_BASE,CONTROL_OFFSET, control | IRQ_ENABLE );

	// Direction en output
	IOWR_32DIRECT(PARALLELPORT_0_BASE, DIR_OFFSET, 0xFFFFFFFF );

	printf("Let s start counting\n");

	while(1)
	{

		printf("counter = %d \n", counter);

	}
}




