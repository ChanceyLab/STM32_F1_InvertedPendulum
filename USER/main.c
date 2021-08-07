#include "sys.h"
#include <stdlib.h>
#include  "stdio.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"
#include "exti.h"
#include "pwm.h"
#include "motor.h"
#include "time2.h"
#include "adc.h"
#include "stmflash.h"
#include "control.h"
#include "OLED_I2C.h"
#include "encoder.h"
#include "variable.h"//�����ļ��еı�������


int target=3894; //�����Ƕȣ��Ƕȴ�������AD�������ֵ

u16 kpa=24,kia=49,kda=160;//�ǶȻ���pid����
u16 kpv=20,kdv=150;  //λ�û���pd����

//���ʱ��������������ĳ�ʼPWM��ռ�ձȺ�ʱ��
int op1=3000,op2=1200,t1=430,t2=160;
u8 page=1;//OLED��ʾ�˵�����ҳ�ı�־  

void MENU(void);//OLED��ʾ����,����OLED_I2C.h��OLED_I2C.c�л�������

int main(void)
 {
	 char*s=USART_RX_BUF;//�������ݻ����ָ��
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�ж����ȼ����ж���2	 
   uart3_init(9600);         //���ڳ�ʼ��
	 LED_Init();              //led��ʼ��
	 delay_init();            //��ʱ��ʼ��
	 MOTO_init();             //�����ʼ��
	 Adc_Init();              //adģ���ʼ��
	 EXTIX_Init();          //����IO��״̬��ʼ��
	 I2C_Configuration();   //��ʾIIC�ӿڳ�ʼ��
	 OLED_Init();            //OLED��ʾ����ʼ��
   TIM_PWM_Init(7199,0);   //pwm����7200��
   TIM5_init(49,7199);    //5ms�жϡ�ÿ5msPID����һ��PWMֵ
	 
   TIM6_init(49,7199);//5ms�жϡ�ÿ5msPID����һ��PWMֵ
											//����2�аڱ���תһȦʱ�ô��ж�
	 TIM_Cmd(TIM6, DISABLE);//�ȹرչ���2�õ�TIM6��5ms�ж�
	 ENC_Init();            //������ģʽ��ʼ��
	 EN=0;                 //TB6612������STBY�رգ�����ͣ���������иı�״̬
   UI_TILTLE();        //OLED��ʾ���еı���
   UI_1();             //OLED��ʾ���г�ʼҳ��   
 //�ж�flash����������addr�Ƿ�д�����ݣ������������  
  if(*(u16*)(addr)!=0xffff) Read_Data(addr);
  if (*(u16*)(addr+10)!=0xffff)  target=*(u16*)(addr+10);
	 while(1)
	 { 
			function();//����������չ����1����ڣ�������2���ڱ���ת1Ȧ��ʵ�ֺ���

			protocol(s);////����ͨѶЭ��	
			MENU();
   }
}

/*
markֵ
ҳ��1
1  		2  		3   	4   	5   a-angle�ڸ˽Ƕ�   v-�ڱ�λ��
kpa		kia		kda		kpv		kdv
ҳ��2
6			  			7     				8  						9   
��ڵ�ƽ��		��תһ��				����չ				 ����չ
mark0 �洢mark��һ�ε�ֵ
*/

void MENU(void)
{
//mark��ֵ5��6Ϊҳ��1��ҳ��2���л���Ϊ�˱����ظ�ˢ����ʾ,
//����mark0�е�ֵ
  if(mark==5&&mark0==6) UI_1(),mark0=4,page=1;
//mark��ֵ6��5Ϊҳ��2��ҳ��1���л���Ϊ�˱����ظ�ˢ����ʾ,
//����mark0�е�ֵ	
  if(mark==6&&mark0==5) UI_2(),mark0=7,page=2;
  if(page==1) 
  {
     //��ʾ����ֵ		
    OLED_ShowNum(24,3, kpa, 1);
    OLED_ShowNum(24,4, kia, 1); 
    OLED_ShowNum(24,5, kda, 1);
    OLED_ShowNum(24,6, kpv, 1);
    OLED_ShowNum(24,7, kdv, 1);
    
    OLED_ShowOneNum(55,2+mark0,-16,1);
    OLED_ShowOneNum(55,2+mark,76,1);
  }
if(page==2)
 {
   OLED_ShowOneNum(55,mark0-3,-16,1);
   OLED_ShowOneNum(55,mark-3,76,1);
   
 }
       //��ʾ�ǶȺ�λ��
   OLED_ShowNum(70,6,angle0,1);OLED_ShowNum(70,4,count,1);
   if(EN==1)  	OLED_ShowStr(70,7," ON",1);
   else   OLED_ShowStr(70,7,"OFF",1);
}
