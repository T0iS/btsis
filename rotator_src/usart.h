#ifndef USART_H
#define USART_H

#include "default.h"

#define BAUD 9600

#define RS422_DDR		DDRD
#define RS422_PORT              PORTD
#define RS422_EN_TX_PIN		PIND3
#define RS422_EN_RX_PIN		PIND2


void uart_init(void);
int uart_putchar(char c, FILE *stream);
void rs422_rx_enable(char val);
void rs422_tx_enable(char val);	

#endif
