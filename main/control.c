#include <stdio.h>
#include "driver/uart.h"
#include <math.h>
#include "control.h"


void turn_deg_h (uart_port_t uart_num, int h_dest_deg){

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int temp1, temp2;
    float h_deg, v_deg = -1;
    int len;
    while((len = uart_read_bytes(uart_num, data, BUF_SIZE, 100 / portTICK_RATE_MS)) <=66);

    
    sscanf((int*)data, "%*[^H]H:%4d(%f) V:%4d(%f)", &temp1, &h_deg, &temp2, &v_deg); 

    

    char* d = "";
    if((h_deg < h_dest_deg)){
        d = "d\n"; //zvysuje se
    }
    else d = "u\n"; // snizuje

       

    while (true) {
        //Read data from the UART
        len = uart_read_bytes(uart_num, data, BUF_SIZE, 100 / portTICK_RATE_MS);

        sscanf((int*)data, "%*[^H]H:%4d(%f) V:%4d(%f)", &temp1, &h_deg, &temp2, &v_deg);
        
        
        if(abs(h_deg - h_dest_deg) > 2){
            uart_write_bytes(uart_num, (const char *) d, 2);
        }
        else {
            uart_write_bytes(uart_num, (const char *) " \n", 2);
            free(data);
            break;
        }
        printf("%f %f \n\n",h_deg, v_deg);

    }
    
    
}

void turn_deg_v (uart_port_t uart_num, int v_dest_deg){

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int temp1, temp2;
    float h_deg, v_deg = -1;
    
    int len;
    while((len = uart_read_bytes(uart_num, data, BUF_SIZE, 100 / portTICK_RATE_MS)) <=66);

    sscanf((int*)data, "%*[^H]H:%4d(%f) V:%4d(%f)", &temp1, &h_deg, &temp2, &v_deg);
       
    char* d = "";
    if ((abs(v_deg - v_dest_deg)) < (360-(abs(v_deg - v_dest_deg)))){
        d = "r\n";
    }
    else d = "l\n";
    

    while (true) {
        //Read data from the UART
        int len = uart_read_bytes(uart_num, data, BUF_SIZE, 100 / portTICK_RATE_MS);

        sscanf((int*)data, "%*[^H]H:%4d(%f) V:%4d(%f)", &temp1, &h_deg, &temp2, &v_deg);
       

        if(abs(v_deg - v_dest_deg) > 2){
            uart_write_bytes(uart_num, (const char *) d, 2);
        }
        else {
            uart_write_bytes(uart_num, (const char *) " \n", 2);
            free(data);
            break;
        }

        printf("%f %f \n\n",h_deg, v_deg);

    }
    
}