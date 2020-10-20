/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */


#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include "system.h"
#include "alt_types.h"
#include "sys/alt_irq.h"

#include "image_lcd.h"

#define CONTROL_OFFSET 0x00
#define COM_OFFSET 0x02
#define DATA_OFFSET 0x04
#define RESET_OFFSET 0x06

// OFFSET Timer interval
#define STATUS_OFFSET 0x00
#define CONTROL_OFFSET_TIMER 0x04

#define X_PIXEL_SIZE 320
#define Y_PIXEL_SIZE 240

void LCD_Write_Command(int command);
void LCD_Write_Data(int data) ;
void init_LCD();
void full_lcd_color(alt_16 color);

// r (red) ) 5 bit > 0 - 31
// g (green) ) 6 bit > 0 - 63
// b (blue) ) 5 bit > 0 - 31
void full_lcd_rgb(int r, int g, int b);

int counter = 0;

void timer_interrupt (void* context, alt_u32 id) {
	 // increase the counter
	counter++;
	// acknowledge IRQ on the timer
	IOWR_16DIRECT(TIMER_0_BASE, STATUS_OFFSET ,0x00);

}

int main()
{
	char  *data = header_data ;
	unsigned char pixel[4];
	int size_img = X_PIXEL_SIZE * Y_PIXEL_SIZE;
	int k = 0;
	int idx= 0;


	//int controle = 0;
	//alt_16 color = 0;
	alt_16 color[size_img+1];
	alt_16 color_reverse[size_img+1];
	int x,y = 0;
	//controle = IORD_16DIRECT(LCD_0_BASE,CONTROL_OFFSET); // Adapt this line

	// link irq to handler
	alt_irq_register(TIMER_0_IRQ,(void*)2,(alt_isr_func)timer_interrupt);
	// start timer and active irq
	IOWR_16DIRECT(TIMER_0_BASE, CONTROL_OFFSET_TIMER ,0x7);

	init_LCD();


	full_lcd_rgb(0,63,0);
	full_lcd_rgb(0,0,31);
	full_lcd_rgb(31,0,0);
	full_lcd_rgb(31,63,0);
	full_lcd_rgb(0,63,31);
	full_lcd_rgb(31,0,31);
	full_lcd_rgb(0,0,0);
	full_lcd_rgb(31,63,31);


	while(k++ < size_img) {

	    pixel[0] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))) / 8;
	    pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)) / 4;
	    pixel[2] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)) / 8;
	    pixel[3] = 0;
	    data += 4;

	    //HEADER_PIXEL(header_data, pixel)

	    color_reverse[k-1] = (((pixel[2]) & 0x1f) | ((pixel[1]) & 0x3f) << 5 | ((pixel[0]) & 0x1f) << (5+6)) & 0xFFFF ;

	}

	LCD_Write_Command(0x002c);     // write pixels
	for(x=0; x < X_PIXEL_SIZE; ++x){
		for(y=0; y < Y_PIXEL_SIZE; ++y){
			LCD_Write_Data(color_reverse[ (319-x) + (320*y) ] );
			color[idx++] = color_reverse[ (319-x) + (320*y) ];
		}
	}

	full_lcd_rgb(31,63,31);

	LCD_Write_Command(0x002c);     // write pixels
	k =0;
	while(k < size_img) {
		LCD_Write_Data(color[k++] );
	}

  return 0;
}

// r (red) ) 5 bit > 0 - 31
// g (green) ) 6 bit > 0 - 63
// b (blue) ) 5 bit > 0 - 31
void full_lcd_rgb(int r, int g, int b){
	alt_16 color = 0;

	color = (r & 0x1f) | (g & 0x3f) << 5| (b & 0x1f) << (5+6) ;

	full_lcd_color(color);

}

void full_lcd_color(alt_16 color){

	int x,y = 0;

	LCD_Write_Command(0x002c);     // write pixels

	for(x=0; x < X_PIXEL_SIZE; ++x){
		for(y=0; y < Y_PIXEL_SIZE; ++y){
			LCD_Write_Data(color);
		}
	}
}

