#ifndef __ENCODER_H
#define __ENCODER_H
#include "sys.h"
#define count TIM3->CNT   //读取计数值，等价于位置

#define OneCycle 1560//旋转一周编码器计数值

//位置环的目标值，霍尔元件测得计数值与之比较，确定最后稳定时的摆臂位置
#define count_exp 10000


void ENC_Init(void);
#endif
