/*
 * counter.c
 *
 *  Created on: Sep 23, 2020
 *      Author: quartus
 */

#include "io.h"
#include <stdio.h>
#include "system.h"

int main(){

	printf("Let s start counting\n");
	IOWR_8DIRECT(LEDS_BASE, 0, 0);
	int counter = 0;

	while(1)
	{
		counter++;
		printf("counter = %d\n", counter);
		IOWR_8DIRECT(LEDS_BASE, 0, counter);

	}
}




