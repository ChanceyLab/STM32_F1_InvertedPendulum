/******************************************
led灯为单片机PD2口，用来调试程序用
*******************************************/
#include "led.h"

//led初始化函数
void LED_Init(void)
{ 
 GPIO_InitTypeDef  GPIO_InitStructure;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能PD端口时钟
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 					
}
 
