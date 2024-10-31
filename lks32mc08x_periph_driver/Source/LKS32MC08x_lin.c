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
 函数名称：    void Lin_init(UART_TypeDef* UARTx , u16 Baud_rate,TIM_TimerTypeDef *TIMERx)
 功能描述：    Lin硬件初始化
 输入参数：    TIMERx：定时器选择 TIMER0/TIMER1/TIMER2/TIMER3
               UARTx:串口选择，UART0/UART1 , Baud_rate:波特率选择 
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
void Lin_init(UART_TypeDef* UARTx , u16 Baud_rate,TIM_TimerTypeDef *TIMERx)
{
	u8 i = 0;
  u32 t_baud  = ((u32)Baud_rate * 39321)>>16;/* 设置波特率Baud_rate的9/15倍*/
	
	LIN_InitTypeStruct.Baud_rate = Baud_rate;
	LIN_InitTypeStruct.TIMERx = TIMERx;
	LIN_InitTypeStruct.UARTx = UARTx;
  UARTx->IF = 0xff;	
/********************LIN接收参数初始化************************/	
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
	LIN_ReceiveTypeStruct.Receive_Len = 8;       //默认接收8字节数据
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
	SYS_WR_PROTECT = 0x7a83;      /* 解除系统寄存器写保护 */
  SYS_CLK_DIV2 = LIN_InitTypeStruct.clkDivUART;		
  SYS_WR_PROTECT = 0x0;         /* 开启系统寄存器写保护 */
	LIN_InitTypeStruct.divCoefficient -= 1; 
	
	Lin_UTimer_init_RX(TIMERx);
  stru_SendFaults.R = 0; 
}


/*******************************************************************************
 函数名称：    void Lin_GPIO_init(void)
 功能描述：    lin GPIO硬件初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
void Lin_Gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct); //初始化结构体

    //配置UART0_RXD  P0.6
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO0, &GPIO_InitStruct);
    //配置UART0_TXD  P0.7
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIO0, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIO0, GPIO_PinSource_6, AF4_UART);
    GPIO_PinAFConfig(GPIO0, GPIO_PinSource_7, AF4_UART);
	  
	  //配置SLP_N  P3.4
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO3, &GPIO_InitStruct);
		GPIO_SetBits(GPIO3,GPIO_Pin_4);//唤醒LIN芯片
		
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_6;
//    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_Init(GPIO2, &GPIO_InitStruct);
}

/*******************************************************************************
 函数名称：    void Lin_Uart_init(UART_TypeDef* UARTx , u16 Baud_rate)
 功能描述：    lin UART寄存器配置
 输入参数：    UARTx:串口选择，UART0/UART1 , Baud_rate:波特率选择
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
void Lin_Uart_init(UART_TypeDef* UARTx , u16 Baud_rate)
{
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);
    UART_InitStruct.BaudRate = Baud_rate;                                /* 设置波特率38400 */
    UART_InitStruct.WordLength = UART_WORDLENGTH_8b;                     /* 发送数据长度8位 */
    UART_InitStruct.StopBits = UART_STOPBITS_1b;                         /* 停止位1位 */
    UART_InitStruct.FirstSend = UART_FIRSTSEND_LSB;                      /* 先发送LSB */
    UART_InitStruct.ParityMode = UART_Parity_NO;                         /* 无奇偶校验 */
    UART_InitStruct.IRQEna = UART_IRQEna_RcvOver;                        /* 串口中断使能,使能接收中断 */
    UART_Init(UARTx, &UART_InitStruct);
    
}

