#include "ESP8266.h"
#include <stdint.h>
#include "usart.h"
#include <string.h>
#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "circle_buffer.h"

#define NETWORK_NAME "ESP_TEST"
#define NETWORK_PWD "123456789"
#define ESP_IP "192.168.137.98"
#define ESP_GATEWAY "192.168.137.1"
#define ESP_NETMASK "255.255.255.0"
#define CONNECT_IP "192.168.137.1"
#define ESP_PORT 9999
#define ESP_NUM 1

//uint16_t g_rx_datalen=0;
uint8_t flag_buf=0;

void Change_buf(uint8_t flag)
{
    flag_buf = flag;
}

uint8_t read_flag_buf(void)
{
    return flag_buf;
}

int sendATcmd(const char *cmd,const char *expectedRespond,uint16_t ms)
{
    uint8_t responod_buf[255]={0};
    uint16_t i=0;
    HAL_UART_Transmit(&huart1,(const uint8_t *)cmd,strlen(cmd),5);
    while(ms--)
    {
        HAL_Delay(1);       //delay 1ms
    }
    Waitfor_RX_COMPLT();

    while(g_buf.r!=g_buf.w)
    {
        circle_buf_read(&g_buf,&responod_buf[i++]);
    }
    #if DEBUG
    printf("receive data:%s\r\n",responod_buf);
    #endif // DEBUG

    
    if(strstr((const char *)responod_buf,expectedRespond)==NULL)
    {
        #if DEBUG
        printf("[ESP8266.c]:RX error\r\n");
        #endif
        return -1;
    }
    else 
    {   
        #if DEBUG
        printf("[ESP8266.c]:RX success!cmd:%s\r\n",cmd);
        #endif
        return 0;
    }
}


int ESP8266_Client_init(void)
{
    char cmd[256];
    HAL_UART_Transmit(&huart1,(const uint8_t *)"+++",3,2);
    HAL_Delay(100);
    HAL_UART_Transmit(&huart1,(const uint8_t *)"\r\n",2,2);
    //HAL_UART_Transmit(&huart1,(const uint8_t *)"AT+RESTORE\r\n",13,2);
    //HAL_Delay(3000);
    //UART1_IDLE_INIT();
    HAL_Delay(200);
    //sendATcmd("AT+RESTORE\r\n","OK",4000);
    if(sendATcmd("AT\r\n","OK",5)!=0)
    {
        return -1;
    }
    sendATcmd("AT+CWMODE=1\r\n","OK",10);
    //sprintf(cmd,"AT+CIPSTA=\"%s\",\"%s\",\"%s\"\r\n", ESP_IP, ESP_GATEWAY, ESP_NETMASK);
    //sendATcmd(cmd,"OK",200);
    //sendATcmd("AT+RST\r\n","OK",200);
    sprintf(cmd,"AT+CWJAP=\"%s\",\"%s\"\r\n", NETWORK_NAME, NETWORK_PWD);
    sendATcmd(cmd,"OK",3000);
    sprintf(cmd,"AT+CIPSTA=\"%s\",\"%s\",\"%s\"\r\n", ESP_IP, ESP_GATEWAY, ESP_NETMASK);
    sendATcmd(cmd,"OK",1000);
    sendATcmd("AT+CIPMUX=0\r\n","OK",10);

    sendATcmd("AT+CIPMODE=1\r\n","OK",10);
    //sprintf(cmd,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", CONNECT_IP, ESP_PORT);
    sprintf(cmd,"AT+CIPSTART=\"UDP\",\"%s\",%d,%d,2\r\n", CONNECT_IP, ESP_PORT,ESP_PORT);

    if(sendATcmd(cmd,"CONNECT",2000)!=0)
    {
        printf("Check the server port %d open or not!!!",ESP_PORT);
        return -1;
    }

    
    sendATcmd("AT+CIPSEND\r\n","OK",10);
    Change_buf(1);
    HAL_UART_Transmit(&huart1,"hello user!\r\n",strlen("hello user!\r\n"),1);
    return 0;
}
