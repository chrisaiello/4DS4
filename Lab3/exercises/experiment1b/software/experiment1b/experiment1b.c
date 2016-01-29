// Copyright by Adam Kinsman, Henry Ko and Nicola Nicolici
// Developed for the Embedded Systems course (COE4DS4)
// Department of Electrical and Computer Engineering
// McMaster University
// Ontario, Canada

#include "define.h"

static alt_u16 sw_buf;
static int floor_request, current_floor, direction;
extern int floor_arrival, doors_open;
alt_u16 disp_seven_seg(alt_u8 val) {
    switch (val) {
        case  0 : return 0x40;
        case  1 : return 0x79;
        case  2 : return 0x24;
        case  3 : return 0x30;
        case  4 : return 0x19;
        case  5 : return 0x12;
        case  6 : return 0x02;
        case  7 : return 0x78;
        case  8 : return 0x00;
        case  9 : return 0x18;
        case 10 : return 0x08;
        case 11 : return 0x03;
        case 12 : return 0x46;
        case 13 : return 0x21;
        case 14 : return 0x06;
        case 15 : return 0x0e;
        default : return 0x7f;
    }
}
void SW_interrupt(void) {


	//if (sw_buf != (IORD(SWITCH_I_BASE, 0)& 0x3FFF ))



    floor_request = (floor_request | (IORD(SWITCH_I_BASE, 0)& 0x3FFF ));

    IOWR(LED_RED_O_BASE, 0, floor_request);

    printf("Floors requested: %x\n",floor_request);

    IOWR(SWITCH_I_BASE, 3, 0x0);

    //sw_buf = (IORD(SWITCH_I_BASE, 0)& 0x3FFF );

}

void check_floors(){

	if(floor_request!=0){
		if(direction==1 && floor_request>(0x1<<current_floor)){//up
			reset_counter();
			while(!floor_arrival){;}
			floor_arrival=0;
			printf("\nArrived at floor %x\n",++current_floor);
			if(floor_request & (0x1<<current_floor)){//if there is a request for the current floor
				floor_request=floor_request^(0x1<<current_floor); //turn off LED when we arrive at the floor, remove floor from request list
				IOWR(LED_RED_O_BASE, 0, floor_request);
				printf("\nOpening Doors...\n"); //we need to stop and open our doors here
				doors_open=1;
				reset_counter2();
				while(doors_open==1){;}//wait for counter
				doors_open=1;//yeah that's confusing, trust me.
				printf("\nClosing Doors...\n");

			}
		}

	}

}

int main()
{
	current_floor = 0;
	sw_buf = 0;
	direction = 1;
	printf("Start main...\n");

	init_button_irq();
	printf("PB initialized...\n");

	init_counter_irq();
	printf("Counter IRQ initialized...\n");

	init_counter_irq2();
	printf("Counter 2 IRQ initialized...\n");

	IOWR(LED_GREEN_O_BASE, 0, 0x0);
	IOWR(LED_RED_O_BASE, 0, 0x0);


	IOWR(SEVEN_SEGMENT_N_O_0_BASE, 0, disp_seven_seg(16));


	IOWR(SWITCH_I_BASE, 3, 0x0); // edge capture register
	IOWR(SWITCH_I_BASE, 2, 0x3FFF); // IRQ mask
	alt_irq_register(SWITCH_I_IRQ, NULL, (void*)SW_interrupt);

	printf("Switch value: %d\n", IORD(SWITCH_I_BASE, 0));

	while (1){
		check_floors();

	}

	return 0;
}
