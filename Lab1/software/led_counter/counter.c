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
#define PORT_OFFSET 0x08
#define SET_OFFSET 0x0c
#define CLR_OFFSET 0x10

// OFFSET Timer interval
#define STATUS_OFFSET 0x00
#define CONTROL_OFFSET 0x04


int counter = 0;

void timer_interrupt (void* context, alt_u32 id) {
	 // increase the counter
	counter++;

	// write counter value on the parallel port
	IOWR_32DIRECT(PARALLELPORT_0_BASE, 8, counter );

	//printf("irq\n");

	// acknowledge IRQ on the timer
	IOWR_16DIRECT(TIMER_0_BASE, STATUS_OFFSET ,0x00);

}

int main(){

	printf("Let s start counting\n");

	IOWR_32DIRECT(PARALLELPORT_0_BASE, DIR_OFFSET ,0xFFFFFFFF);
	//IOWR_32DIRECT(PARALLELPORT_0_BASE,4,0xaaaaaaaa);


	// port
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PORT_OFFSET ,0xaaaaaaaa);
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PORT_OFFSET ,0xaaaaaa00);

	// set
	IOWR_32DIRECT(PARALLELPORT_0_BASE, SET_OFFSET ,0x55555555);

	// clr
	IOWR_32DIRECT(PARALLELPORT_0_BASE, CLR_OFFSET ,0x55555555);

	// link irq to handler
	alt_irq_register(TIMER_0_IRQ,(void*)2,(alt_isr_func)timer_interrupt);
	// start timer and active irq
	IOWR_16DIRECT(TIMER_0_BASE, CONTROL_OFFSET ,0x7);


	while(1)
	{
		printf("counter timer = %d \n", counter);


	}
}




