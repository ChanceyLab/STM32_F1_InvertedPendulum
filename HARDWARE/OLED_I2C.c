/************************************************************************************
* 注意：这里对于oled的IIC代码是不规范的，因为显示中只用到了写入数据，不需要读oled中
        寄存器的值，所以我们并不关心从设备是否给予主设备MCU回应，所以这里的IIC通讯代
        码是一种尽力而为的写法，没有判断ack信号，学习IIC通讯时请注意
 IIC_SDA --PB9
 IIC_SCL --PB8
* 1. void I2C_Configuration(void) -- 配置软件硬件I2C的IO口，因为不需要读寄存器，所以配置成了输出模式
* 2. void IIC_WriteByte(u8 txd) -- 向寄存器地址写一个byte的数据
* 3. void WriteCmd(unsigned char IIC_Command) -- 写命令
* 4. void WriteDat(unsigned char IIC_Data) -- 写数据
* 5. void OLED_Init(void) -- OLED屏初始化，参考器件手册的初始化命令
* 6. void OLED_SetPos(unsigned char x, unsigned char y) -- 设置起始点坐标
* 7. void OLED_Fill(unsigned char fill_Data) -- 全屏填充
* 8. void OLED_CLS(void) -- 清屏
* 9. void OLED_ON(void) -- 唤醒
* 10. void OLED_OFF(void) -- 睡眠
* 11. void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) -- 显示字符串(字体大小有6*8和8*16两种)
* 12. void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) -- 显示中文(中文需要先取模，然后放到codetab.h中)
* 13. void OLED_ShowOneNum(unsigned char x, unsigned char y, int num, unsigned char TextSize) --显示一个数字，0~9之间
* 14. void OLED_ShowNum(unsigned char x, unsigned char y, u16 num, unsigned char TextSize) --显示数字,最多5位，0~99999之间
* 15. void UI_TILTLE(void) --显示初始化的标题界面
* 16. void UI_1(void) --显示初始化的页面1界面
* 17. void UI_2(void) --显示初始化的页面2界面
* 18. void Show(u8 mark,u8 mark1) --显示选中标记符以及相关参数
*************************************************************************************/

#include "OLED_I2C.h"
#include "delay.h"
#include "codetab.h"
#include "variable.h"

void I2C_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);
}

//产生IIC起始信号
void IIC_Start(void)
{
    IIC_SDA=1;
    IIC_SCL=1;
    delay_us(1);
    IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    delay_us(1);
    IIC_SCL=0;//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void)
{
    IIC_SCL=0;
    IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    delay_us(1);
    IIC_SCL=1;
    IIC_SDA=1;//发送I2C总线结束信号
    delay_us(1);
}

void IIC_WriteByte(u8 txd)
{
    u8 t;
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0; t<8; t++)
    {
        if(txd&0x80)
            IIC_SDA=1;
        else
            IIC_SDA=0;
        txd<<=1;
        //delay_us(1);
        IIC_SCL=1;
        delay_us(1);
        IIC_SCL=0;
        delay_us(1);
    }
    IIC_SDA=1;
    delay_us(1);
    IIC_SCL=1;
    delay_us(1);
    IIC_SCL=0;
    delay_us(1);
}

void WriteCmd(unsigned char IIC_Command)//写命令
{
    IIC_Start();
    IIC_WriteByte(0x78);
    IIC_WriteByte(0x00);			//write data
    IIC_WriteByte(IIC_Command);
    IIC_Stop();
}

void WriteDat(unsigned char IIC_Data)//写数据
{
    IIC_Start();
    IIC_WriteByte(0x78);
    IIC_WriteByte(0x40);			//write data
    IIC_WriteByte(IIC_Data);
    IIC_Stop();
}

