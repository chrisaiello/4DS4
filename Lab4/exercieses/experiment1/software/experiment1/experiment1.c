// Copyright by Adam Kinsman, Henry Ko and Nicola Nicolici
// Developed for the Embedded Systems course (COE4DS4)
// Department of Electrical and Computer Engineering
// McMaster University
// Ontario, Canada

// This is the embedded software for the LCD design

#include <stdio.h>
#include "io.h"
#include "system.h"
#include "sys/alt_irq.h"
#include "sys/alt_stdio.h"
#include "priv/alt_busy_sleep.h"

static int curs_row=0, curs_col=0;
static int colour_offset[10][15];

int RGB_colour(int colour) {
    switch (colour & 0x7) {
        case 0 : return 0x00000000; // black 
        case 1 : return 0x000003FF; // red 
        case 2 : return 0x7C007C00; // green 
        case 3 : return 0x7C007FFF; // yellow
        case 4 : return 0x03FF0000; // blue
        case 5 : return 0x03FF03FF; // magenta
        case 6 : return 0x7FFF7C00; // cyan
        case 7 : return 0x7FFF7FFF; // white
    }
    return 0x00000000;
}    

void draw_horizontal_bars(int width);



void TouchPanel_int(void) {
    static int width = 32;

    int TP_val, x_val, y_val, key = 6;

    TP_val = IORD(NIOS_LCD_COMPONENT_0_TOUCHPANEL_BASE, 0);
    x_val = (TP_val >> 20) & 0xFF; y_val = (TP_val >> 4) & 0xFF;

    if (((TP_val >> 31) & 0x1) && (x_val >= 0xC9) && (x_val <= 0xF1)) {
        if ((y_val >= 0x17) && (y_val <= 0x33)) { // Key 0
            key = 0;
            IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x1);
        }
        if ((y_val >= 0x3D) && (y_val <= 0x58)) { // Key 1
            key = 1;
            IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x2);
        }
        if ((y_val >= 0x62) && (y_val <= 0x7E)) { // Key 2
            key = 2;
            IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x4);
        }
        if ((y_val >= 0x88) && (y_val <= 0xA4)) { // Key 3
            key = 3;
            IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x8);
        }
        if ((y_val >= 0xAE) && (y_val <= 0xC9)) { // Key 4
            key = 4;
            IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x10);
        }
        if ((y_val >= 0xD3) && (y_val <= 0xEF)) { // Key 5
            key = 5;
            IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x20);
        }
    } else IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x0);
        
    if (IORD(NIOS_LCD_COMPONENT_0_TOUCHPANEL_BASE, 2) & 0x2) { // posedge
        if ((key == 0)) {
            curs_row--;
            if(curs_row==-1){curs_row=14;}

            draw_horizontal_bars(32);
        }
        if ((key == 1)) {
            curs_row++;
            if(curs_row==15){curs_row=0;}
            draw_horizontal_bars(32);
        }
        if ((key == 2)) {
            curs_col--;
            if(curs_col==-1){curs_col=9;}
            draw_horizontal_bars(32);
        }
        if ((key == 3)) {
            curs_col++;
            if(curs_col==10){curs_col=0;}
            draw_horizontal_bars(32);
        }
        if ((key == 4)) {
        	colour_offset[curs_col][curs_row]++;
        	draw_horizontal_bars(32);

        }
        if ((key == 5)) {
        	colour_offset[curs_col][curs_row]--;
        	draw_horizontal_bars(32);
        }

    }

    IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x0);
    TP_val = IORD(NIOS_LCD_COMPONENT_0_TOUCHPANEL_BASE, 2);
    IOWR(NIOS_LCD_COMPONENT_0_TOUCHPANEL_BASE, 2, TP_val & 0x30);
}

void draw_horizontal_bars(int width) {
    int i, j, colour = 0, base_colour=0, colour_check;
    int RGB = RGB_colour((colour+colour_offset[j/64][i/32]));
    int row_count=0, col_count=0, same_colour=0;
    

    // Set pixel position to top-left corner
    IOWR(NIOS_LCD_COMPONENT_0_IMAGE_BASE, 2, 0x1);

    for (i = 0; i < 480; i++) {
        for (j = 0; j < 640; j++) {

        	if(j==0)RGB = RGB_colour(colour+colour_offset[j/64][i/32]);

        	if(j/64==curs_col && i/32==curs_row){      //if i and j are in the range of the coordinate given by curs_col,curs_row
        		if(col_count > 16 && col_count < 48 &&
        		   row_count > 8  && row_count < 24){
        			RGB = RGB_colour(~(colour+colour_offset[j/64][i/32]));
        		}
        		else{RGB = RGB_colour(colour+colour_offset[j/64][i/32]);}
        	}

        	IOWR(NIOS_LCD_COMPONENT_0_IMAGE_BASE, 0, RGB);
        	col_count++;

        	if(row_count==4){
        		if(j==0){
        			colour_check = RGB;
        		}
        		else{
        			if (RGB == colour_check){
        				same_colour++;
        			}
        		}
        	}
        	if(col_count==64){
        		col_count=0;
        		colour--;
        		RGB = RGB_colour(colour+colour_offset[(j+1)/64][i/32]);
        	}


        }

        //if(same_colour>400)printf("Same_colour: %d\n",same_colour);
        if(same_colour>620)printf("All rectangles from row %d have colour %X\n",((i-5)/32),colour_check);
        same_colour=0;

        row_count++;

        colour=base_colour;
        RGB = RGB_colour(colour+colour_offset[j/64][i/32]);

        if (row_count == 32) {
            base_colour-=2;
            if(base_colour==-2)base_colour=6;
            colour=base_colour;
            row_count = 0;
            RGB = RGB_colour(colour+colour_offset[j/64][(i+1)/32]);
        }
    }
}

int main()
{

	int i,j;
	for(i=0;i<10;i++){
		for(j=0;j<15;j++){
			colour_offset[i][j]=0;
		}
	}

    printf("Experiment 1!\n");
    alt_irq_register(NIOS_LCD_COMPONENT_0_TOUCHPANEL_IRQ, NULL, (void *)TouchPanel_int);

    // Turn button indicators off
    IOWR(NIOS_LCD_COMPONENT_0_CONSOLE_BASE, 0, 0x0);

    draw_horizontal_bars(32);

    while (1);        

    return 0;
}
