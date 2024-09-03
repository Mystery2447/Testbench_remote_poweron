#ifndef  _ESP8266_H
#define _ESP8266_H
#define DEBUG 1
#include <stdint.h>
int sendATcmd(const char *cmd,const char *expectedRespond,uint16_t ms);
int ESP8266_Client_init(void);
uint8_t read_flag_buf(void);
void Change_buf(uint8_t flag);
#endif // ! _ESP8266_H
