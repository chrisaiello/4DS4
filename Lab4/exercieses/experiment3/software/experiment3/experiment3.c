// Copyright by Adam Kinsman, Henry Ko and Nicola Nicolici
// Developed for the Embedded Systems course (COE4DS4)
// Department of Electrical and Computer Engineering
// McMaster University
// Ontario, Canada

// This is the embedded software for the
// LCD / Camera design


#include <stdio.h>
#include "io.h"
#include "system.h"
#include "sys/alt_irq.h"
#include "sys/alt_stdio.h"
#include "priv/alt_busy_sleep.h"
#include "alt_types.h"

int coeff_id=0;
void TouchPanel_int(void) {
    static int exposure = 0x0400, run = 1;
    static int config = 0;
	static int new_config = 4;
    int TP_val, x_val, y_val, key = 6;

    //note - the 0th entry here is multiplied by two to avoid the 0.5's

    alt_8        C_m1_m1[7]  =	{2, 1, 1, 1,-1, 0,-1};    	//coeffs0 byte 1
    alt_8		 C_m1_0[7]   = 	{2, 1, 1,-2,-1,-1,-2};	//coeffs0 byte 0
    alt_8		 C_m1_p1[7]  =	{2, 1, 2, 1,-1, 0,-1};		//coeffs1 byte 3
    alt_8		 C_0_m1[7]   =	{1, 1, 2,-2,-1,-1,-2};	//coeffs1 byte 2
    alt_8		 C_0_0[7]    =	{2, 8, 4, 5, 9, 5, 28};		//coeffs1 byte 1
    alt_8		 C_0_p1[7]   = 	{1, 1, 2,-2,-1,-1,-2};	//coeffs1 byte 0
    alt_8		 C_p1_m1[7]  = 	{2, 1, 2, 1,-1, 0,-1};		//coeffs2 byte 3
    alt_8		 C_p1_0[7]   = 	{2, 1, 1,-2,-1,-1,-2};	//coeffs2 byte 2
    alt_8		 C_p1_p1[7]  = 	{2, 1, 1, 1,-1, 0,-1};		//coeffs2 byte 1
    alt_8		 scale[7]    = 	{16,16,16,1, 1, 1, 16};	//coeffs2 byte 0

    alt_u32 coeffs0=0x0000, coeffs1=0x0000, coeffs2=0x0000;



    TP_val = IORD(NIOS_LCD_CAMERA_COMPONENT_0_TOUCHPANEL_BASE, 0);
    x_val = (TP_val >> 20) & 0xFF; y_val = (TP_val >> 4) & 0xFF;

    if (((TP_val >> 31) & 0x1) && (x_val >= 0xC9) && (x_val <= 0xF1)) {
        if ((y_val >= 0x17) && (y_val <= 0x33)) { // Key 0
            key = 0;
            IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CONSOLE_BASE, 0, 0x1);
        }
        if ((y_val >= 0x3D) && (y_val <= 0x58)) { // Key 1
            key = 1;
            IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CONSOLE_BASE, 0, 0x2);
        }
        if ((y_val >= 0x62) && (y_val <= 0x7E)) { // Key 2
            key = 2;
            IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CONSOLE_BASE, 0, 0x4);
        }
        if ((y_val >= 0x88) && (y_val <= 0xA4)) { // Key 3
            key = 3;
            IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CONSOLE_BASE, 0, 0x8);
        }
        if ((y_val >= 0xAE) && (y_val <= 0xC9)) { // Key 4
            key = 4;
            IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CONSOLE_BASE, 0, 0x10);
        }
        if ((y_val >= 0xD3) && (y_val <= 0xEF)) { // Key 5
            key = 5;
            IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CONSOLE_BASE, 0, 0x20);
        }
    } else IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CONSOLE_BASE, 0, 0x0);
    
    if (IORD(NIOS_LCD_CAMERA_COMPONENT_0_TOUCHPANEL_BASE, 2) & 0x2) { // posedge
        switch (key) {
            case 0 : 
                if (run == 1) {
                    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 1, 0x8);
                    run = 0;
                } else {
                    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 1, 0x4);
                    run = 1;
                }
                break;
            case 1 : 
                config++; if (config == 4) config = 0;
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_IMAGELINE_BASE, 4, config);
                break;
            case 2 : 
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_IMAGELINE_BASE, 4, 4);
                break;
            case 3 :

            	coeffs0=0x0000;
            	coeffs1=0x0000;
            	coeffs2=0x0000;

            	//shifting and ORing until the coefficients are in the right place.

                coeffs0 = C_m1_m1[coeff_id] & 0xFF;      //coeffs0 contains 2 8-bit values
                coeffs0 <<= 8;
                coeffs0 = coeffs0 | (C_m1_0[coeff_id] & 0xFF);

                coeffs1 = C_m1_p1[coeff_id] & 0xFF;		//coeffs1 contains 4 8- values
                coeffs1 <<= 8;
                coeffs1 = coeffs1 | (C_0_m1[coeff_id] & 0xFF);
                coeffs1 <<= 8;
                coeffs1 = coeffs1 | (C_0_0[coeff_id] & 0xFF);
                coeffs1 <<= 8;
                coeffs1 = coeffs1 | (C_0_p1[coeff_id] & 0xFF);

                coeffs2 = C_p1_m1[coeff_id] & 0xFF;		//coeffs2 contains 4 8-bit values
                coeffs2 <<= 8;
                coeffs2 = coeffs1 | (C_p1_0[coeff_id] & 0xFF);
                coeffs2 <<= 8;
                coeffs2 = coeffs1 | (C_p1_p1[coeff_id] & 0xFF);
                coeffs2 <<= 8;
                coeffs2 = coeffs1 | (scale[coeff_id] & 0xFF);

                printf("ID: %d\nCoeffs0: %.8X \nCoeffs1: %.8X \nCoeffs1: %.8X\n\n",coeff_id,coeffs0, coeffs1, coeffs2);

                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_IMAGELINE_BASE, 4, 5);
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_IMAGELINE_BASE, 5, coeffs0);
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_IMAGELINE_BASE, 6, coeffs1);
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_IMAGELINE_BASE, 7, coeffs2);

                coeff_id ++;
                if(coeff_id==7)coeff_id=0;
                break;
            case 4 : 
                if (exposure <= 0xFEFF) exposure += 0x0100;
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 0, exposure);
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 1, 0x2);
                break;                              
            case 5 : 
                if (exposure >= 0x0100) exposure -= 0x0100;
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 0, exposure);
                IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 1, 0x2);
                break;
        }
    }
    
    TP_val = IORD(NIOS_LCD_CAMERA_COMPONENT_0_TOUCHPANEL_BASE, 2);
    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_TOUCHPANEL_BASE, 2, TP_val & 0x30);
}

int main() 
{
    alt_irq_register(NIOS_LCD_CAMERA_COMPONENT_0_TOUCHPANEL_IRQ, NULL, (void *)TouchPanel_int);

    printf("Experiment 3!\n");

    // initialize the touch panel
    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_TOUCHPANEL_BASE, 2, 0x0);
    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_TOUCHPANEL_BASE, 1, 0x400000);

    // initialize the camera
    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 0, 0x0400);
    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 1, 0x2);
    while ((IORD(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 1) & 0x1) == 0);
    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CAMERA_BASE, 1, 0x4);

    // initialize the buttons
    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_CONSOLE_BASE, 1, 0x0);
    
    // initialize the filter pipe
    IOWR(NIOS_LCD_CAMERA_COMPONENT_0_IMAGELINE_BASE, 4, 0);

    while (1);

    
    return 0;
}
