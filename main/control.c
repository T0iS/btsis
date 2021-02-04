#include <stdio.h>
#include "driver/uart.h"
#include <string.h>
#include <math.h>
#include "control.h"


float absolute(float value) {
  if (value < 0)
    return -value;
  return value;  
}

void turn_deg_v (uart_port_t uart_num, float h_dest_deg){

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int temp1, temp2;
    float h_deg, v_deg = -1;
    int len;
    while((len = uart_read_bytes(uart_num, data, BUF_SIZE, 100 / portTICK_RATE_MS)) <= 66);

    
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

void turn_deg_h (uart_port_t uart_num, float v_dest_deg){

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int temp1, temp2;
    float h_deg, v_deg = -1;
    
    int len;
    while((len = uart_read_bytes(uart_num, data, BUF_SIZE, 100 / portTICK_RATE_MS)) <= 66);

    sscanf((int*)data, "%*[^H]H:%4d(%f) V:%4d(%f)", &temp1, &h_deg, &temp2, &v_deg);
       
    char* d = "";

    if (v_dest_deg > v_deg && (v_dest_deg-v_deg) <= 180){
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

float get_radius_h(uart_port_t uart_num){

    int temp, len;
    float v_deg;
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    while((len = uart_read_bytes(uart_num, data, BUF_SIZE, 100 / portTICK_RATE_MS)) <= 66);

    sscanf((int*)data, "%*[^V]V:%4d(%f)", &temp, &v_deg);

    free(data);
    return v_deg; 

}

float get_radius_v(uart_port_t uart_num){

    int temp, len;
    float h_deg;
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    while((len = uart_read_bytes(uart_num, data, BUF_SIZE, 100 / portTICK_RATE_MS)) <= 66);

    sscanf((int*)data, "%*[^H]H:%4d(%f)", &temp, &h_deg);

    free(data);
    return h_deg;
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = (char*) malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}
    
