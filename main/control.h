#ifndef _CONTROL_H
#define _CONTROL_H


#define BUF_SIZE (1024)

void turn_deg_h (uart_port_t uart_num, float h_dest_deg);
void turn_deg_v (uart_port_t uart_num, float v_dest_deg);
float get_radius_h(uart_port_t uart_num);
float get_radius_v(uart_port_t uart_num);    



#endif