void init_LCD() {

      IOWR_8DIRECT(LCD_0_BASE,RESET_OFFSET,0x01); // set reset on and 16 bits mode
      while (counter<100){}   // include delay of at least 120 ms use your timer or a loop
      IOWR_8DIRECT(LCD_0_BASE,RESET_OFFSET,0x00); // set reset off and 16 bits mode and enable LED_CS
      while (counter<200){}   // include delay of at least 120 ms use your timer or a loop

      LCD_Write_Command(0x0028);     //display OFF
      LCD_Write_Command(0x0011);     //exit SLEEP mode
      LCD_Write_Data(0x0000);

      LCD_Write_Command(0x00CB);     //Power Control A
      LCD_Write_Data(0x0039);     //always 0x39
      LCD_Write_Data(0x002C);     //always 0x2C
      LCD_Write_Data(0x0000);     //always 0x00
      LCD_Write_Data(0x0034);     //Vcore = 1.6V
      LCD_Write_Data(0x0002);     //DDVDH = 5.6V

      LCD_Write_Command(0x00CF);     //Power Control B
      LCD_Write_Data(0x0000);     //always 0x00
      LCD_Write_Data(0x0081);     //PCEQ off
      LCD_Write_Data(0x0030);     //ESD protection

      LCD_Write_Command(0x00E8);     //Driver timing control A
      LCD_Write_Data(0x0085);     //non - overlap
      LCD_Write_Data(0x0001);     //EQ timing
      LCD_Write_Data(0x0079);     //Pre-charge timing


      LCD_Write_Command(0x00EA);     //Driver timing control B
      LCD_Write_Data(0x0000);        //Gate driver timing
      LCD_Write_Data(0x0000);        //always 0x00

      LCD_Write_Command(0x00ED); //Power On sequence control
      LCD_Write_Data(0x0064);        //soft start
      LCD_Write_Data(0x0003);        //power on sequence
      LCD_Write_Data(0x0012);        //power on sequence
      LCD_Write_Data(0x0081);        //DDVDH enhance on

      LCD_Write_Command(0x00F7);     //Pump ratio control
      LCD_Write_Data(0x0020);     //DDVDH=2xVCI

      LCD_Write_Command(0x00C0);    //power control 1
      LCD_Write_Data(0x0026);
      LCD_Write_Data(0x0004);     //second parameter for ILI9340 (ignored by ILI9341)

      LCD_Write_Command(0x00C1);     //power control 2
      LCD_Write_Data(0x0011);

      LCD_Write_Command(0x00C5);     //VCOM control 1
      LCD_Write_Data(0x0035);
      LCD_Write_Data(0x003E);

      LCD_Write_Command(0x00C7);     //VCOM control 2
      LCD_Write_Data(0x00BE);

      LCD_Write_Command(0x00B1);     //frame rate control
      LCD_Write_Data(0x0000);
      LCD_Write_Data(0x0010);

      LCD_Write_Command(0x003A);    //pixel format = 16 bit per pixel
      LCD_Write_Data(0x0055);

      LCD_Write_Command(0x00B6);     //display function control
      LCD_Write_Data(0x000A);
      LCD_Write_Data(0x00A2);

      LCD_Write_Command(0x00F2);     //3G Gamma control
      LCD_Write_Data(0x0002);         //off

      LCD_Write_Command(0x0026);     //Gamma curve 3
      LCD_Write_Data(0x0001);

      LCD_Write_Command(0x0036);     //memory access control = BGR
      LCD_Write_Data(0x0000);

      LCD_Write_Command(0x002A);     //column address set
      LCD_Write_Data(0x0000);
      LCD_Write_Data(0x0000);        //start 0x0000
      LCD_Write_Data(0x0000);
      LCD_Write_Data(0x00EF);        //end 0x00EF

      LCD_Write_Command(0x002B);    //page address set
      LCD_Write_Data(0x0000);
      LCD_Write_Data(0x0000);        //start 0x0000
      LCD_Write_Data(0x0001);
      LCD_Write_Data(0x003F);        //end 0x013F

      LCD_Write_Command(0x0029);

  }

  void LCD_Write_Command(int command) {
      IOWR_16DIRECT(LCD_0_BASE,COM_OFFSET,command); // Adapt this line
  }

  void LCD_Write_Data(int data) {
      IOWR_16DIRECT(LCD_0_BASE,DATA_OFFSET,data); // Adapt this line
  }
