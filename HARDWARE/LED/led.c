/******************************************
led��Ϊ��Ƭ��PD2�ڣ��������Գ�����
*******************************************/
#include "led.h"

//led��ʼ������
void LED_Init(void)
{ 
 GPIO_InitTypeDef  GPIO_InitStructure;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PD�˿�ʱ��
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 					
}
 
