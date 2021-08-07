/******************************************
������ƶ˿ڶ���
��ӦTB6612���ƶ˿�
AN0--PB4
AN1--PB15
EN--PA11
void setmoto(int pwmx) ��������ֵ�����ı���ת��ת�ٺ���
void function(void)  ���ܶ��庯��
func1  --��ڲ�����
func2  --��ƽ��״̬�£�������ת��360�ȣ�Ȼ���ȶ�
*******************************************/
#include "motor.h"
#include "variable.h"
#include  "stmflash.h"
extern float Speed_Integral;
extern u16 count0;
int local_pos;
//��ʼ�����ת����ƿ�
void MOTO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //ʹ��GPIO����ʱ��ʹ��


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;         //stbyʹ�ܶ˿�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    AN1=AN0=0;
}

//�趨pwmֵ
//���������жϷ���
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


void function(void)//����������չ����1����ڣ�������2���ڱ���ת1Ȧ��ʵ�ֺ���
{

    if(WriteOk==1)
        OLED_ShowCN(111,0,0);
    else
        OLED_ShowCN(111,0,1);


    if(func2==1)  //����2����תһ�ܱ�־
    {
        if(abs(count-local_pos)>OneCycle||	abs(target-angle0)>300)//�ж��Ƿ���תһ��λ��
        {
            count=count_exp;   //�ö�ʱ��5��PID�������ȶ����λ��
            TIM_Cmd(TIM6,DISABLE);
            Speed_Integral=0;//��������

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
        count=10000;  //λ�ù�λ
        count0=10000;
        local_pos=count;    //��¼��ǰλ�ã���Ϊ��ת360�ȵĳ�ֵ

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