/*******************************************************************************
 函数名称：    void Lin_UTimer_init_RX(TIM_TimerTypeDef *TIMERx)
 功能描述：    UTimer硬件初始化
 输入参数：    TIMERx：定时器选择 TIMER0/TIMER1/TIMER2/TIMER3
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
void Lin_UTimer_init_RX(TIM_TimerTypeDef *TIMERx)
{
//	  u32 t_time = 1000000/LIN_InitTypeStruct.Baud_rate;       /*1bit所需传输时间*/
    TIM_TimerInitTypeDef TIM_InitStruct;

    TIM_TimerStrutInit(&TIM_InitStruct);                  /* Timer结构体初始化*/
    TIM_InitStruct.Timer_TH = 96000;//t_time*15*48;    	  /* 定时器计数门限初始值 2ms*/
    TIM_InitStruct.Timer_ClockDiv = TIM_Clk_Div2;         /* 设置Timer模块时钟2分频系数 */
	  TIM_InitStruct.Timer_IRQEna = Timer_IRQEna_Zero;      /* 使能过零中断方便库函数函数判断中断标志位*/
	  TIM_TimerInit(TIMERx, &TIM_InitStruct); 
    TIM_TimerCmd(TIMERx, DISABLE); /*模块失能 */
    TIMERx->CNT = 0;
}

 /*******************************************************************************
 函数名称：    void RTU_TIMEIRQ(void)
 功能描述：    接收数据定时器中断函数
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2023/1/16     V1.0           HuangMG            创建
 *******************************************************************************/
void Lin_TIMEIRQ(void)
{
	 TIM_ClearIRQFlag(LIN_InitTypeStruct.TIMERx,TIMER_IF_ZERO);//清除UTimer过零中断标志位
	 LIN_InitTypeStruct.TIMERx->CNT = 1;//清零定时器计数值
	 TIM_TimerCmd(LIN_InitTypeStruct.TIMERx,DISABLE);          //Timer模块关闭 
	 LIN_ReceiveTypeStruct.DataProcess = 0;
	 stru_ReceiveFaults.B.Reveice_over_time = 1;               //接收超时错误
//	 LIN_ReceiveTypeStruct.Receive_SyncSegment_flag = 0;
}

/*******************************************************************************
 函数名称：    void Lin_UTimer_init_TX(TIM_TimerTypeDef *TIMERx)
 功能描述：    UTimer硬件初始化
 输入参数：    TIMERx：定时器选择 TIMER0/TIMER1/TIMER2/TIMER3
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
void Lin_UTimer_init_TX(TIM_TimerTypeDef *TIMERx)
{
    TIM_TimerInitTypeDef TIM_InitStruct;

    TIM_TimerStrutInit(&TIM_InitStruct);                  /* Timer结构体初始化*/
    TIM_InitStruct.Timer_TH = 48;    									    /* 定时器计数门限初始值24000,周期1us*/
    TIM_InitStruct.Timer_ClockDiv = TIM_Clk_Div2;         /* 设置Timer模块时钟2分频系数 */
	  TIM_InitStruct.Timer_IRQEna = TIMER_IF_ZERO;          /* 使能过零中断方便库函数函数判断中断标志位*/
	  TIM_TimerInit(TIMERx, &TIM_InitStruct); 
    TIM_TimerCmd(TIMERx, ENABLE); /*模块失能 */	
}

