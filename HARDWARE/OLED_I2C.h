#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "stm32f10x.h"
#define IIC_SDA PBout(9)
#define	IIC_SCL PBout(8)



#define OLED_ADDRESS	0x78

void I2C_Configuration(void);
void IIC_WriteByte(u8 txd);
void WriteCmd(unsigned char IIC_Command);
void WriteDat(unsigned char IIC_Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_ShowOneNum(unsigned char x, unsigned char y, int num, unsigned char TextSize);
void OLED_ShowNum(unsigned char x, unsigned char y, u16 num, unsigned char TextSize);
void Show(u8 mark,u8 mark1);
void UI_TILTLE(void);
void UI_1(void);
void UI_2(void);
#endif
