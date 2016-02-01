// Copyright by Adam Kinsman, Henry Ko and Nicola Nicolici
// Developed for the Embedded Systems course (COE4DS4)
// Department of Electrical and Computer Engineering
// McMaster University
// Ontario, Canada

#ifndef __CUSTOM_COUNTER2_H__
#define __CUSTOM_COUNTER2_H__

// Global functions
void handle_counter_expire_interrupts2();
int read_counter2();
void reset_counter2();
int read_counter_interrupt2();
void init_counter_irq2();
void load_counter_config2(int);

#endif