void OLED_Init(void)
{
    delay_ms(100); //这里的延时很重要

    WriteCmd(0xAE); //display off
    WriteCmd(0x20);	//Set Memory Addressing Mode
    WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xc8);	//Set COM Output Scan Direction
    WriteCmd(0x00); //---set low column address
    WriteCmd(0x10); //---set high column address
    WriteCmd(0x40); //--set start line address
    WriteCmd(0x81); //--set contrast control register
    WriteCmd(0xAF); //亮度调节 0x00~0xff
    WriteCmd(0xa1); //--set segment re-map 0 to 127
    WriteCmd(0xa6); //--set normal display
    WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    WriteCmd(0x3F); //
    WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    WriteCmd(0xd3); //-set display offset
    WriteCmd(0x00); //-not offset
    WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    WriteCmd(0xf0); //--set divide ratio
    WriteCmd(0xd9); //--set pre-charge period
    WriteCmd(0x22); //
    WriteCmd(0xda); //--set com pins hardware configuration
    WriteCmd(0x12);
    WriteCmd(0xdb); //--set vcomh
    WriteCmd(0x20); //0x20,0.77xVcc
    WriteCmd(0x8d); //--set DC-DC enable
    WriteCmd(0x14); //
    WriteCmd(0xaf); //--turn on oled panel
}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{
    WriteCmd(0xb0+y);
    WriteCmd(((x&0xf0)>>4)|0x10);
    WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
    unsigned char m,n;
    for(m=0; m<8; m++)
    {
        WriteCmd(0xb0+m);		//page0-page1
        WriteCmd(0x00);		//low column start address
        WriteCmd(0x10);		//high column start address
        for(n=0; n<128; n++)
        {
            WriteDat(fill_Data);
        }
    }
}

void OLED_CLS(void)//清屏
{
    OLED_Fill(0x00);
}


// 将OLED从休眠中唤醒
void OLED_ON(void)
{
    WriteCmd(0X8D);  //设置电荷泵
    WriteCmd(0X14);  //开启电荷泵
    WriteCmd(0XAF);  //OLED唤醒
}


//让OLED休眠 -- 休眠模式下,OLED功耗不到10uA

void OLED_OFF(void)
{
    WriteCmd(0X8D);  //设置电荷泵
    WriteCmd(0X10);  //关闭电荷泵
    WriteCmd(0XAE);  //OLED休眠
}

// x,y -- 起始点坐标(x:0~127, y:0~7);
//ch[] -- 要显示的字符串;
//TextSize -- 字符大小(1:6*8 ; 2:8*16)
//显示codetab.h中的ASCII字符,有6*8和8*16可选择
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
    unsigned char c = 0,i = 0,j = 0;
    switch(TextSize)
    {
    case 1:
    {
        while(ch[j] != '\0')
        {
            c = ch[j] - 32;
            if(x > 126)
            {
                x = 0;
                y++;
            }
            OLED_SetPos(x,y);
            for(i=0; i<6; i++)
                WriteDat(F6x8[c][i]);
            x += 6;
            j++;
        }
    }
    break;
    case 2:
    {
        while(ch[j] != '\0')
        {
            c = ch[j] - 32;
            if(x > 120)
            {
                x = 0;
                y++;
            }
            OLED_SetPos(x,y);
            for(i=0; i<8; i++)
                WriteDat(F8X16[c*16+i]);
            OLED_SetPos(x,y+1);
            for(i=0; i<8; i++)
                WriteDat(F8X16[c*16+i+8]);
            x += 8;
            j++;
        }
    }
    break;
    }
}


//x,y -- 起始点坐标(x:0~127, y:0~7);
//N:汉字在codetab.h中的索引
//显示codetab.h中的汉字,16*16点阵
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned char wm=0;
    unsigned int  adder=32*N;
    OLED_SetPos(x , y);
    for(wm = 0; wm < 16; wm++)
    {
        WriteDat(F16x16[adder]);
        adder += 1;
    }
    OLED_SetPos(x,y + 1);
    for(wm = 0; wm < 16; wm++)
    {
        WriteDat(F16x16[adder]);
        adder += 1;
    }
}

