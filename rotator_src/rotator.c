#include "default.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "usart.h"
#include "as5040.h"
#include "l298n.h"

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);

#define INPUT_SIZE	80
char input[INPUT_SIZE];
unsigned char input_cnt=0;

unsigned int	set_ver_position=0;
unsigned int    set_hor_position=0;

char P_input[80];
bool b_calibrate = false;
bool b_pFlag = false;
bool b_Pllllluu        lnvadnlxcm,nbyxvcljkdfgiluerwtiorretertertertdfv     dfsgdfgsdfgdfgsdfgsdfgFlag = false;
bool b_SFlag = false;

#define MODE_HAMLIB	1
#define MDDE_MANUAL	2
#define MODE_HALFMANUAL	3
#define MODE_DEBUG	4
char mode = MDDE_MANUAL;

void rx_process(char * in)
{
	//printf("%s\n",input);
	//for(int i=0;input[i];i++)printf("%c[%d]",input[i],i);

	switch (mode)
        {
	  case MDDE_MANUAL:
	  case MODE_DEBUG:
		if(!strncmp("hamlib",in,6))      mode = MODE_HAMLIB;
		if(!strncmp("halfmanual",in,10)) mode = MODE_HALFMANUAL;
		if(!strncmp("debug",in,5))       mode = MODE_DEBUG;
		if(!strncmp("manual",in,6))      mode = MDDE_MANUAL;

		if(!strncmp("calibrate",in,9)) b_calibrate=true;
		if(in[0]=='r') MOT_RIGHT;
		if(in[0]=='l') MOT_LEFT;
        	if(in[0]=='u') MOT_UP;
        	if(in[0]=='d') MOT_DOWN;
		if(in[0]==' ') { MOT_UD_STOP; MOT_LR_STOP; }
	  break;
          case MODE_HALFMANUAL:
                if(!strncmp("hamlib",in,6))      mode = MODE_HAMLIB;
                if(!strncmp("halfmanual",in,10)) mode = MODE_HALFMANUAL;
                if(!strncmp("debug",in,5))       mode = MODE_DEBUG;
                if(!strncmp("manual",in,6))      mode = MDDE_MANUAL;

		if(!strncmp("V:",in,2))
		{
			set_ver_position=atoi(&in[2]);
		}
		if(!strncmp("H:",in,2))
		{
			set_hor_position=atoi(&in[2]);
		}
          break;
	  case MODE_HAMLIB:
                if(!strncmp("hamlib",in,6))      mode = MODE_HAMLIB;
                if(!strncmp("halfmanual",in,10)) mode = MODE_HALFMANUAL;
                if(!strncmp("debug",in,5))       mode = MODE_DEBUG;
                if(!strncmp("manual",in,6))      mode = MDDE_MANUAL;

		if(in[0]=='p') b_pFlag = true;
		if(in[0]=='S') b_SFlag = true;
		if(in[0]=='P')
		{
		  b_PFlag = true;
		  strcpy(P_input,in);
		} 
		if(in[0]=='K')
		{
		  set_ver_position=0;
                  set_hor_position=0;
		}
	  break;
	}
}

ISR ( USART0_RX_vect )
{
	cli();
	char c = UDR0;

	input[input_cnt++] = c;
	if(input_cnt>=INPUT_SIZE)
	{
		input[INPUT_SIZE-1]='\0';
		rx_process(input);
		input_cnt=0;
	}
	else if( c=='\r' && input_cnt==1)
	{
		input_cnt=0;
	}
	else if( c=='\n' && input_cnt==1)
        {
                input_cnt=0;
        }
	else if( c=='\r' || c=='\n' )
	{
		input[input_cnt]='\0';
		rx_process(input);
		input_cnt=0;
	}

	sei();
}


