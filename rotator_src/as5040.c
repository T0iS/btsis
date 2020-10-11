#include "default.h"
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include <avr/eeprom.h>
#include "as5040.h"

void as5040_init(void)
{
        ROT_DDR |=(1<<ROT_CLK)|(1<<ROT_CS0)|(1<<ROT_CS1);
        ROT_DDR &=~((1<<ROT_DATA_INPUT1)|(1<<ROT_DATA_INPUT0));	

        ROT_PORT |=  (1<<ROT_DATA_INPUT0);
        ROT_PORT |=  (1<<ROT_DATA_INPUT1);

        ROT_PORT |=  (1<<ROT_CLK);_delay_ms(1);
        ROT_PORT |=  (1<<ROT_CS0);_delay_ms(1);
        ROT_PORT |=  (1<<ROT_CS1);_delay_ms(1);

}

void as5040_read(AS5040 *as_vert,AS5040 *as_hor)
{
	#define AS_DELAY	1

	unsigned int data0 = 0;
	unsigned int data1 = 0;

        ROT_PORT &= ~(1<<ROT_CS0);_delay_ms(AS_DELAY);
        ROT_PORT &= ~(1<<ROT_CS1);_delay_ms(AS_DELAY);
        for(char i=0;i<16;i++)
        {
		ROT_PORT &= ~(1<<ROT_CLK);_delay_ms(AS_DELAY);
                ROT_PORT |=  (1<<ROT_CLK);_delay_ms(AS_DELAY);
                if(ROT_PIN & (1<<ROT_DATA_INPUT0)) data0 |=(1<<(15-i));
                if(ROT_PIN & (1<<ROT_DATA_INPUT1)) data1 |=(1<<(15-i));
	}
        ROT_PORT |=  (1<<ROT_CS0);_delay_ms(AS_DELAY);
        ROT_PORT |=  (1<<ROT_CS1);_delay_ms(AS_DELAY);

	memcpy(as_vert,(AS5040*)&data1,sizeof(AS5040));
	memcpy(as_hor,(AS5040*)&data0,sizeof(AS5040));
}


unsigned int as5040_bit2angle(unsigned int angle)
{
	return angle*3.515625;
}


void calibrate(AS5040 *as_ver,AS5040 *as_hor)
{
	eeprom_write_word((uint16_t *)EEPROM_VER_OFSET_ADR,(uint16_t)(as_ver->angle));
	eeprom_write_word((uint16_t *)EEPROM_HOR_OFSET_ADR,(uint16_t)(as_hor->angle));
}


unsigned int get_hor_angle(AS5040 * as_hor)
{
	uint16_t ofset = eeprom_read_word((uint16_t *)EEPROM_HOR_OFSET_ADR);
	if((uint16_t)as_hor->angle >= ofset ) 
		return (uint16_t)as_hor->angle - ofset;
	else
		return (uint16_t)as_hor->angle + (0x3FF - ofset + 1);
	


}

unsigned int get_ver_angle(AS5040 * as_ver)
{
	uint16_t ofset = eeprom_read_word((uint16_t *)EEPROM_VER_OFSET_ADR);
        if((uint16_t)as_ver->angle >= ofset )
                return (uint16_t)as_ver->angle - ofset;
        else
                return (uint16_t)as_ver->angle + (0x3FF - ofset + 1);
}


int angle_len(unsigned int start, unsigned int finish)
{
	unsigned int len_p = abs((int)start-(int)finish);
	unsigned int len_d = 3600 - len_p;

	unsigned int len = 0;
	if(len_p<len_d) len = len_p;
	else len = len_d;

	if( ((start<finish)&&(len_p>len_d)) || ((start>finish)&&(len_p<len_d )))
	   return -len;
	else
	   return len;
}

