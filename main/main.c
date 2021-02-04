#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <sys/param.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include <sys/param.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "server.h"
#include "connect.h"
#include "control.h"
#include <pthread.h>

#define TAG "DATA"
#define NUMBER CONFIG_TEL_NUMBER

xSemaphoreHandle connectionSemaphore;

#define PORT 4533
#define BUF_SIZE (1024)
#define UART_TXD (GPIO_NUM_4)
#define UART_RXD (GPIO_NUM_2)


SemaphoreHandle_t  sem_threads = NULL; 



struct pt_args{
    int uart_num;
    double value_h;
    double value_v;
    //char dir;
};


void* thread_turn_h(void* args){

    struct pt_args* p = args;
    
    
    //if(p->dir=='h'){
        //xSemaphoreTake(sem_threads, 10000 / portTICK_RATE_MS);
        //turn_deg_h(p->uart_num, p->value_h);
        //xSemaphoreGive(sem_threads);
   // }
    //if(p->dir=='v'){
       // xSemaphoreTake(sem_threads, 10000 / portTICK_RATE_MS);
       // turn_deg_v(p->uart_num, p->value_v);
        //xSemaphoreGive(sem_threads);
    //}

    turn_deg_h(UART_NUM_1, p->value_h);
    turn_deg_v(UART_NUM_1, p->value_v);

    
    ESP_LOGE(TAG, "thread %f %f", p->value_h, p->value_v);
    

    return NULL;
}



static void do_retransmit(const int sock)
{
    int len;
    char rx_buffer[128];
    

    do {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0) {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        } else if (len == 0) {
            ESP_LOGW(TAG, "Connection closed");
        } else {
            rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
            //ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);
            double az, el;
            char* buf_pointer = rx_buffer;
            buf_pointer = str_replace(buf_pointer, ",", ".");
            //ESP_LOGI(TAG, "Received PP %s", rx_buffer);

            sscanf(buf_pointer, "P %lf %lf", &az, &el);
                   
            if(rx_buffer[0] == 'P'){

                pthread_t threads[2];
                
                ESP_LOGI(TAG, "Received P %f %f", az, el);
                
                struct pt_args args;
                args.uart_num = UART_NUM_1;
                args.value_h = az;
                args.value_v = el;
                ESP_LOGI(TAG, "pre create");
                pthread_create(&threads[0], NULL, thread_turn_h, (void*)&args);
/*
                struct pt_args args2;
                args2.uart_num = UART_NUM_1;
                args2.value_ = el;
                args2.dir = 'v';
                pthread_create(&threads[1], NULL, thread_turn_h, (void*)&args2);
                */
                //uart_write_bytes(UART_NUM_1, "r\n", 2);
                //turn_deg_h(UART_NUM_1, az);
                //turn_deg_v(UART_NUM_1, el);
                ESP_LOGI(TAG, "pre join");
                //pthread_join(threads[0], NULL);
                //pthread_join(threads[1], NULL);
                ESP_LOGI(TAG, "after join");

            }

            // send() can return less bytes than supplied length.
            // Walk-around for robust implementation. 
            int to_write = len;
            while (to_write > 0) {
                //azimuth, elevation
                char sent_data[128]; //float is 32bit
                printf("%f\n%f\n",get_radius_h(UART_NUM_1), get_radius_v(UART_NUM_1));

                int n = sprintf(sent_data, "%f\n%f\n",get_radius_h(UART_NUM_1), get_radius_v(UART_NUM_1));
                int written = send(sock, sent_data, n, 0);
                //int written = send(sock, "%f\n%f\n", strlen("0.0\n0.1\n"), 0);
                if (written < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                }
                else {
                    ESP_LOGE(TAG, "SENT %s", sent_data);
                }
                to_write -= written;
            }
        }
    } while (len > 0);
}

void OnConnected(void* para)
{

  while (true)
  {
    if (xSemaphoreTake(connectionSemaphore, 10000 / portTICK_RATE_MS))
    {
        RegisterEndPoints();

            
        xSemaphoreTake(connectionSemaphore, portMAX_DELAY);
    }
    else
    {
      ESP_LOGE(TAG, "Failed to connect. Retry in");
      for (int i = 0; i < 5; i++)
      {
        ESP_LOGE(TAG, "...%d", i);
        vTaskDelay(1000 / portTICK_RATE_MS);
      }
      esp_restart();
    }
  }
}

void Gpredict_server(void* par){
char addr_str[128];
    int addr_family;
    int ip_protocol;

    //sem_threads = xSemaphoreCreateBinary();
   
#ifdef CONFIG_EXAMPLE_IPV4
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else // IPV6
    struct sockaddr_in6 dest_addr;
    bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
    dest_addr.sin6_family = AF_INET6;
    dest_addr.sin6_port = htons(PORT);
    addr_family = AF_INET6;
    ip_protocol = IPPROTO_IPV6;
    inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1) {

        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Convert ip address to string
        if (source_addr.sin6_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
        } else if (source_addr.sin6_family == PF_INET6) {
            inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
        }
        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

        do_retransmit(sock);

        shutdown(sock, 0);
        close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

void app_main()
{
  UART_init();
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  connectionSemaphore = xSemaphoreCreateBinary();
  wifiInit();

  xTaskCreate(&OnConnected, "btsis", 1024 * 5, NULL, 5, NULL);
  xTaskCreate(&Gpredict_server, "btsis_gpredict", 1024 * 5, NULL, 5, NULL);

}