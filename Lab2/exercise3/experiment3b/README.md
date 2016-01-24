Exercise 3

The amount of money availble in incremented when the first four switches are toggled. The pointer to amount is passed to
the interrupt handlers for both switch groups A and B. The interrupt handler for group B hnadles the logic for puchasing
tickets and lighting the LEDs by comparing the value of 'amount' to the value required for the selected destination.
If 'amount' is insufficient, more money may be added, otherwise the value of the ticket is subtracted from 'amount' and any
change is shown on the 7-segment display. LEDs are lit up as required. The transaction can be canccled at any time, which
will reset 'amount' to 0. The main function was modified so that the two seven-segment displays are off upon initialization.
