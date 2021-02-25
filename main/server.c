#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "esp_spiffs.h"
#include "driver/uart.h"
#include "driver/gpio.h"


#define TAG "SERVER"

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_2)

bool GPREDICT_ALLOWED = false;



void UART_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}


static esp_err_t on_url_hit(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    esp_vfs_spiffs_register(&config);

    char path[600];

    if(strcmp(req->uri, "/") == 0){
        strcpy(path, "/spiffs/index.html");
    }
    else{
        sprintf(path, "/spiffs%s", req->uri);
    }

    FILE *file = fopen(path, "r");

    if (file == NULL){
        httpd_resp_send_404(req);
    }
        else{
        char lineR[256];
        while (fgets(lineR, sizeof(lineR), file)){
            httpd_resp_sendstr_chunk(req, lineR);
        }
        httpd_resp_sendstr_chunk(req, NULL);
    }
    esp_vfs_spiffs_unregister(NULL);
    return ESP_OK;
}

static esp_err_t on_hello(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char *message = "{\"hello\":22}";
    httpd_resp_send(req, message, strlen(message));
    httpd_resp_set_status(req, "204 NO CONTENT");
    return ESP_OK;
    
}

static esp_err_t on_dir_set(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[150];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    cJSON *rcv = cJSON_Parse(buf);
    cJSON *direction = cJSON_GetObjectItem(rcv, "direction"); 
    char *str = cJSON_GetStringValue(direction); 
    
    //extending the command by space, no null termination needed
    char *newstr = malloc(strlen(str) + 1);
    strcpy(newstr, str);
    strcat(newstr, "\n");
    ESP_LOGI(TAG, "%s", newstr);

    //serial connection
    uart_write_bytes(UART_NUM_1, (const char *)newstr, 2);


    cJSON_Delete(rcv);
    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t gpredict_enable(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[150];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    cJSON *rcv = cJSON_Parse(buf);
    cJSON *enable_gpredict = cJSON_GetObjectItem(rcv, "gpredict"); 
    char *str = cJSON_GetStringValue(enable_gpredict); 
    
    if(strcmp(str, (const char*)"true")){
        GPREDICT_ALLOWED = true;    
    }
    else{
        GPREDICT_ALLOWED = false;
    }


    cJSON_Delete(rcv);
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
    
}

void RegisterEndPoints(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "starting server");
    if (httpd_start(&server, &config) != ESP_OK)
    {
        ESP_LOGE(TAG, "COULD NOT START SERVER");
    }

    

    httpd_uri_t hello_end_point_config = {
        .uri = "/hello",
        .method = HTTP_GET,
        .handler = on_hello};
    httpd_register_uri_handler(server, &hello_end_point_config);

    httpd_uri_t dir_end_point_config = {
        .uri = "/dir",
        .method = HTTP_POST,
        .handler = on_dir_set};
    httpd_register_uri_handler(server, &dir_end_point_config);

    httpd_uri_t gpredict_end_point_config = {
        .uri = "/gpredict_enable",
        .method = HTTP_GET,
        .handler = gpredict_enable};
    httpd_register_uri_handler(server, &gpredict_end_point_config);

    httpd_uri_t first_end_point_config = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_url_hit};
    httpd_register_uri_handler(server, &first_end_point_config);

}