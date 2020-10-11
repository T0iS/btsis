#ifndef AS5040_H
#define AS5040_H

#include <avr/io.h>

#define ROT_DATA_INPUT0 	PINA0
#define ROT_DATA_INPUT1 	PINA3
#define ROT_CLK                 PINA1
#define ROT_CS0                 PINA2
#define ROT_CS1                 PINA4

#define ROT_DDR			DDRA
#define ROT_PORT		PORTA
#define ROT_PIN                	PINA

typedef struct AS5040{
        int par : 1;
        int mag_dec : 1;
        int mag_inc : 1;
        int lin : 1;
        int cof : 1;
        int ocf : 1;
        unsigned int angle : 10;
} AS5040;

void  as5040_init(void);
void  as5040_read(AS5040 *as_vert, AS5040 *as_hor);
unsigned int as5040_bit2angle(unsigned int angle);
void calibrate(AS5040 *as_ver,AS5040 *as_hor);
unsigned int get_hor_angle(AS5040 * as_hor);
unsigned int get_ver_angle(AS5040 * as_ver);
int angle_len(unsigned int start, unsigned int finish);

#endif
