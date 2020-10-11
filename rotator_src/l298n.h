#ifndef L298N_H
#define L298N_H

#include <avr/io.h>

#define L298N_IN1                 PINC7
#define L298N_IN2                 PINC6
#define L298N_IN3                 PINC3
#define L298N_IN4                 PINC4

#define L298N_PORT               PORTC
#define L298N_DDR                DDRC

#define MOT_LEFT                {L298N_PORT|= (1<<L298N_IN1);L298N_PORT&=~(1<<L298N_IN2);}
#define MOT_RIGHT               {L298N_PORT|= (1<<L298N_IN2);L298N_PORT&=~(1<<L298N_IN1);}
#define MOT_LR_STOP             {L298N_PORT&=~(1<<L298N_IN1);L298N_PORT&=~(1<<L298N_IN2);}

#define MOT_UP                  {L298N_PORT|= (1<<L298N_IN3);L298N_PORT&=~(1<<L298N_IN4);}
#define MOT_DOWN                {L298N_PORT|= (1<<L298N_IN4);L298N_PORT&=~(1<<L298N_IN3);}
#define MOT_UD_STOP             {L298N_PORT&=~(1<<L298N_IN3);L298N_PORT&=~(1<<L298N_IN4);}

//#define MOT_UP                  {L298N_PORT|= (1<<L298N_IN1);L298N_PORT&=~(1<<L298N_IN2);}
//#define MOT_DOWN                {L298N_PORT|= (1<<L298N_IN2);L298N_PORT&=~(1<<L298N_IN1);}
//#define MOT_UD_STOP             {L298N_PORT&=~(1<<L298N_IN1);L298N_PORT&=~(1<<L298N_IN2);}

//#define MOT_LEFT                {L298N_PORT|= (1<<L298N_IN3);L298N_PORT&=~(1<<L298N_IN4);}
//#define MOT_RIGHT               {L298N_PORT|= (1<<L298N_IN4);L298N_PORT&=~(1<<L298N_IN3);}
//#define MOT_LR_STOP             {L298N_PORT&=~(1<<L298N_IN3);L298N_PORT&=~(1<<L298N_IN4);}


void l298n_init(void);


#endif
