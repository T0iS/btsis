#include "default.h"
#include <stdio.h>
#include "l298n.h"


void l298n_init(void)
{
        L298N_DDR |= (1<<L298N_IN1) | (1<<L298N_IN2) | (1<<L298N_IN3) | (1<<L298N_IN4);

	DDRD  |=(1<<4)|(1<<5);
	PORTD |=(1<<4)|(1<<5);
}