int main(void)
{
	cli();	

	uart_init();
	rs422_rx_enable(true);
	rs422_tx_enable(true);

	stdout = &mystdout;

	printf("\n\n\n");
	printf(" mmmmm   mmmm mmmmmmm   mm  mmmmmmm  mmmm  mmmmm  \n");
	printf(" #   '# m'  'm   #      ##     #    m'  'm #   '# \n");
	printf(" #mmmm' #    #   #     #  #    #    #    # #mmmm' \n");
	printf(" #   'm #    #   #     #mm#    #    #    # #   'm \n");
	printf(" #    '  #mm#    #    #    #   #     #mm#  #    ' \n");

	printf("version:0.2\n\n");

	l298n_init();

	as5040_init();

	AS5040 as_ver;
        AS5040 as_hor;

	sei();	

	while(1)
	{
		as5040_read(&as_ver, &as_hor);

		unsigned int ver_raw = (unsigned int)as_ver.angle;
		unsigned int hor_raw = (unsigned int)as_hor.angle;

		unsigned int angle_ver_raw = as5040_bit2angle(ver_raw);
		unsigned int angle_hor_raw = as5040_bit2angle(hor_raw);

		unsigned int correcr_ver_raw = get_ver_angle(&as_ver);
		unsigned int correcr_hor_raw = get_hor_angle(&as_hor);

		unsigned int correcr_ver = as5040_bit2angle(correcr_ver_raw);
		unsigned int correcr_hor = as5040_bit2angle(correcr_hor_raw);

		int v_len = angle_len(correcr_ver,set_ver_position);
                int h_len = angle_len(correcr_hor,set_hor_position);

		switch (mode)
		{
		  case MDDE_MANUAL:
			printf("Manual: ");
			printf("h:%04d(%04d.%d) ",hor_raw,angle_hor_raw/10,angle_hor_raw%10);
                        printf("v:%04d(%04d.%d) ",ver_raw,angle_ver_raw/10,angle_ver_raw%10);
			printf("H:%04d(%04d.%d) ",correcr_hor_raw,correcr_hor/10,correcr_hor%10);
                        printf("V:%04d(%04d.%d) ",correcr_ver_raw,correcr_ver/10,correcr_ver%10);
                        printf("\r");

			if(b_calibrate)
                	{
                        	b_calibrate=false;
                        	as5040_read(&as_ver, &as_hor);
                        	calibrate(&as_ver, &as_hor);
                	}
		  break;
		  case MODE_DEBUG:
			printf("Debug: ");
			printf("V: p:%d,md:%d,mi:%d,li:%d,co:%d,oc:%d,angle:%04d ",
			  as_ver.par&1,as_ver.mag_dec&1,as_ver.mag_inc&1,as_ver.lin&1,
			  as_ver.cof&1,as_ver.ocf&1,as_ver.angle);
			printf("H: p:%d,md:%d,mi:%d,li:%d,co:%d,oc:%d,angle:%04d ",
			  as_hor.par&1,as_hor.mag_dec&1,as_hor.mag_inc&1,as_hor.lin&1,
			  as_hor.cof&1,as_hor.ocf&1,as_hor.angle);
			printf("         \r");
		  break;
		  case MODE_HALFMANUAL:
			printf("Half manual: ");

                        printf("H:%04d.%d(%04d.%d) Hlen:%04d ",
			  set_hor_position/10,set_hor_position%10,correcr_hor/10,correcr_hor%10,h_len);
                        printf("V:%04d.%d(%04d.%d) Vlen:%04d ",
			  set_ver_position/10,set_ver_position%10,correcr_ver/10,correcr_ver%10,v_len);
                        printf("                              \r");

                        if(v_len<-8)      {MOT_UP;     }
                        else if(v_len>8)  {MOT_DOWN;   }
                        else              {MOT_UD_STOP;}

                        if(h_len<-12)     {MOT_LEFT;   }
                        else if(h_len>12) {MOT_RIGHT;  }
                        else              {MOT_LR_STOP;}

		  break;
		  case MODE_HAMLIB:
		        if(v_len<-8)      {MOT_UP;     }
                        else if(v_len>8)  {MOT_DOWN;   }
                        else              {MOT_UD_STOP;}

                        if(h_len<-12)     {MOT_LEFT;   }
                        else if(h_len>12) {MOT_RIGHT;  }
                        else              {MOT_LR_STOP;}

			if(b_pFlag == true)
			{
			  b_pFlag = false;
			  printf("%d.%d\n%d.%d\n",
			    correcr_hor/10,correcr_hor%10,correcr_ver/10,correcr_ver%10);
			}
			if(b_SFlag == true)
			{
			  b_SFlag = false;
			  set_ver_position = correcr_ver;
			  set_hor_position = correcr_hor;
			}

			if(b_PFlag == true)
			{
			  b_PFlag = false;
                  	  float f_hor,f_ver;
                  	  sscanf(P_input,"%*c %f %f",&f_hor,&f_ver);
			  P_input[0]='\0';

                  	  if( f_hor>=0.0 && f_hor<=360.0 && f_ver>=0.0 && f_ver<=90.0  )
                  	  {
                    	    set_ver_position=(unsigned int)(f_ver*10.0);
                    	    set_hor_position=(unsigned int)(f_hor*10.0);
                    	    printf("RPRT 0\n");
                  	  }
                  	  else
                    	    printf("RPRT -1\n");
			}

		  break;
		}

	}

	return 0;
}
