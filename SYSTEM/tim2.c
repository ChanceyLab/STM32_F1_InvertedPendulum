/******************************************
定时器中断
定时器5  产生5ms中断用来做平衡pid调节
定时器6  产生5ms中断用来做pid_c调节，完成func2圆周运动
*******************************************/
#include "time2.h"

//定时器5 的相关设定
void TIM5_init(u16 arr,u16 psc)
{
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 

TIM_TimeBaseStructure.TIM_Period = arr;
TIM_TimeBaseStructure.TIM_Prescaler =psc; 
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); 
//优先级设定
NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
NVIC_Init(&NVIC_InitStructure); 
TIM_Cmd(TIM5, ENABLE); 
}

void TIM6_init(u16 arr,u16 psc)
{
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); 

TIM_TimeBaseStructure.TIM_Period = arr;
TIM_TimeBaseStructure.TIM_Prescaler =psc; 
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); 
//优先级设定
NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
NVIC_Init(&NVIC_InitStructure); 
TIM_Cmd(TIM6, ENABLE); 
}

