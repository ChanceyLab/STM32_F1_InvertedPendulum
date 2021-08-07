/******************************************
电机控制端口定义
对应TB6612控制端口
AN0--PB4
AN1--PB15
EN--PA11
void setmoto(int pwmx) 根据输入值正负改变电机转向及转速函数
void function(void)  功能定义函数
func1  --起摆并倒立
func2  --在平衡状态下，倒立摆转过360度，然后稳定
*******************************************/
#include "motor.h"
#include "variable.h"
#include  "stmflash.h"
extern float Speed_Integral;
extern u16 count0;
int local_pos;
//初始化电机转向控制口
void MOTO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //使能GPIO外设时钟使能


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;         //stby使能端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    AN1=AN0=0;
}

//设定pwm值
//根据正负判断方向
void setmoto(int pwmx)
{
    if (pwmx<0)
    {
        pwmx=-pwmx;
        AN1=1;
        AN0=0;
    }
    else
    {
        AN1=0;
        AN0=1;
    }
    if(pwmx>7200)
        pwm=7200;
    else
        pwm=pwmx;
}

void setmoto2(int pwmx)
{
    if (pwmx<0)
    {
        pwmx=-pwmx;
        AN1=1;
        AN0=0;
    }
    else
    {
        AN1=0;
        AN0=1;
    }
    if(pwmx>3000)
        pwm=3000;
    else
        pwm=pwmx;
}


void function(void)//倒立摆中扩展功能1（起摆），功能2（摆臂旋转1圈）实现函数
{

    if(WriteOk==1)
        OLED_ShowCN(111,0,0);
    else
        OLED_ShowCN(111,0,1);


    if(func2==1)  //功能2，旋转一周标志
    {
        if(abs(count-local_pos)>OneCycle||	abs(target-angle0)>300)//判断是否旋转一周位置
        {
            count=count_exp;   //让定时器5的PID调节能稳定这个位置
            TIM_Cmd(TIM6,DISABLE);
            Speed_Integral=0;//积分清零

            TIM_Cmd(TIM5,ENABLE);
            LED=~LED;
            func2=0;
        }
    }
    switch(func)
    {
    case(1): {
        EN=0;
        TIM_Cmd(TIM5, DISABLE);
        delay_ms(1000);
        EN=1;
        setmoto(-op1);
        delay_ms(t1);
        setmoto(op2);
        delay_ms(t2);
        TIM_Cmd(TIM5,ENABLE);
        func=0;
        break;
    }

    case(2): {
        TIM_Cmd(TIM5, DISABLE);
        TIM_Cmd(TIM6,ENABLE);
        count=10000;  //位置归位
        count0=10000;
        local_pos=count;    //记录当前位置，作为旋转360度的初值

        func=0;
        func2=1;
        break;
    }

    case(3): {
        target=angle0;
        Write_HByte(addr,target);
        WriteOk=1;
        func=0;
        break;
    }

    case(4):
        func=0;
        break;

    default:
        break;
    }
}
