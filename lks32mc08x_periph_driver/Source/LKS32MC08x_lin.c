#include "lks32mc08x_lin.h"

LIN_InitTypeDef LIN_InitTypeStruct;
LIN_ReceiveTypeDef LIN_ReceiveTypeStruct;
stru_SendFaultStatus_t stru_SendFaults;
stru_ReceiveFaultStatus_t stru_ReceiveFaults;

void Lin_Gpio_init(void);
void Lin_Uart_init(UART_TypeDef* UARTx , u16 Baud_rate);
void Lin_UTimer_init_RX(TIM_TimerTypeDef *TIMERx);
u8 Lin_CheckPID(u8 id);
/*******************************************************************************
 �������ƣ�    void Lin_init(UART_TypeDef* UARTx , u16 Baud_rate,TIM_TimerTypeDef *TIMERx)
 ����������    LinӲ����ʼ��
 ���������    TIMERx����ʱ��ѡ�� TIMER0/TIMER1/TIMER2/TIMER3
               UARTx:����ѡ��UART0/UART1 , Baud_rate:������ѡ�� 
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
void Lin_init(UART_TypeDef* UARTx , u16 Baud_rate,TIM_TimerTypeDef *TIMERx)
{
	u8 i = 0;
  u32 t_baud  = ((u32)Baud_rate * 39321)>>16;/* ���ò�����Baud_rate��9/15��*/
	
	LIN_InitTypeStruct.Baud_rate = Baud_rate;
	LIN_InitTypeStruct.TIMERx = TIMERx;
	LIN_InitTypeStruct.UARTx = UARTx;
  UARTx->IF = 0xff;	
/********************LIN���ղ�����ʼ��************************/	
  LIN_ReceiveTypeStruct.DataReceiveflag = 0;
	LIN_ReceiveTypeStruct.FrameReceiveOverFlag = 0;
	LIN_ReceiveTypeStruct.Host_SendBreak_flag = 0;
	for(i = 0;i<8;i++)
	{
	  LIN_ReceiveTypeStruct.LinReceiveData[i] = 0;
	}
	LIN_ReceiveTypeStruct.ReceiveCheckSum = 0;
	LIN_ReceiveTypeStruct.ReceiveID = 0;
	LIN_ReceiveTypeStruct.ReceivePID = 0;
	LIN_ReceiveTypeStruct.Receive_Len = 8;       //Ĭ�Ͻ���8�ֽ�����
	LIN_ReceiveTypeStruct.RX_Data_Flag = 0;
	LIN_ReceiveTypeStruct.CheckSum_type = 0;
	Lin_Gpio_init();
	Lin_Uart_init(UARTx ,Baud_rate);
	LIN_InitTypeStruct.t_DIVL = LIN_InitTypeStruct.UARTx->DIVL;
	LIN_InitTypeStruct.t_DIVH = LIN_InitTypeStruct.UARTx->DIVH;
	LIN_InitTypeStruct.clkDivUART = 0;
    
	  LIN_InitTypeStruct.divCoefficient = UART_MCU_MCLK / t_baud / (1 + LIN_InitTypeStruct.clkDivUART);
    while (LIN_InitTypeStruct.divCoefficient > 0xFFFF)
    {
        LIN_InitTypeStruct.clkDivUART++;
        LIN_InitTypeStruct.divCoefficient = UART_MCU_MCLK / t_baud / (1 + LIN_InitTypeStruct.clkDivUART);
    }
	SYS_WR_PROTECT = 0x7a83;      /* ���ϵͳ�Ĵ���д���� */
  SYS_CLK_DIV2 = LIN_InitTypeStruct.clkDivUART;		
  SYS_WR_PROTECT = 0x0;         /* ����ϵͳ�Ĵ���д���� */
	LIN_InitTypeStruct.divCoefficient -= 1; 
	
	Lin_UTimer_init_RX(TIMERx);
  stru_SendFaults.R = 0; 
}


