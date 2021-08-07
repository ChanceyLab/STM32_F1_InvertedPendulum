/******************************************
����������ã��˴�ʹ���˴���3
USART3_TX   GPIOB.10
USART3_RX	  GPIOB.11
8λ��У���շ�ģʽ
ͨ������Э��protocol(char*s)���ô����޸���Ӧ�������ӿ���pid�����ĵ����ٶ�
Э���ʽΪ kpa kia kda kpv kdv\n

֮���Ǽ����Ա������
void prints(char *str)  ����ַ���
void printc(char c)     ������ַ�
void itoa(int num, char*str,int radix) ����ת�ַ��� num--���� str--�ַ���ָ�� radix--����
void printnum(int num)  �������
*******************************************/
#include "sys.h"
#include "usart.h"	 
#include "variable.h"

#if EN_USART3_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
char USART_RX_BUF[USART_REC_LEN];     //���ջ���,���200���ֽ�.
//����״̬

u16 rx_num=0;       //������Ŀ
u8 rxflag=0;        //���ܽ�����־λ

void uart3_init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	//USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  //USART3_RX	  GPIOB.11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART1 ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������1
  
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���

}

void USART3_IRQHandler(void)                	//����3�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
		{
		   Res =USART_ReceiveData(USART3); 
			 if(Res=='\n') rxflag=1,USART_RX_BUF[rx_num]='\n';              //���ܵ�\n,��ȡ��־λ��1
        else			                          //������뻺����
       {USART_RX_BUF[rx_num]=Res;rx_num++;}			 		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
void protocol(char*s)   //����Э�飬�����޸Ĳ��� ���ڸ�ʽ   "kpa kia kda kpv kdv\n"
{
	int i;
	int	j=0;
		if(rxflag==1)                     //�������
			{        
		    for(i=0;i<=rx_num+1;i++)
				{
					if(USART_RX_BUF[i]==' ')    //��ȡ���ո�
					{					
						switch (j)                //�����ݸ�ֵ����Ӧ����
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
				rx_num=0;rxflag=0; 			 //����	
			}
}

void itoa(int num, char*str,int radix)  //���ڽ�����ת��Ϊ�ַ��� radixΪ���� ����str
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



void prints(char *str)   //����һ���ַ��� ������
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

void printc(char c) //����һ���ַ�
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

