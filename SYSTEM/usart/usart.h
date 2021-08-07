#ifndef __USART_H
#define __USART_H
#include "sys.h" 

#define USART_REC_LEN  			20  	//�����������ֽ��� 20
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern char USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
void protocol(char*s); 
void uart3_init(u32 bound);
void prints(char *str);
void printc(char c);
void itoa(int num, char*str,int radix);
void printnum(int num);
#endif


