#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define pwm 	 TIM2->CCR4        //ռ�ձ�ֵ
void TIM_PWM_Init(u16 arr,u16 psc);

#endif
