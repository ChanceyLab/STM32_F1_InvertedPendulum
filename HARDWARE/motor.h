#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"

//正向反向控制端口
#define   AN0 PBout(4)
#define   AN1 PBout(15)
#define   EN  PAout(11)
void setmoto(int pwmx);
void setmoto2(int pwmx);
void MOTO_init(void);
void function(void);

#endif
