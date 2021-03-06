/******************************************
用外部中断定义了相应的按键功能
Pinx口对应响应的外部中断x
KEY_EN    PA12 --电机使能与否按键
KEY_UP    PA10 --五项按键上键，光标向上选定
KEY_DOWN  PA5  --五项按键下键，光标向下选定
KEY_P     PA4  --五项按键右键，选定的参数plus++
KEY_M     PA0	 --五项按键左键，选定的参数minus--
KEY_C     PA1  --五项按键中键，短按确定功能，长按设置参数为当前的参数值
*******************************************/
#include "exti.h"
#include "led.h"
#include "delay.h"
#include "stmflash.h"
#include "variable.h"

u8 mark=1;
u8 mark0=5;
u8 func=0;
u8 WriteOk=0;           //写入成功标志
u16 redif[6];           //写入flash的数据

void exti_io_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_10|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//外部中断初始化函数
void EXTIX_Init(void)
{

    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
    exti_io_init();

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line=EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line=EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource10);
    EXTI_InitStructure.EXTI_Line=EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);
    EXTI_InitStructure.EXTI_Line=EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2，
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2，
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2，
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2，
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2，
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);

}

void EXTI1_IRQHandler(void)   //write flash
{

    delay_ms(10);
    if(KEY_C==0)
    {
        int i;
        while(!KEY_C)
        {
            delay_ms(10);
            i++;
        }
        LED=~LED;
        if(i>200&&page==1)
            //longset();
        {
//保存现有数据
            redif[0]=kpa;
            redif[1]=kia;
            redif[2]=kda;
            redif[3]=kpv;
            redif[4]=kdv;
            STMFLASH_Write(addr,(u16*)redif,5);
            WriteOk=1;
        }
        else
        {
            func=mark-5;
        }

    }
    EXTI_ClearITPendingBit(EXTI_Line1);
}

void EXTI9_5_IRQHandler(void)
{
    delay_ms(10);
    if(KEY_DOWN==0)
    {
        while(!KEY_DOWN);
        LED=~LED;
        WriteOk=0;
        mark0=mark;
        mark++;
        if(mark>9)
        {   mark=9;
            mark0=8;
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line5);
}

void EXTI15_10_IRQHandler(void)  //
{
    if(EXTI_GetITStatus(EXTI_Line10))
    {
        delay_ms(10);
        if(KEY_UP==0)
        {
            while(!KEY_UP);
            LED=~LED;
            WriteOk=0;
            mark0=mark;
            mark--;
            if(mark==0)
            {
                mark=1;
                mark0=2;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if(EXTI_GetITStatus(EXTI_Line12))   //使能电机
    {
        delay_ms(10);
        if(KEY_EN==0)
        {
            while(!KEY_EN);
            EN=~EN;
            LED=~LED;
        }
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
}


void EXTI4_IRQHandler(void)   //PA4 ++
{
    delay_ms(10);
    if(KEY_P==0)
    {
        while(!KEY_P);
        LED=~LED;
        switch(mark)
        {
        case(1):
            kpa++;
            break;
        case(2):
            kia++;
            break;
        case(3):
            kda+=10;
            break;
        case(4):
            kpv++;
            break;
        case(5):
            kdv+=10;
            break;
        default:
            break;
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line4);
}

void EXTI0_IRQHandler(void)   //PA0  --
{
    delay_ms(10);
    if(KEY_M==0)
    {
        while(!KEY_M);
        LED=~LED;
        switch(mark)
        {
        case(1):
            kpa--;
            break;
        case(2):
            kia--;
            break;
        case(3):
            kda-=10;
            break;
        case(4):
            kpv--;
            break;
        case(5):
            kdv-=10;
            break;
        default:
            break;
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line0);
}

