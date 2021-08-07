#ifndef __USART_H
#define __USART_H
#include "sys.h" 

#define USART_REC_LEN  			20  	//定义最大接收字节数 20
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern char USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
void protocol(char*s); 
void uart3_init(u32 bound);
void prints(char *str);
void printc(char c);
void itoa(int num, char*str,int radix);
void printnum(int num);
#endif


