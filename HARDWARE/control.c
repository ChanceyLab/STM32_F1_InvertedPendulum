/******************************************
�ڶ�ʱ��5�жϺ����н���pid���ڣ��ж�Ϊ5ms
����ϵͳ����PID����
�ǶȻ�����Ҫ��pid���ڹ��̣���ȡ��λ����ѹֵ��ȡ��ǰ�Ƕȣ�����pid����
λ�û������и������ڣ���ֹϵͳ��ɢ
void pid(void)    --����ƽ��ʱ��pid����
void pid_c(void)  --����2����תһȦʱ�õ�pid���ڣ�ȥ����λ�û�������
*******************************************/
#include <stdlib.h>
#include "control.h"
#include "adc.h"
#include "encoder.h"
#include "led.h"
#include "variable.h"

u16 angle0, angle1;            //����Ϊ��ǰ�Ƕȣ��ϴνǶȣ��ϴ�λ��
int all_pwm, target_a;                  //����Ϊ��Ϻ��pwmֵ���������Ԥ��ֵ
int pos0, pos1, speed, pos_pwm, pos_angle; //λ�ã��ٶȣ�λ�û��֣�λ��pid��λ��ż�ϳɵĽǶ�ֵ
int ap_error, ad_error, ai_error;              //�ǶȲ�Ǽ��ٶ�
int nu = 0;                           //�������ִ���
u8 func2 = 0; //����2�ı�־λ
//����˼·��ͨ����λ��pid�ó��Ĳ������ϵ��Ƕ���ʵ����ǶȻ������
void pid(void)
{
    angle0 = Get_Adc_Average(2, 12);      //�����Ƕ�ֵ

    if(count > count_exp + 500) count = count_exp;  //�޷�
    else if(count < count_exp - 500)	count = count_exp;
    pos0 = count - count_exp;            //������ǰλ�ò� pos0
    speed = pos0 - pos1;             //λ�õ�΢�� ���ٶ�
    pos_pwm = kpv * pos0 + kdv * speed; //λ��pid
    pos_angle = pos_pwm / 32;           //���ϳɽǶ�
    if(pos_angle > 160) pos_angle = 160;    //������ֵ�޷�
    else if(pos_angle < -160) pos_angle = -160;
    target_a = target - pos_angle;      //�����µ�Ԥ��ֵ

    ap_error = target_a - angle0;         //��Ϻ�ǶȲ�
    ad_error = angle0 - angle1;	          //�Ƕ�΢��

    if(abs(target - angle0) < 300)
    {

        ai_error = ai_error + target - angle0; //�ǶȻ���
        all_pwm = kpa * ap_error + kia * ai_error / 100 - kda * ad_error; //��Ϻ��pid����
    }
    else  all_pwm = 0;
    angle1 = angle0;
    pos1 = pos0;             //��ֵ
    nu++;                                 //�����������ƴ�������ֹ���޴�
    if(nu == 200) ai_error = 0, nu = 0;
}

void TIM5_IRQHandler(void)              //��ʱ��5�жϺ���
{
    if(TIM5->SR & 0X0001) //����ж�
    {
        pid();
        setmoto(all_pwm);                //����pwm
    }
    TIM5->SR &= ~(1 << 0);
}

/*ֱ��������*/
u16 Angle;	//��ǰ�Ƕ�
float AP = 44;
float AD = 220;
float  Perror = 0;

/*�ٶȻ�����*/
float SP = 120.0f, SI = 0.0f; //�ٶȻ��ı�����΢��
float Speed_Integral = 0; //�ٶȻ��Ļ�����
s16 SpeedTarget = 15; //�ٶȵ�Ԥ��ֵ��Խ����ת�ٶ�Խ��
u16 count0 = 10000;
/*�������ٶ�*/
s16 Speed;

/*�ٶȻ�*/
s16 SpeedPID(s16 Speed)
{
    float VPwm, SpDelta, CurrentSpeed = 0;
    //VPwm-PWM�����SpDelta-�ٶȱ仯����CurrentSpeed-�ٶ�
    static float CurrentSpeed_Pre = 0; //��¼ǰһ�ε��ٶȵ�ֵ

    /*��ͨ*/
    CurrentSpeed = Speed;
    CurrentSpeed = (float)(CurrentSpeed_Pre * 0.2f + CurrentSpeed * 0.8f);
    CurrentSpeed_Pre = CurrentSpeed;

    SpDelta = SpeedTarget - CurrentSpeed; //Ŀ���ٶ�-��ǰ�ٶ�

    Speed_Integral += SpDelta; //�������ۼ�
    /*������������*/
    if(abs(Speed_Integral) > 20000) {
        Speed_Integral -= SpDelta;
    }

    VPwm = SpDelta * SP + Speed_Integral * SI; //���ַ����Ѿ�ȷ����ȷ
    return VPwm;
}

/*ֱ����*/
s16 AnglePID(void)
{
    static u16 Angle_Last = 0; //�ϴεĽǶ�
    s16 B_PWM;

    float Derror = 0;

    Angle = Get_Adc_Average(2, 12); //��ȡ��ǰ�Ƕ�

    Perror = target - Angle;	//��������
    Derror = Angle_Last - Angle; //΢�ֲ���
    Angle_Last = Angle;

    if(abs(Perror) < 500) //�ж���û�е��£����¹���
        B_PWM = (s16)(AP * Perror + AD * Derror);
    else
        B_PWM = 0;

    return B_PWM;
}



void TIM6_IRQHandler(void)              //��ʱ��5�жϺ���
{
    static u8 SpCount = 0, LedCount = 0; //SpCount�ٶȻ����ڼ�����LedCount״̬�Ƶ��ڼ���
    static s16 Angle_Pwm, Speed_Pwm; //�ǶȻ����ٶȻ�PWM���

    if(TIM6->SR & 0X0001) //����ж�
    {
        Angle_Pwm = AnglePID();//ֱ����

        LedCount++;

        SpCount++;
        if(SpCount > 10) //�ϳ�ʱ���һ���ٶȻ�
        {
            SpCount = 0; //
            Speed = count - count0; //��ǰ�ٶ�
            count0 = count; //��¼�ϴε�λ��
            Speed_Pwm = SpeedPID(Speed); //�ٶȻ�
        }

        setmoto(Angle_Pwm + Speed_Pwm);

        if(LedCount > 200) LedCount = 0, LED = ~LED;


    }
    TIM6->SR &= ~(1 << 0); //��ʱ����־����
}