/*******************************************************************************
 �������ƣ�    void Lin_GPIO_init(void)
 ����������    lin GPIOӲ����ʼ��
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
void Lin_Gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct); //��ʼ���ṹ��

    //����UART0_RXD  P0.6
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO0, &GPIO_InitStruct);
    //����UART0_TXD  P0.7
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIO0, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIO0, GPIO_PinSource_6, AF4_UART);
    GPIO_PinAFConfig(GPIO0, GPIO_PinSource_7, AF4_UART);
	  
	  //����SLP_N  P3.4
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO3, &GPIO_InitStruct);
		GPIO_SetBits(GPIO3,GPIO_Pin_4);//����LINоƬ
		
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_6;
//    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_Init(GPIO2, &GPIO_InitStruct);
}

/*******************************************************************************
 �������ƣ�    void Lin_Uart_init(UART_TypeDef* UARTx , u16 Baud_rate)
 ����������    lin UART�Ĵ�������
 ���������    UARTx:����ѡ��UART0/UART1 , Baud_rate:������ѡ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
void Lin_Uart_init(UART_TypeDef* UARTx , u16 Baud_rate)
{
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);
    UART_InitStruct.BaudRate = Baud_rate;                                /* ���ò�����38400 */
    UART_InitStruct.WordLength = UART_WORDLENGTH_8b;                     /* �������ݳ���8λ */
    UART_InitStruct.StopBits = UART_STOPBITS_1b;                         /* ֹͣλ1λ */
    UART_InitStruct.FirstSend = UART_FIRSTSEND_LSB;                      /* �ȷ���LSB */
    UART_InitStruct.ParityMode = UART_Parity_NO;                         /* ����żУ�� */
    UART_InitStruct.IRQEna = UART_IRQEna_RcvOver;                        /* �����ж�ʹ��,ʹ�ܽ����ж� */
    UART_Init(UARTx, &UART_InitStruct);
    
}

/*******************************************************************************
 �������ƣ�    void Lin_UTimer_init_RX(TIM_TimerTypeDef *TIMERx)
 ����������    UTimerӲ����ʼ��
 ���������    TIMERx����ʱ��ѡ�� TIMER0/TIMER1/TIMER2/TIMER3
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
void Lin_UTimer_init_RX(TIM_TimerTypeDef *TIMERx)
{
//	  u32 t_time = 1000000/LIN_InitTypeStruct.Baud_rate;       /*1bit���贫��ʱ��*/
    TIM_TimerInitTypeDef TIM_InitStruct;

    TIM_TimerStrutInit(&TIM_InitStruct);                  /* Timer�ṹ���ʼ��*/
    TIM_InitStruct.Timer_TH = 96000;//t_time*15*48;    	  /* ��ʱ���������޳�ʼֵ 2ms*/
    TIM_InitStruct.Timer_ClockDiv = TIM_Clk_Div2;         /* ����Timerģ��ʱ��2��Ƶϵ�� */
	  TIM_InitStruct.Timer_IRQEna = Timer_IRQEna_Zero;      /* ʹ�ܹ����жϷ���⺯�������ж��жϱ�־λ*/
	  TIM_TimerInit(TIMERx, &TIM_InitStruct); 
    TIM_TimerCmd(TIMERx, DISABLE); /*ģ��ʧ�� */
    TIMERx->CNT = 0;
}

 /*******************************************************************************
 �������ƣ�    void RTU_TIMEIRQ(void)
 ����������    �������ݶ�ʱ���жϺ���
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2023/1/16     V1.0           HuangMG            ����
 *******************************************************************************/
void Lin_TIMEIRQ(void)
{
	 TIM_ClearIRQFlag(LIN_InitTypeStruct.TIMERx,TIMER_IF_ZERO);//���UTimer�����жϱ�־λ
	 LIN_InitTypeStruct.TIMERx->CNT = 1;//���㶨ʱ������ֵ
	 TIM_TimerCmd(LIN_InitTypeStruct.TIMERx,DISABLE);          //Timerģ��ر� 
	 LIN_ReceiveTypeStruct.DataProcess = 0;
	 stru_ReceiveFaults.B.Reveice_over_time = 1;               //���ճ�ʱ����
//	 LIN_ReceiveTypeStruct.Receive_SyncSegment_flag = 0;
}

