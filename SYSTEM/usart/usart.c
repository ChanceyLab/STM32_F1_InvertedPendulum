/******************************************
串口相关设置，此处使用了串口3
USART3_TX   GPIOB.10
USART3_RX	  GPIOB.11
8位无校验收发模式
通过串口协议protocol(char*s)，用串口修改相应参数，加快了pid参数的调试速度
协议格式为 kpa kia kda kpv kdv\n

之后是几种自编输出流
void prints(char *str)  输出字符串
void printc(char c)     输出单字符
void itoa(int num, char*str,int radix) 数据转字符串 num--数据 str--字符串指针 radix--进制
void printnum(int num)  输出数据
*******************************************/
#include "sys.h"
#include "usart.h"	 
#include "variable.h"

#if EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
char USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大200个字节.
//接收状态

u16 rx_num=0;       //接收数目
u8 rxflag=0;        //接受结束标志位

void uart3_init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	//USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  //USART3_RX	  GPIOB.11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART1 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure); //初始化串口1
  
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口

}

void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
		{
		   Res =USART_ReceiveData(USART3); 
			 if(Res=='\n') rxflag=1,USART_RX_BUF[rx_num]='\n';              //接受到\n,读取标志位置1
        else			                          //否则放入缓冲区
       {USART_RX_BUF[rx_num]=Res;rx_num++;}			 		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
void protocol(char*s)   //串口协议，用于修改参数 串口格式   "kpa kia kda kpv kdv\n"
{
	int i;
	int	j=0;
		if(rxflag==1)                     //接受完毕
			{        
		    for(i=0;i<=rx_num+1;i++)
				{
					if(USART_RX_BUF[i]==' ')    //读取到空格
					{					
						switch (j)                //将数据赋值给对应参数
						{
							case(0):kia=atoi(s+i);j++;break;
							case(1):kda=atoi(s+i);j++;break;
							case(2):kpv=atoi(s+i);j++;break;							
							case(3):kdv=atoi(s+i);kpa=atoi(s);
						}					
					}
					USART_SendData(USART3,USART_RX_BUF[i]);				
				  while((USART3->SR&0X40)==0); 				
				}        
				rx_num=0;rxflag=0; 			 //清零	
			}
}

void itoa(int num, char*str,int radix)  //用于将数字转化为字符串 radix为进制 返回str
{
    char temp;
    char index[]="0123456789ABCDEF";
    unsigned unum;
    int i=0,j,k;
    /*??unum??*/
    if(radix==10&&num<0)/*?????*/
    {
      unum=(unsigned)-num;
      str[i++]='-';
    }
    else 
      unum=(unsigned)num;/*????*/
    do{
      str[i++]=index[unum%(unsigned)radix];
      unum/=radix;
      }
    while(unum);
      
    str[i]='\0';

    if(str[0]=='-')
      k=1;/*?????*/
    else 
      k=0;

    for(j=k;j<=(i-1)/2;j++)
    {
      temp=str[j];
      str[j]=str[i-1+k-j];
      str[i-1+k-j]=temp;
    }
}



void prints(char *str)   //发送一个字符串 并换行
{
  while(*str!='\0')
  {
    USART_SendData(USART3,*str);				
		while((USART3->SR&0X40)==0);
    str++;
  } 
    USART_SendData(USART3,'\n');				
		while((USART3->SR&0X40)==0);  
}

void printc(char c) //发送一个字符
{
    USART_SendData(USART3,c);				
		while((USART3->SR&0X40)==0);
}

void printnum(int num)
{
  char *str;
  char n[7];
  str=n;
  itoa(num,n,10);
    while(*str!='\0')
  {
    USART_SendData(USART3,*str);				
    while((USART3->SR&0X40)==0);
    str++;
  } 
}
#endif	

