#include "define.h"

// ISR when the counter is expired
void handle_counter_expire_interrupts()
{
	printf("Counter expires\n");

	IOWR(CUSTOM_COUNTER_COMPONENT_0_BASE, 2, 0);
}

void reset_counter() {
	printf("Resetting counter value\n");

	IOWR(CUSTOM_COUNTER_COMPONENT_0_BASE, 1, 1);
	IOWR(CUSTOM_COUNTER_COMPONENT_0_BASE, 1, 0);

	IOWR(CUSTOM_COUNTER_COMPONENT_0_BASE, 2, 0);
}

int read_counter() {
	return IORD(CUSTOM_COUNTER_COMPONENT_0_BASE, 0);
}

int read_counter_interrupt() {
	return IORD(CUSTOM_COUNTER_COMPONENT_0_BASE, 2);
}

void load_counter_config(int config) {
	printf("Loading counter config %d\n", config);

	IOWR(CUSTOM_COUNTER_COMPONENT_0_BASE, 3, config);
}

// Function for initializing the ISR of the Counter
void init_counter_irq() {
	IOWR(CUSTOM_COUNTER_COMPONENT_0_BASE, 2, 0);

	alt_irq_register(CUSTOM_COUNTER_COMPONENT_0_IRQ, NULL, (void*)handle_counter_expire_interrupts );
}