/*******************************************************************************
 �������ƣ�    void Lin_UTimer_init_TX(TIM_TimerTypeDef *TIMERx)
 ����������    UTimerӲ����ʼ��
 ���������    TIMERx����ʱ��ѡ�� TIMER0/TIMER1/TIMER2/TIMER3
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
void Lin_UTimer_init_TX(TIM_TimerTypeDef *TIMERx)
{
    TIM_TimerInitTypeDef TIM_InitStruct;

    TIM_TimerStrutInit(&TIM_InitStruct);                  /* Timer�ṹ���ʼ��*/
    TIM_InitStruct.Timer_TH = 48;    									    /* ��ʱ���������޳�ʼֵ24000,����1us*/
    TIM_InitStruct.Timer_ClockDiv = TIM_Clk_Div2;         /* ����Timerģ��ʱ��2��Ƶϵ�� */
	  TIM_InitStruct.Timer_IRQEna = TIMER_IF_ZERO;          /* ʹ�ܹ����жϷ���⺯�������ж��жϱ�־λ*/
	  TIM_TimerInit(TIMERx, &TIM_InitStruct); 
    TIM_TimerCmd(TIMERx, ENABLE); /*ģ��ʧ�� */	
}

/*******************************************************************************
 �������ƣ�    int Delay_us(u32 us , u8 resetTim)
 ����������    ����������ʱ,��λ1us
 ���������    us ����ʱ����΢��
              resetTim����0λ��������������ʱ
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
int Delay_us(u32 us , u8 resetTim)
{
	static u32 Delay_us = 0;
	static u8 enableTime = 0; 
  if(resetTim)/*Ϊ1������ʱ��Ϊ��һ����ʱ��׼��*/
	{
	   enableTime = 0;
		 LIN_InitTypeStruct.TIMERx->CNT = 0;  
		 Delay_us = 0;
		 return 0;
	}
	if(enableTime == 0)
	{
	   enableTime = 1;
		 Delay_us = us;  
		 LIN_InitTypeStruct.TIMERx->CNT = 0;
     TIM_ClearIRQFlag(LIN_InitTypeStruct.TIMERx,TIMER_IF_ZERO);	 /*�����ʱ�������־λ*/	
		 return 1;
	}else{
	  if(TIM_GetIRQFlag(LIN_InitTypeStruct.TIMERx,TIMER_IF_ZERO))  /*�ж�0.5ms���������ж���1*/
	   {
			TIM_ClearIRQFlag(LIN_InitTypeStruct.TIMERx,TIMER_IF_ZERO); /*�����ʱ�������־λ*/	
		  if(Delay_us > 0)
		  {
					Delay_us--;                         /*ʱ���0.5ms*/
					if(Delay_us == 0)                   /*��ʱʱ�䵽*/
					{
						 enableTime = 0;
						 return 0;
					}else{
						 return 2;
					}
			 }else{
				enableTime = 0;
				return 0;
				}
		}else{
		  return 1;
	  }
	}
}

/*******************************************************************************
 �������ƣ�    u8 Lin_CheckPID(u8 id)
 ����������    PID����
 ���������    id:ID��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
u8 Lin_CheckPID(u8 id)
{
	u8 returnpid ;
	u8 P0 ;
	u8 P1 ;
	
	P0 = (((id)^(id>>1)^(id>>2)^(id>>4))&0x01)<<6 ;
	P1 = ((~((id>>1)^(id>>3)^(id>>4)^(id>>5)))&0x01)<<7 ;
	
	returnpid = id|P0|P1 ;
	
	return returnpid ;
	
}

/*******************************************************************************
 �������ƣ�    u8 Lin_Checksum(u8 id , u8 data[] ,u8 len ,u8 id_type) 
 ����������    LINУ�麯��
 ���������    id������ID��
              data[]���������ݻ�����,
              len:���ݳ���
              id_type��У�����ͣ�0������У��,1����ǿУ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����    �����ֻ֡�ܾ���У��
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6      V1.0           HuangMG             ����
 *******************************************************************************/
