/******************************************
在定时器5中断函数中进行pid调节，中断为5ms
控制系统核心PID调节
角度环：主要的pid调节过程，读取电位器电压值获取当前角度，进行pid调节
位置环：进行辅助调节，防止系统发散
void pid(void)    --倒立平衡时的pid调节
void pid_c(void)  --功能2中旋转一圈时用的pid调节，去除了位置环的作用
*******************************************/
#include <stdlib.h>
#include "control.h"
#include "adc.h"
#include "encoder.h"
#include "led.h"
#include "variable.h"

u16 angle0, angle1;            //依次为当前角度，上次角度，上次位置
int all_pwm, target_a;                  //依次为耦合后的pwm值，修正后的预期值
int pos0, pos1, speed, pos_pwm, pos_angle; //位置，速度，位置积分，位置pid，位置偶合成的角度值
int ap_error, ad_error, ai_error;              //角度差，角加速度
int nu = 0;                           //计数积分次数
u8 func2 = 0; //功能2的标志位
//整体思路：通过将位置pid得出的参数整合到角度中实现与角度环的耦合
void pid(void)
{
    angle0 = Get_Adc_Average(2, 12);      //读出角度值

    if(count > count_exp + 500) count = count_exp;  //限幅
    else if(count < count_exp - 500)	count = count_exp;
    pos0 = count - count_exp;            //读出当前位置差 pos0
    speed = pos0 - pos1;             //位置的微分 即速度
    pos_pwm = kpv * pos0 + kdv * speed; //位置pid
    pos_angle = pos_pwm / 32;           //整合成角度
    if(pos_angle > 160) pos_angle = 160;    //对整合值限幅
    else if(pos_angle < -160) pos_angle = -160;
    target_a = target - pos_angle;      //产生新的预期值

    ap_error = target_a - angle0;         //耦合后角度差
    ad_error = angle0 - angle1;	          //角度微分

    if(abs(target - angle0) < 300)
    {

        ai_error = ai_error + target - angle0; //角度积分
        all_pwm = kpa * ap_error + kia * ai_error / 100 - kda * ad_error; //耦合后的pid调节
    }
    else  all_pwm = 0;
    angle1 = angle0;
    pos1 = pos0;             //锁值
    nu++;                                 //给积分项限制次数，防止无限大
    if(nu == 200) ai_error = 0, nu = 0;
}

void TIM5_IRQHandler(void)              //定时器5中断函数
{
    if(TIM5->SR & 0X0001) //溢出中断
    {
        pid();
        setmoto(all_pwm);                //设置pwm
    }
    TIM5->SR &= ~(1 << 0);
}

/*直立环部分*/
u16 Angle;	//当前角度
float AP = 44;
float AD = 220;
float  Perror = 0;

/*速度环部分*/
float SP = 120.0f, SI = 0.0f; //速度环的比例和微分
float Speed_Integral = 0; //速度环的积分量
s16 SpeedTarget = 15; //速度的预期值，越大旋转速度越快
u16 count0 = 10000;
/*左右轮速度*/
s16 Speed;

/*速度环*/
s16 SpeedPID(s16 Speed)
{
    float VPwm, SpDelta, CurrentSpeed = 0;
    //VPwm-PWM输出，SpDelta-速度变化量，CurrentSpeed-速度
    static float CurrentSpeed_Pre = 0; //记录前一次的速度的值

    /*低通*/
    CurrentSpeed = Speed;
    CurrentSpeed = (float)(CurrentSpeed_Pre * 0.2f + CurrentSpeed * 0.8f);
    CurrentSpeed_Pre = CurrentSpeed;

    SpDelta = SpeedTarget - CurrentSpeed; //目标速度-当前速度

    Speed_Integral += SpDelta; //积分量累加
    /*积分上限设限*/
    if(abs(Speed_Integral) > 20000) {
        Speed_Integral -= SpDelta;
    }

    VPwm = SpDelta * SP + Speed_Integral * SI; //积分方向已经确认正确
    return VPwm;
}

/*直立环*/
s16 AnglePID(void)
{
    static u16 Angle_Last = 0; //上次的角度
    s16 B_PWM;

    float Derror = 0;

    Angle = Get_Adc_Average(2, 12); //获取当前角度

    Perror = target - Angle;	//比例部分
    Derror = Angle_Last - Angle; //微分部分
    Angle_Last = Angle;

    if(abs(Perror) < 500) //判断有没有倒下，倒下归零
        B_PWM = (s16)(AP * Perror + AD * Derror);
    else
        B_PWM = 0;

    return B_PWM;
}



void TIM6_IRQHandler(void)              //定时器5中断函数
{
    static u8 SpCount = 0, LedCount = 0; //SpCount速度环调节计数，LedCount状态灯调节计数
    static s16 Angle_Pwm, Speed_Pwm; //角度环和速度环PWM输出

    if(TIM6->SR & 0X0001) //溢出中断
    {
        Angle_Pwm = AnglePID();//直立环

        LedCount++;

        SpCount++;
        if(SpCount > 10) //较长时间调一次速度环
        {
            SpCount = 0; //
            Speed = count - count0; //当前速度
            count0 = count; //记录上次的位置
            Speed_Pwm = SpeedPID(Speed); //速度环
        }

        setmoto(Angle_Pwm + Speed_Pwm);

        if(LedCount > 200) LedCount = 0, LED = ~LED;


    }
    TIM6->SR &= ~(1 << 0); //定时器标志清零
}


