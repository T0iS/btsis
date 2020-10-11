#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>

#include "usart.h"

#include <util/setbaud.h>


void uart_init(void) {
        UBRR0H = UBRRH_VALUE;
        UBRR0L = UBRRL_VALUE;

        #if USE_2X
        UCSR0A |= _BV(U2X0);
        #else
        UCSR0A &= ~(_BV(U2X0));
        #endif

        UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
        UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);   /* Enable RX, TX and RX interrupt */

	//set pin as output
	RS422_DDR = (1<<RS422_EN_TX_PIN) | (1<<RS422_EN_RX_PIN);
}

void rs422_rx_enable(char val){
	if(val)
		RS422_PORT &=~(1<<RS422_EN_RX_PIN);
	else
		RS422_PORT |=(1<<RS422_EN_RX_PIN);
}

void rs422_tx_enable(char val){
	if(val)
		RS422_PORT |=(1<<RS422_EN_TX_PIN);
        else
		RS422_PORT &=~(1<<RS422_EN_TX_PIN);
}

int uart_putchar(char c, FILE *stream) {
        if (c == '\n') {
                uart_putchar('\r', stream);
        }
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
        return 0;
}