/*******************************************************************************
 函数名称：    int Delay_us(u32 us , u8 resetTim)
 功能描述：    非阻塞型延时,单位1us
 输入参数：    us ：延时多少微秒
              resetTim：非0位立即结束本次延时
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
int Delay_us(u32 us , u8 resetTim)
{
	static u32 Delay_us = 0;
	static u8 enableTime = 0; 
  if(resetTim)/*为1结束延时，为下一次延时做准备*/
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
     TIM_ClearIRQFlag(LIN_InitTypeStruct.TIMERx,TIMER_IF_ZERO);	 /*清除定时器过零标志位*/	
		 return 1;
	}else{
	  if(TIM_GetIRQFlag(LIN_InitTypeStruct.TIMERx,TIMER_IF_ZERO))  /*判断0.5ms到，过零中断置1*/
	   {
			TIM_ClearIRQFlag(LIN_InitTypeStruct.TIMERx,TIMER_IF_ZERO); /*清除定时器过零标志位*/	
		  if(Delay_us > 0)
		  {
					Delay_us--;                         /*时间减0.5ms*/
					if(Delay_us == 0)                   /*延时时间到*/
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
 函数名称：    u8 Lin_CheckPID(u8 id)
 功能描述：    PID函数
 输入参数：    id:ID号
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
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
 函数名称：    u8 Lin_Checksum(u8 id , u8 data[] ,u8 len ,u8 id_type) 
 功能描述：    LIN校验函数
 输入参数：    id：发送ID，
              data[]：发送数据缓冲区,
              len:数据长度
              id_type：校验类型，0：经典校验,1：增强校验
 输出参数：    无
 返 回 值：    无
 其它说明：    另：诊断帧只能经典校验
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6      V1.0           HuangMG             创建
 *******************************************************************************/
u8 Lin_Checksum(u8 id , u8 data[] ,u8 len ,u8 id_type) 
{
	u8 t = 0;
	u16 sum = 0;
	for(t=0;t<len;t++) //ID和相加
	{
		sum += data[t];
		if(sum&0xff00)
		{
			sum&=0x00ff;
			sum+=1;
		}
	}
  if(id_type == Enhance_Check)			// 增强校验
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
 函数名称：    static int LIN_SetBit(u8 byte)
 功能描述：    LIN发送1字节数据函数
 输入参数：    byte：需要发送的字节
 输出参数：    无
 返 回 值：    1：发送失败，0：发送成功
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
static int LIN_SetBit(u16 byte)
{
//	u32 t_time = 1000000/LIN_InitTypeStruct.Baud_rate;       /*1bit所需传输时间*/
  if(LIN_InitTypeStruct.UARTx->STT & BIT0) /*发送缓冲区空*/
  { //数据发送函数 
		LIN_ReceiveTypeStruct.TX_Data = 0x100;
	  LIN_InitTypeStruct.UARTx->BUFF = byte;
//		while(Delay_us(t_time*11,0))/*发送1字节数据,10个bit时间（11是留有1bit余量）*/
			{
				while(!(LIN_InitTypeStruct.UARTx->STT & BIT1)){};	/*发生完成*/
				LIN_InitTypeStruct.UARTx->STT = BIT1;					
			}
//			Delay_us(0,1); /*清除延时*/
			if((LIN_InitTypeStruct.UARTx->BUFF == byte) || (LIN_ReceiveTypeStruct.TX_Data == byte))
			{
			  return 0; //发送成功
			}else{
				return 1; //发送失败
			}
	}else{
		return 1;  //发送失败
	}
}

/*******************************************************************************
 函数名称：    static int Lin_SendBreak_SyncSegment(void)
 功能描述：    发送主机同步间隔段和同步段
 输入参数：    无
 输出参数：    无
 返 回 值：    1：发送失败，0：发送成功
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
//static int Lin_SendBreak_SyncSegment(void)
//{
//	  uint32_t clkDivUART=0;
//    uint32_t divCoefficient;
//	  u32 t_baud  = ((u32)LIN_InitTypeStruct.Baud_rate * 39321)>>16;/* 设置波特率Baud_rate的9/15倍*/
//    u32 t_DIVL = LIN_InitTypeStruct.UARTx->DIVL , t_DIVH = LIN_InitTypeStruct.UARTx->DIVH;
//	  divCoefficient = UART_MCU_MCLK / t_baud / (1 + clkDivUART);
//    while (divCoefficient > 0xFFFF)
//    {
//        clkDivUART++;
//        divCoefficient = UART_MCU_MCLK / t_baud / (1 + clkDivUART);
//    }
//		
//    SYS_WR_PROTECT = 0x7a83;      /* 解除系统寄存器写保护 */
//    SYS_CLK_DIV2 = clkDivUART;		
//    divCoefficient = divCoefficient - 1;   
//    LIN_InitTypeStruct.UARTx->DIVL = divCoefficient & 0xFF;     
//    LIN_InitTypeStruct.UARTx->DIVH = (divCoefficient & 0xFF00) >> 8;
//		
//		LIN_ReceiveTypeStruct.Host_SendBreak_flag = 1;//发送同步间隔段标志位置1（主机接收时使用）
//		
//	  stru_SendFaults.B.SendBreakError = LIN_SetBit(0x00);//发送同步间隔段
//		if(stru_SendFaults.B.SendBreakError) return 1;      //同步间隔发送失败
//	  LIN_InitTypeStruct.UARTx->DIVL = t_DIVL;
//	  LIN_InitTypeStruct.UARTx->DIVH = t_DIVH;
//		
//	  stru_SendFaults.B.SyncSegment = LIN_SetBit(0x55);   //发同步段
//		if(stru_SendFaults.B.SyncSegment) return 1;         //同步段发送失败
//		return 0;  //发送成功
//}

static int Lin_SendBreak_SyncSegment(void)
{
    LIN_InitTypeStruct.UARTx->DIVL = LIN_InitTypeStruct.divCoefficient & 0xFF;     
    LIN_InitTypeStruct.UARTx->DIVH = (LIN_InitTypeStruct.divCoefficient & 0xFF00) >> 8;
		
		LIN_ReceiveTypeStruct.Host_SendBreak_flag = 1;//发送同步间隔段标志位置1（主机接收时使用）
		
	  stru_SendFaults.B.SendBreakError = LIN_SetBit(0x100);//发送同步间隔段
		if(stru_SendFaults.B.SendBreakError) return 1;      //同步间隔发送失败
	  LIN_InitTypeStruct.UARTx->DIVL = LIN_InitTypeStruct.t_DIVL;
	  LIN_InitTypeStruct.UARTx->DIVH = LIN_InitTypeStruct.t_DIVH;
		
	  stru_SendFaults.B.SyncSegment = LIN_SetBit(0x55);   //发同步段
		if(stru_SendFaults.B.SyncSegment) return 1;         //同步段发送失败
		return 0;  //发送成功
}
/*******************************************************************************
 函数名称：    int Lin_SendHead(u8 id)
 功能描述：    发送主机帧头
 输入参数：    id：帧ID号
 输出参数：    无
 返 回 值：    1：发送失败，0：发送成功
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
 int Lin_SendHead(u8 id)
{
	  u8 Set_flag = 0;
	  stru_SendFaults.R = 0;
	  LIN_InitTypeStruct.UARTx->IF = 0xff;	  /*清除中断标志位,用于接收数据，提前将停止
	                                            位错误标志位清除，用于接收中断内判断同步间隔*/
    Set_flag = Lin_SendBreak_SyncSegment(); //发送帧同步间隔和同步段
	  if(Set_flag) return 1;//发送失败
	  stru_SendFaults.B.PIDError = LIN_SetBit(Lin_CheckPID(id));//发送ID
	  if(stru_SendFaults.B.PIDError) return 1;//PID段发送失败
	  
		return 0;  //发送成功
}

/*******************************************************************************
 函数名称：    int Lin_SentData(u8 data[])
 功能描述：    发送帧数据
 输入参数：    data[]：发送数据缓冲区,len:发送字节数
 输出参数：    无
 返 回 值：    1：发送失败，0：发送成功
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
int Lin_SentData(u8 data[] , u8 len)
{
	u8 t = 0, Set_flag = 0;
	for(t=0;t<len;t++)
	{
		Set_flag = LIN_SetBit(data[t]);
		if(Set_flag) return 1;//发送失败
	}
	 return 0;  //发送成功
}

/*******************************************************************************
 函数名称：    int Lin_SendAnswer(u8 id ,u8 data[],u8 len)
 功能描述：    主机的响应函数（主机内的从机任务发送函数）
 输入参数：    id：帧ID号
              data[]：发送数据缓冲区
              len:发送字节数
              id_type：校验类型，0：经典校验,1：增强校验
 输出参数：    无
 返 回 值：    1：发送失败，0：发送成功
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
int Lin_SendAnswer(u8 id ,u8 data[],u8 len,u8 id_type)
{
	stru_SendFaults.B.DataError = Lin_SentData(data,len);
	if(stru_SendFaults.B.DataError) return 1;//发送失败
	stru_SendFaults.B.CheckSumError = LIN_SetBit(Lin_Checksum(id,data,len,id_type));
	if(stru_SendFaults.B.CheckSumError) return 1;//发送失败
	return 0;  //发送成功
}

/*******************************************************************************
 函数名称：    void Lin_Host_Receive_Data(u8 len)
 功能描述：    接收帧数据配置函数
 输入参数：    len:接收字节数
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2022/12/6    V1.0          HuangMG            创建
 *******************************************************************************/
void Lin_Host_Receive_Data(u8 len)
{
  LIN_ReceiveTypeStruct.Receive_Len = len;        //设置接收数据长度
	LIN_ReceiveTypeStruct.FrameReceiveOverFlag = 0; 
	LIN_ReceiveTypeStruct.DataReceiveflag = 0;
//	stru_ReceiveFaults.R = 0; 
	LIN_ReceiveTypeStruct.Host_SendBreak_flag = 0;
	LIN_ReceiveTypeStruct.RX_Data_Flag = 1;
}



