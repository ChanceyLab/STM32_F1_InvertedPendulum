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
#include "variable.h"//各个文件中的变量定义


int target=3894; //期望角度，角度传感器经AD测量后的值

u16 kpa=24,kia=49,kda=160;//角度环的pid参数
u16 kpv=20,kdv=150;  //位置环的pd参数

//起摆时，来回两个方向的初始PWM的占空比和时间
int op1=3000,op2=1200,t1=430,t2=160;
u8 page=1;//OLED显示菜单所在页的标志  

void MENU(void);//OLED显示函数,放在OLED_I2C.h，OLED_I2C.c中会有问题

int main(void)
 {
	 char*s=USART_RX_BUF;//串口数据缓存的指针
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断优先级，中断组2	 
   uart3_init(9600);         //串口初始化
	 LED_Init();              //led初始化
	 delay_init();            //延时初始化
	 MOTO_init();             //电机初始化
	 Adc_Init();              //ad模块初始化
	 EXTIX_Init();          //按键IO口状态初始化
	 I2C_Configuration();   //显示IIC接口初始化
	 OLED_Init();            //OLED显示屏初始化
   TIM_PWM_Init(7199,0);   //pwm精度7200、
   TIM5_init(49,7199);    //5ms中断。每5msPID调节一次PWM值
	 
   TIM6_init(49,7199);//5ms中断。每5msPID调节一次PWM值
											//功能2中摆臂旋转一圈时用此中断
	 TIM_Cmd(TIM6, DISABLE);//先关闭功能2用的TIM6的5ms中断
	 ENC_Init();            //编码器模式初始化
	 EN=0;                 //TB6612驱动的STBY关闭，在启停按键功能中改变状态
   UI_TILTLE();        //OLED显示屏中的标题
   UI_1();             //OLED显示屏中初始页面   
 //判断flash程序外区域addr是否写入数据，有数据则读入  
  if(*(u16*)(addr)!=0xffff) Read_Data(addr);
  if (*(u16*)(addr+10)!=0xffff)  target=*(u16*)(addr+10);
	 while(1)
	 { 
			function();//倒立摆中扩展功能1（起摆），功能2（摆臂旋转1圈）实现函数

			protocol(s);////串口通讯协议	
			MENU();
   }
}

/*
mark值
页面1
1  		2  		3   	4   	5   a-angle摆杆角度   v-摆臂位置
kpa		kia		kda		kpv		kdv
页面2
6			  			7     				8  						9   
起摆到平衡		旋转一周				待扩展				 待扩展
mark0 存储mark上一次的值
*/

void MENU(void)
{
//mark的值5到6为页面1至页面2的切换，为了避免重复刷新显示,
//更改mark0中的值
  if(mark==5&&mark0==6) UI_1(),mark0=4,page=1;
//mark的值6到5为页面2至页面1的切换，为了避免重复刷新显示,
//更改mark0中的值	
  if(mark==6&&mark0==5) UI_2(),mark0=7,page=2;
  if(page==1) 
  {
     //显示参数值		
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
       //显示角度和位置
   OLED_ShowNum(70,6,angle0,1);OLED_ShowNum(70,4,count,1);
   if(EN==1)  	OLED_ShowStr(70,7," ON",1);
   else   OLED_ShowStr(70,7,"OFF",1);
}
