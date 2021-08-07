/******************************************
Ϊ����ͨ�����ļ��еĲ����������½���һ��variable.h
�ļ����������ó������õ��ĸ�������
*******************************************/
#ifndef __VARIABLE_H
#define __VARIABLE_H	
#include "stm32f10x.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"
#include "exti.h"
#include "pwm.h"
#include <stdlib.h>
#include "time2.h"
#include "adc.h"
#include "OLED_I2C.h"
#include "motor.h"
#include "control.h"
#include "encoder.h"



extern int speed,target,op1,op2,t1,t2,local_pos;
extern u16 angle0;
extern u16 kpa,kia,kda,kpv,kdv; 
extern u8 mark,mark0,func,func2,page,WriteOk;

#endif