// x,y -- 起始点坐标(x:0~127, y:0~7);
//num -- 要显示的数字(0~9);
//TextSize -- 字符大小(1:6*8 ; 2:8*16)
void OLED_ShowOneNum(unsigned char x, unsigned char y, int num, unsigned char TextSize)
{
    unsigned char c = 0,i = 0;
    switch(TextSize)
    {
    case 1:
    {
        c=num+16;
        if(x > 126)
        {
            x = 0;
            y++;
        }
        OLED_SetPos(x,y);
        for(i=0; i<6; i++)
            WriteDat(F6x8[c][i]);
    }
    break;
    case 2:
    {
        c = 16+num;
        if(x > 120)
        {
            x = 0;
            y++;
        }
        OLED_SetPos(x,y);
        for(i=0; i<8; i++)
            WriteDat(F8X16[c*16+i]);
        OLED_SetPos(x,y+1);
        for(i=0; i<8; i++)
            WriteDat(F8X16[c*16+i+8]);
        x += 8;
    }
    break;
    }
}

// x,y -- 起始点坐标(x:0~127, y:0~7);
//num -- 要显示的数字(最大为五位数);
//TextSize -- 字符大小(1:6*8 ; 2:8*16)
void OLED_ShowNum(unsigned char x, unsigned char y, u16 num, unsigned char TextSize)
{
    int i=0;
    int  n;
    int a[4];   //最长为五位数
    if (num==0)
    {
        OLED_ShowOneNum(x,y,num,TextSize);
        for(i=1; i<5; i++)
            OLED_ShowOneNum(x+6*i,y,-16,TextSize);
    }
    else
    {
        while(num>=10)
        {
            a[i]=num%10;
            num/=10;
            i++;
        }
        a[i]=num;
        n=i;
        for(i=0; i<5; i++)
        {
            if(n-i<0)  OLED_ShowOneNum(x+6*i,y,-16,TextSize);
            else  OLED_ShowOneNum(x+6*i,y,a[n-i],TextSize);
        }
    }
}

//oled交互界面设定
void UI_TILTLE(void)
{
    OLED_Fill(0x00);

    OLED_ShowCN(0,0,2);
    OLED_ShowCN(16,0,3);
    OLED_ShowCN(32,0,4);
    OLED_ShowCN(48,0,5);
    OLED_ShowCN(64,0,6);
    OLED_ShowCN(80,0,7);
    OLED_ShowCN(96,0,8);


}


void UI_R(void)
{
    u8 m,n;
    for(m=2; m<8; m++)
    {
        WriteCmd(0xb0+m);		//page0-page1
        WriteCmd(0x00);		//low column start address
        WriteCmd(0x10);		//high column start address
        for(n=0; n<128; n++)
        {
            WriteDat(0x00);
        }
    }
}
void UI_1(void)
{
    UI_R();
    OLED_ShowStr(0,3,"kpa:",1);
    OLED_ShowStr(0,4,"kia:",1);
    OLED_ShowStr(0,5,"kda:",1);
    OLED_ShowStr(0,6,"kpv:",1);
    OLED_ShowStr(0,7,"kdv:",1);
    OLED_ShowStr(70,3,"pos:",1);
    OLED_ShowStr(70,5,"angle:",1);
    OLED_ShowStr(104,7,"1/2",1);
}

void UI_2(void)
{
    UI_R();
    OLED_ShowStr(0,3,"Start",1);
    OLED_ShowStr(0,4,"Circle",1);
    OLED_ShowStr(0,5,"LockAngle",1);
    OLED_ShowStr(0,6,"Func4",1);
    OLED_ShowStr(104,7,"2/2",1);
    OLED_ShowStr(70,3,"pos:",1);
    OLED_ShowStr(70,5,"angle:",1);
}