u8 Lin_Checksum(u8 id , u8 data[] ,u8 len ,u8 id_type) 
{
	u8 t = 0;
	u16 sum = 0;
	for(t=0;t<len;t++) //ID�����
	{
		sum += data[t];
		if(sum&0xff00)
		{
			sum&=0x00ff;
			sum+=1;
		}
	}
  if(id_type == Enhance_Check)			// ��ǿУ��
	{	
		sum+=Lin_CheckPID(id);
    if(sum&0xff00)
		{
			sum&=0x00ff;
			sum+=1;
		}		
  }
	sum = ~sum;	
	return (u8)sum ;

}
/*******************************************************************************
 �������ƣ�    static int LIN_SetBit(u8 byte)
 ����������    LIN����1�ֽ����ݺ���
 ���������    byte����Ҫ���͵��ֽ�
 ���������    ��
 �� �� ֵ��    1������ʧ�ܣ�0�����ͳɹ�
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
static int LIN_SetBit(u16 byte)
{
//	u32 t_time = 1000000/LIN_InitTypeStruct.Baud_rate;       /*1bit���贫��ʱ��*/
  if(LIN_InitTypeStruct.UARTx->STT & BIT0) /*���ͻ�������*/
  { //���ݷ��ͺ��� 
		LIN_ReceiveTypeStruct.TX_Data = 0x100;
	  LIN_InitTypeStruct.UARTx->BUFF = byte;
//		while(Delay_us(t_time*11,0))/*����1�ֽ�����,10��bitʱ�䣨11������1bit������*/
			{
				while(!(LIN_InitTypeStruct.UARTx->STT & BIT1)){};	/*�������*/
				LIN_InitTypeStruct.UARTx->STT = BIT1;					
			}
//			Delay_us(0,1); /*�����ʱ*/
			if((LIN_InitTypeStruct.UARTx->BUFF == byte) || (LIN_ReceiveTypeStruct.TX_Data == byte))
			{
			  return 0; //���ͳɹ�
			}else{
				return 1; //����ʧ��
			}
	}else{
		return 1;  //����ʧ��
	}
}

/*******************************************************************************
 �������ƣ�    static int Lin_SendBreak_SyncSegment(void)
 ����������    ��������ͬ������κ�ͬ����
 ���������    ��
 ���������    ��
 �� �� ֵ��    1������ʧ�ܣ�0�����ͳɹ�
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
//static int Lin_SendBreak_SyncSegment(void)
//{
//	  uint32_t clkDivUART=0;
//    uint32_t divCoefficient;
//	  u32 t_baud  = ((u32)LIN_InitTypeStruct.Baud_rate * 39321)>>16;/* ���ò�����Baud_rate��9/15��*/
//    u32 t_DIVL = LIN_InitTypeStruct.UARTx->DIVL , t_DIVH = LIN_InitTypeStruct.UARTx->DIVH;
//	  divCoefficient = UART_MCU_MCLK / t_baud / (1 + clkDivUART);
//    while (divCoefficient > 0xFFFF)
//    {
//        clkDivUART++;
//        divCoefficient = UART_MCU_MCLK / t_baud / (1 + clkDivUART);
//    }
//		
//    SYS_WR_PROTECT = 0x7a83;      /* ���ϵͳ�Ĵ���д���� */
//    SYS_CLK_DIV2 = clkDivUART;		
//    divCoefficient = divCoefficient - 1;   
//    LIN_InitTypeStruct.UARTx->DIVL = divCoefficient & 0xFF;     
//    LIN_InitTypeStruct.UARTx->DIVH = (divCoefficient & 0xFF00) >> 8;
//		
//		LIN_ReceiveTypeStruct.Host_SendBreak_flag = 1;//����ͬ������α�־λ��1����������ʱʹ�ã�
//		
//	  stru_SendFaults.B.SendBreakError = LIN_SetBit(0x00);//����ͬ�������
//		if(stru_SendFaults.B.SendBreakError) return 1;      //ͬ���������ʧ��
//	  LIN_InitTypeStruct.UARTx->DIVL = t_DIVL;
//	  LIN_InitTypeStruct.UARTx->DIVH = t_DIVH;
//		
//	  stru_SendFaults.B.SyncSegment = LIN_SetBit(0x55);   //��ͬ����
//		if(stru_SendFaults.B.SyncSegment) return 1;         //ͬ���η���ʧ��
//		return 0;  //���ͳɹ�
//}

