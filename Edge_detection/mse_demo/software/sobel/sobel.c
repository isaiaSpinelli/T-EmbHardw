/*
 * sobel.c
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include <system.h>

const char gx_array[3][3] = {{-1,0,1},
                             {-2,0,2},
                             {-1,0,1}};
const char gy_array[3][3] = { {1, 2, 1},
                              {0, 0, 0},
                             {-1,-2,-1}};

short *sobel_x_result;
short *sobel_y_result;
unsigned short *sobel_rgb565;
unsigned char *sobel_result;
int sobel_width;
int sobel_height;

void init_sobel_arrays(int width , int height) {
	int loop;
	sobel_width = width;
	sobel_height = height;
	if (sobel_x_result != NULL)
		free(sobel_x_result);
	sobel_x_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_y_result != NULL)
		free(sobel_y_result);
	sobel_y_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_result != NULL)
		free(sobel_result);
	sobel_result = (unsigned char *)malloc(width*height*sizeof(unsigned char));
	if (sobel_rgb565 != NULL)
		free(sobel_rgb565);
	sobel_rgb565 = (unsigned short *)malloc(width*height*sizeof(unsigned short));
	for (loop = 0 ; loop < width*height ; loop++) {
		sobel_x_result[loop] = 0;
		sobel_y_result[loop] = 0;
		sobel_result[loop] = 0;
		sobel_rgb565[loop] = 0;
	}
}

short sobel_mac( unsigned char *pixels,
                 int x,
                 int y,
                 const char *filter,
                 unsigned int width ) {
   short result = 0;

  result += filter[(0)*3]*pixels[(y-1)*width+(x-1)];
  result += filter[(0)*3+(1)]*pixels[(y-1)*width+(x)];
  result += filter[(0)*3+(2)]*pixels[(y-1)*width+(x+1)];

  result += filter[(1)*3]*pixels[(y)*width+(x-1)];
  result += filter[(1)*3+(1)]*pixels[(y)*width+(x)];
  result += filter[(1)*3+(2)]*pixels[(y)*width+(x+1)];

  result += filter[(2)*3]*pixels[(y+1)*width+(x-1)];
  result += filter[(2)*3+(1)]*pixels[(y+1)*width+(x)];
  result += filter[(2)*3+(2)]*pixels[(y+1)*width+(x+1)];

   return result;
}

void sobel_x( unsigned char *source ) {
   int x,y;
   short result = 0;

   const int width = sobel_width;
   const char *filter = gx_array;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {

    	  result = 0;

    	  result += filter[(0)*3]*source[(y-1)*width+(x-1)];
    	  result += filter[(0)*3+(1)]*source[(y-1)*width+(x)];
    	  result += filter[(0)*3+(2)]*source[(y-1)*width+(x+1)];

    	  result += filter[(1)*3]*source[(y)*width+(x-1)];
    	  result += filter[(1)*3+(1)]*source[(y)*width+(x)];
    	  result += filter[(1)*3+(2)]*source[(y)*width+(x+1)];

    	  result += filter[(2)*3]*source[(y+1)*width+(x-1)];
    	  result += filter[(2)*3+(1)]*source[(y+1)*width+(x)];
    	  result += filter[(2)*3+(2)]*source[(y+1)*width+(x+1)];
          sobel_x_result[y*sobel_width+x] = result ; // sobel_mac(source,x,y,gx_array,sobel_width);

      }
   }
}

void sobel_x_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gx_array,sobel_width);
          sobel_x_result[y*sobel_width+x] = result;
          if (result < 0) {
        	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
          } else {
        	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
          }
      }
   }
}

// merge sobel_x and sobel_y
void sobel_complete( unsigned char *source ) {

	int x,y;

	short result_x, result_y = 0;

	const int width = sobel_width;
	const char *filter_x = gx_array;
	const char *filter_y = gy_array;

	const int s_width = sobel_width;
	const int s_height = sobel_height;

	short sum;

	   for (y = 1 ; y < (s_height-1) ; y++) {
	      for (x = 1 ; x < (s_width-1) ; x++) {

	    	  result_x = 0;
	    	  result_y = 0;
	    	  // Inlining and unrolling

	    	  // use cache for source
	    	  result_x += filter_x[0]*source[(y-1)*width+(x-1)];
	    	  result_x += filter_x[1]*source[(y-1)*width+(x)];
	    	  result_x += filter_x[2]*source[(y-1)*width+(x+1)];
	    	  result_x += filter_x[3]*source[(y)*width+(x-1)];
	    	  result_x += filter_x[4]*source[(y)*width+(x)];
	    	  result_x += filter_x[5]*source[(y)*width+(x+1)];
	    	  result_x += filter_x[6]*source[(y+1)*width+(x-1)];
	    	  result_x += filter_x[7]*source[(y+1)*width+(x)];
	    	  result_x += filter_x[8]*source[(y+1)*width+(x+1)];

	    	  result_y += filter_y[0]*source[(y-1)*width+(x-1)];
	    	  result_y += filter_y[1]*source[(y-1)*width+(x)];
	    	  result_y += filter_y[2]*source[(y-1)*width+(x+1)];
	    	  result_y += filter_y[3]*source[(y)*width+(x-1)];
	    	  result_y += filter_y[4]*source[(y)*width+(x)];
	    	  result_y += filter_y[5]*source[(y)*width+(x+1)];
	    	  result_y += filter_y[6]*source[(y+1)*width+(x-1)];
	    	  result_y += filter_y[7]*source[(y+1)*width+(x)];
	    	  result_y += filter_y[8]*source[(y+1)*width+(x+1)];

	    	  // threshold
				sum = (result_x < 0) ? -result_x : result_x;
				sum += (result_y < 0) ? -result_y : result_y;
				//sobel_result[y*width+x] = (sum > 128) ? 0xFF : 0; // result in main memory
				IOWR_8DIRECT(sobel_result,y*width+x, (sum > 128) ? 0xFF : 0 ); // result in main memory

	      }
	   }
}

void sobel_and_gray_complete_CI(void *picture){

	int x,y,k;
	//unsigned char * source1 = source;

	volatile char thr2;
	short result_x, result_y = 0;

	const char *filter_x = gx_array;
	const char *filter_y = gy_array;


	unsigned short *pixels = (unsigned short *)picture;
	unsigned short rgb;


	const unsigned char divHeightBy = 8;
	const unsigned char divWidthBy = 8;
	const unsigned char nbImg = divWidthBy * divHeightBy ;

	unsigned char divImg = nbImg;
	const int s_width = sobel_width/divWidthBy;
	const int s_height = sobel_height/divHeightBy;


	unsigned char * grayscale_array = (unsigned char *) malloc(s_width*s_height);
	if (grayscale_array == NULL){
		printf("Error malloc grayscale !");
		return ;
	}


	for (k = 0 ; k < divImg -1; k++) {

		if (k%8 >= 4 || k>=56) {
			continue;
		}

	   int offsetWidth = ((k%divWidthBy)*s_width);
	   int offsetHeight = ((k/divHeightBy) * s_height);

	   pixels = picture + ((offsetWidth + (offsetHeight*sobel_width))*2); // * 2

	   for (y = 0 ; y < s_height ; y++) {
	   		for (x = 0 ; x < s_width ; x++) {
				rgb = pixels[y*sobel_width+x];
				// write in cache (rgb to graz CI)
				grayscale_array[y*s_width+x] = ALT_CI_CI_TEST_0(rgb);
	   		}
	   }



	   for (y = 1 ; y < (s_height-1) ; y++) {
			  for (x = 1 ; x < (s_width-1) ; x++) {
				  result_x = 0;
				  result_y = 0;
				  // Inlining and unrolling

				  // use cache for source
				  result_x += filter_x[0]*grayscale_array[(y-1)*s_width+(x-1)];
				  result_y += filter_y[0]*grayscale_array[(y-1)*s_width+(x-1)];

				  result_x += filter_x[1]*grayscale_array[(y-1)*s_width+(x)];
				  result_y += filter_y[1]*grayscale_array[(y-1)*s_width+(x)];

				  result_x += filter_x[2]*grayscale_array[(y-1)*s_width+(x+1)];
				  result_y += filter_y[2]*grayscale_array[(y-1)*s_width+(x+1)];

				  result_x += filter_x[3]*grayscale_array[(y)*s_width+(x-1)];
				  result_y += filter_y[3]*grayscale_array[(y)*s_width+(x-1)];

				  result_x += filter_x[4]*grayscale_array[(y)*s_width+(x)];
				  result_y += filter_y[4]*grayscale_array[(y)*s_width+(x)];

				  result_x += filter_x[5]*grayscale_array[(y)*s_width+(x+1)];
				  result_y += filter_y[5]*grayscale_array[(y)*s_width+(x+1)];

				  result_x += filter_x[6]*grayscale_array[(y+1)*s_width+(x-1)];
				  result_y += filter_y[6]*grayscale_array[(y+1)*s_width+(x-1)];

				  result_x += filter_x[7]*grayscale_array[(y+1)*s_width+(x)];
				  result_y += filter_y[7]*grayscale_array[(y+1)*s_width+(x)];

				  result_x += filter_x[8]*grayscale_array[(y+1)*s_width+(x+1)];
				  result_y += filter_y[8]*grayscale_array[(y+1)*s_width+(x+1)];


				// threshold CI
				thr2 = ALT_CI_THRESHOLD_0(result_x, result_y);
				// result in main memory
				IOWR_8DIRECT(sobel_result, (((y+offsetHeight)*sobel_width) +  (x+offsetWidth)) , thr2 );

			  }
		   }



   }


	free(grayscale_array);

}

void sobel_and_gray_complete(void *picture){

	int x,y,k,gray;

	short result_x, result_y = 0;

	const char *filter_x = gx_array;
	const char *filter_y = gy_array;


	short sum;

	unsigned short *pixels = (unsigned short *)picture;
	unsigned short rgb;


	const unsigned char divHeightBy = 8;
	const unsigned char divWidthBy = 8;
	const unsigned char nbImg = divWidthBy * divHeightBy ;

	unsigned char divImg = nbImg;
	const int s_width = sobel_width/divWidthBy;
	const int s_height = sobel_height/divHeightBy;


	unsigned char * grayscale_array = (unsigned char *) malloc(s_width*s_height);
	if (grayscale_array == NULL){
		printf("Error malloc grayscale !");
		return ;
	}


	for (k = 0 ; k < divImg -1; k++) {

		if (k%8 >= 4 || k>=56) {
			continue;
		}

	   int offsetWidth = ((k%divWidthBy)*s_width);
	   int offsetHeight = ((k/divHeightBy) * s_height);

	   pixels = picture + ((offsetWidth + (offsetHeight*sobel_width))*2); // * 2

	   for (y = 0 ; y < s_height ; y++) {
	   		for (x = 0 ; x < s_width ; x++) {
				rgb = pixels[y*sobel_width+x];

				// Solution de base
				gray = (((rgb>>11)&0x1F)<<3)*21; // red part
				gray += (((rgb>>5)&0x3F)<<2)*72; // green part
				gray += (((rgb>>0)&0x1F)<<3)*7; // blue part
				gray /= 100;

				grayscale_array[y*s_width+x] = gray; // write in cache
			}
	   }



	   for (y = 1 ; y < (s_height-1) ; y++) {
			  for (x = 1 ; x < (s_width-1) ; x++) {
				  result_x = 0;
				  result_y = 0;
				  // Inlining and unrolling

				  // use cache for source
				  result_x += filter_x[0]*grayscale_array[(y-1)*s_width+(x-1)];
				  result_y += filter_y[0]*grayscale_array[(y-1)*s_width+(x-1)];

				  result_x += filter_x[1]*grayscale_array[(y-1)*s_width+(x)];
				  result_y += filter_y[1]*grayscale_array[(y-1)*s_width+(x)];

				  result_x += filter_x[2]*grayscale_array[(y-1)*s_width+(x+1)];
				  result_y += filter_y[2]*grayscale_array[(y-1)*s_width+(x+1)];

				  result_x += filter_x[3]*grayscale_array[(y)*s_width+(x-1)];
				  result_y += filter_y[3]*grayscale_array[(y)*s_width+(x-1)];

				  result_x += filter_x[4]*grayscale_array[(y)*s_width+(x)];
				  result_y += filter_y[4]*grayscale_array[(y)*s_width+(x)];

				  result_x += filter_x[5]*grayscale_array[(y)*s_width+(x+1)];
				  result_y += filter_y[5]*grayscale_array[(y)*s_width+(x+1)];

				  result_x += filter_x[6]*grayscale_array[(y+1)*s_width+(x-1)];
				  result_y += filter_y[6]*grayscale_array[(y+1)*s_width+(x-1)];

				  result_x += filter_x[7]*grayscale_array[(y+1)*s_width+(x)];
				  result_y += filter_y[7]*grayscale_array[(y+1)*s_width+(x)];

				  result_x += filter_x[8]*grayscale_array[(y+1)*s_width+(x+1)];
				  result_y += filter_y[8]*grayscale_array[(y+1)*s_width+(x+1)];


				// threshold
				sum = (result_x < 0) ? -result_x : result_x;
				sum += (result_y < 0) ? -result_y : result_y;

				// result in main memory
				IOWR_8DIRECT(sobel_result, (((y+offsetHeight)*sobel_width) +  (x+offsetWidth)) , (sum > 128) ? 0xFF : 0 );
			  }
		   }
   }
	free(grayscale_array);

}

void sobel_and_gray_complete_modif_gray(void *picture){

	int x,y,k,gray;

	short result_x, result_y = 0;

	const char *filter_x = gx_array;
	const char *filter_y = gy_array;


	short sum;

	unsigned short *pixels = (unsigned short *)picture;
	unsigned short rgb;


	const unsigned char divHeightBy = 8;
	const unsigned char divWidthBy = 8;
	const unsigned char nbImg = divWidthBy * divHeightBy ;

	unsigned char divImg = nbImg;
	const int s_width = sobel_width/divWidthBy;
	const int s_height = sobel_height/divHeightBy;


	unsigned char * grayscale_array = (unsigned char *) malloc(s_width*s_height);
	if (grayscale_array == NULL){
		printf("Error malloc grayscale !");
		return ;
	}


	for (k = 0 ; k < divImg -1; k++) {

		if (k%8 >= 4 || k>=56) {
			continue;
		}

	   int offsetWidth = ((k%divWidthBy)*s_width);
	   int offsetHeight = ((k/divHeightBy) * s_height);

	   pixels = picture + ((offsetWidth + (offsetHeight*sobel_width))*2); // * 2

	   for (y = 0 ; y < s_height ; y++) {
	   		for (x = 0 ; x < s_width ; x++) {
				rgb = pixels[y*sobel_width+x];

				int red = ((rgb>>11)&0x1F) ;
				int green = ((rgb>>5)&0x1F) ;
				int blue = ((rgb>>0)&0x1F) ;
				int c = 3; //  1.68 * 16 = 27;	3
				int b = 5; // 2.88 * 16 = 46;	5  2
				int a = 20; // 0.56 * 16 = 9;	1		20
				gray = (int)((red * c) + (green * b) + (blue*a) ) >> 2;

				grayscale_array[y*s_width+x] = gray; // write in cache
			}
	   }



	   for (y = 1 ; y < (s_height-1) ; y++) {
			  for (x = 1 ; x < (s_width-1) ; x++) {
				  result_x = 0;
				  result_y = 0;
				  // Inlining and unrolling

				  // use cache for source
				  result_x += filter_x[0]*grayscale_array[(y-1)*s_width+(x-1)];
				  result_y += filter_y[0]*grayscale_array[(y-1)*s_width+(x-1)];

				  result_x += filter_x[1]*grayscale_array[(y-1)*s_width+(x)];
				  result_y += filter_y[1]*grayscale_array[(y-1)*s_width+(x)];

				  result_x += filter_x[2]*grayscale_array[(y-1)*s_width+(x+1)];
				  result_y += filter_y[2]*grayscale_array[(y-1)*s_width+(x+1)];

				  result_x += filter_x[3]*grayscale_array[(y)*s_width+(x-1)];
				  result_y += filter_y[3]*grayscale_array[(y)*s_width+(x-1)];

				  result_x += filter_x[4]*grayscale_array[(y)*s_width+(x)];
				  result_y += filter_y[4]*grayscale_array[(y)*s_width+(x)];

				  result_x += filter_x[5]*grayscale_array[(y)*s_width+(x+1)];
				  result_y += filter_y[5]*grayscale_array[(y)*s_width+(x+1)];

				  result_x += filter_x[6]*grayscale_array[(y+1)*s_width+(x-1)];
				  result_y += filter_y[6]*grayscale_array[(y+1)*s_width+(x-1)];

				  result_x += filter_x[7]*grayscale_array[(y+1)*s_width+(x)];
				  result_y += filter_y[7]*grayscale_array[(y+1)*s_width+(x)];

				  result_x += filter_x[8]*grayscale_array[(y+1)*s_width+(x+1)];
				  result_y += filter_y[8]*grayscale_array[(y+1)*s_width+(x+1)];


				// threshold
				sum = (result_x < 0) ? -result_x : result_x;
				sum += (result_y < 0) ? -result_y : result_y;

				// result in main memory
				IOWR_8DIRECT(sobel_result, (((y+offsetHeight)*sobel_width) +  (x+offsetWidth)) , (sum > 128) ? 0xFF : 0 );
			  }
		   }
   }
	free(grayscale_array);

}


void sobel_y( unsigned char *source ) {
   int x,y;

   short result = 0;

   const int width = sobel_width;
  const char *filter = gx_array;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {

    	  result = 0;

		  result += filter[(0)*3]*source[(y-1)*width+(x-1)];
		  result += filter[(0)*3+(1)]*source[(y-1)*width+(x)];
		  result += filter[(0)*3+(2)]*source[(y-1)*width+(x+1)];

		  result += filter[(1)*3]*source[(y)*width+(x-1)];
		  result += filter[(1)*3+(1)]*source[(y)*width+(x)];
		  result += filter[(1)*3+(2)]*source[(y)*width+(x+1)];

		  result += filter[(2)*3]*source[(y+1)*width+(x-1)];
		  result += filter[(2)*3+(1)]*source[(y+1)*width+(x)];
		  result += filter[(2)*3+(2)]*source[(y+1)*width+(x+1)];
		  sobel_y_result[y*sobel_width+x] = result ; // sobel_mac(source,x,y,gx_array,sobel_width);

         //sobel_y_result[y*sobel_width+x] = sobel_mac(source,x,y,gy_array,sobel_width);
      }
   }
}

void sobel_y_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gy_array,sobel_width);
         sobel_y_result[y*sobel_width+x] = result;
         if (result < 0) {
       	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
         } else {
       	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
         }
      }
   }
}

void sobel_threshold(short threshold) {
	int x,y,arrayindex;
	short sum,value;
	for (y = 1 ; y < (sobel_height-1) ; y++) {
		for (x = 1 ; x < (sobel_width-1) ; x++) {
			arrayindex = (y*sobel_width)+x;
			value = sobel_x_result[arrayindex];
			sum = (value < 0) ? -value : value;
			value = sobel_y_result[arrayindex];
			sum += (value < 0) ? -value : value;
			sobel_result[arrayindex] = (sum > threshold) ? 0xFF : 0;
		}
	}
}

unsigned short *GetSobel_rgb() {
	return sobel_rgb565;
}

unsigned char *GetSobelResult() {
	return sobel_result;
}
