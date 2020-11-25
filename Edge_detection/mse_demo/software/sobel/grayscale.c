/*
 * grayscale.c
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <io.h>
#include <system.h>

unsigned char *grayscale_array;
int grayscale_width = 0;
int grayscape_height = 0;

void conv_grayscale(void *picture,
		            int width,
		            int height) {
	int x,y,gray;
	unsigned short *pixels = (unsigned short *)picture , rgb;
	grayscale_width = width;
	grayscape_height = height;
	if (grayscale_array != NULL)
		free(grayscale_array);
	grayscale_array = (unsigned char *) malloc(width*height);
	for (y = 0 ; y < height ; y++) {
		for (x = 0 ; x < width ; x++) {
			rgb = pixels[y*width+x];
			// IORD_16DIRECT(pixels,y*width+x); // read in main memory

			// Solution de base (0.864)
			/*
			gray = (((rgb>>11)&0x1F)<<3)*21; // red part
			gray += (((rgb>>5)&0x3F)<<2)*72; // green part
			gray += (((rgb>>0)&0x1F)<<3)*7; // blue part
			gray /= 100;*/

			// Solution 1 (0.277 ) (1.68 2.88 0.56 * 1024)

			int red = ((rgb>>11)&0x1F) ;
			int green = ((rgb>>5)&0x1F) ;
			int blue = ((rgb>>0)&0x1F) ;

			int c = 3; //  1.68 * 16 = 27;	3
			int b = 5; // 2.88 * 16 = 46;	5  2
			int a = 20; // 0.56 * 16 = 9;	1		20

			gray = (int)((red * c) + (green * b) + (blue*a) ) >> 2;
			// IOWR_8DIRECT(grayscale_array,y*width+x,gray);
			grayscale_array[y*width+x] = gray; // write in cache
		}
	}
}


int get_grayscale_width() {
	return grayscale_width;
}

int get_grayscale_height() {
	return grayscape_height;
}

unsigned char *get_grayscale_picture() {
	return grayscale_array;
}