static int Lin_SendBreak_SyncSegment(void)
{
    LIN_InitTypeStruct.UARTx->DIVL = LIN_InitTypeStruct.divCoefficient & 0xFF;     
    LIN_InitTypeStruct.UARTx->DIVH = (LIN_InitTypeStruct.divCoefficient & 0xFF00) >> 8;
		
		LIN_ReceiveTypeStruct.Host_SendBreak_flag = 1;//����ͬ������α�־λ��1����������ʱʹ�ã�
		
	  stru_SendFaults.B.SendBreakError = LIN_SetBit(0x100);//����ͬ�������
		if(stru_SendFaults.B.SendBreakError) return 1;      //ͬ���������ʧ��
	  LIN_InitTypeStruct.UARTx->DIVL = LIN_InitTypeStruct.t_DIVL;
	  LIN_InitTypeStruct.UARTx->DIVH = LIN_InitTypeStruct.t_DIVH;
		
	  stru_SendFaults.B.SyncSegment = LIN_SetBit(0x55);   //��ͬ����
		if(stru_SendFaults.B.SyncSegment) return 1;         //ͬ���η���ʧ��
		return 0;  //���ͳɹ�
}
/*******************************************************************************
 �������ƣ�    int Lin_SendHead(u8 id)
 ����������    ��������֡ͷ
 ���������    id��֡ID��
 ���������    ��
 �� �� ֵ��    1������ʧ�ܣ�0�����ͳɹ�
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
 int Lin_SendHead(u8 id)
{
	  u8 Set_flag = 0;
	  stru_SendFaults.R = 0;
	  LIN_InitTypeStruct.UARTx->IF = 0xff;	  /*����жϱ�־λ,���ڽ������ݣ���ǰ��ֹͣ
	                                            λ�����־λ��������ڽ����ж����ж�ͬ�����*/
    Set_flag = Lin_SendBreak_SyncSegment(); //����֡ͬ�������ͬ����
	  if(Set_flag) return 1;//����ʧ��
	  stru_SendFaults.B.PIDError = LIN_SetBit(Lin_CheckPID(id));//����ID
	  if(stru_SendFaults.B.PIDError) return 1;//PID�η���ʧ��
	  
		return 0;  //���ͳɹ�
}

/*******************************************************************************
 �������ƣ�    int Lin_SentData(u8 data[])
 ����������    ����֡����
 ���������    data[]���������ݻ�����,len:�����ֽ���
 ���������    ��
 �� �� ֵ��    1������ʧ�ܣ�0�����ͳɹ�
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
int Lin_SentData(u8 data[] , u8 len)
{
	u8 t = 0, Set_flag = 0;
	for(t=0;t<len;t++)
	{
		Set_flag = LIN_SetBit(data[t]);
		if(Set_flag) return 1;//����ʧ��
	}
	 return 0;  //���ͳɹ�
}

/*******************************************************************************
 �������ƣ�    int Lin_SendAnswer(u8 id ,u8 data[],u8 len)
 ����������    ��������Ӧ�����������ڵĴӻ������ͺ�����
 ���������    id��֡ID��
              data[]���������ݻ�����
              len:�����ֽ���
              id_type��У�����ͣ�0������У��,1����ǿУ��
 ���������    ��
 �� �� ֵ��    1������ʧ�ܣ�0�����ͳɹ�
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
int Lin_SendAnswer(u8 id ,u8 data[],u8 len,u8 id_type)
{
	stru_SendFaults.B.DataError = Lin_SentData(data,len);
	if(stru_SendFaults.B.DataError) return 1;//����ʧ��
	stru_SendFaults.B.CheckSumError = LIN_SetBit(Lin_Checksum(id,data,len,id_type));
	if(stru_SendFaults.B.CheckSumError) return 1;//����ʧ��
	return 0;  //���ͳɹ�
}

/*******************************************************************************
 �������ƣ�    void Lin_Host_Receive_Data(u8 len)
 ����������    ����֡�������ú���
 ���������    len:�����ֽ���
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            ����
 *******************************************************************************/
void Lin_Host_Receive_Data(u8 len)
{
  LIN_ReceiveTypeStruct.Receive_Len = len;        //���ý������ݳ���
	LIN_ReceiveTypeStruct.FrameReceiveOverFlag = 0; 
	LIN_ReceiveTypeStruct.DataReceiveflag = 0;
//	stru_ReceiveFaults.R = 0; 
	LIN_ReceiveTypeStruct.Host_SendBreak_flag = 0;
	LIN_ReceiveTypeStruct.RX_Data_Flag = 1;
}



