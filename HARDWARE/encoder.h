#ifndef __ENCODER_H
#define __ENCODER_H
#include "sys.h"
#define count TIM3->CNT   //��ȡ����ֵ���ȼ���λ��

#define OneCycle 1560//��תһ�ܱ���������ֵ

//λ�û���Ŀ��ֵ������Ԫ����ü���ֵ��֮�Ƚϣ�ȷ������ȶ�ʱ�İڱ�λ��
#define count_exp 10000


void ENC_Init(void);
#endif
