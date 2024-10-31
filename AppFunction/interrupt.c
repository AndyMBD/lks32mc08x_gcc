/*******************************************************************************
 * ��Ȩ���� (C)2015, LINKO SEMICONDUCTOR Co.ltd
 *
 * �ļ����ƣ� interrupt.c
 * �ļ���ʶ��
 * ����ժҪ�� �жϷ�������ļ�
 * ����˵���� ��
 * ��ǰ�汾�� V 1.0
 * ��    �ߣ� Howlet
 * ������ڣ� 2015��11��5��
 *
 * �޸ļ�¼1��
 * �޸����ڣ�2021��8��27��
 * �� �� �ţ�V 1.0
 * �� �� �ˣ�HMG
 * �޸����ݣ�����

 *
 *******************************************************************************/
#include "hardware_config.h"
#include "SEGGER_RTT.h"
#include "lks32mc08x_lin.h"

#if (RTT_FUNCTION == FUNCTION_ON)
struct {
    s16 data0;
    s16 data1;
    s16 data2;
} Rttstru;
#endif

u8 Test_flag = 0;
s16 GET_BUS_CURR_ADC = 0;
s16 GET_BUS_VOL_ADC = 0;
s16 GET_5V_VOL_ADC = 0;

/*******************************************************************************
 �������ƣ�    void PWRDN_IRQHandler(void)
 ����������    ��ԴǷѹ�жϺ���
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2023/10/19      V1.0           HuangMG           ����
 *******************************************************************************/
void PWRDN_IRQHandler(void)
{
  SYS_VolSelModuleIRQ(); 
}

/*******************************************************************************
 �������ƣ�    void ADC0_IRQHandler(void)
 ����������    ADC0�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2021/8/27      V1.0           HMG               ����
 *******************************************************************************/
void ADC0_IRQHandler(void)
{
		ADC0_IF = BIT0 | BIT1; //�����һ�β�������жϱ�־λ
	
    struTaskScheduler.bPWM_UpdateFlg = 1;     /* PWM���±�־ */ 
	
	  GET_BUS_CURR_ADC = GET_BUS_CURR_ADC_RESULT;
		GET_BUS_VOL_ADC = GET_BUS_VOL_ADC_RESULT;
		GET_5V_VOL_ADC = GET_5V_VOL_ADC_RESULT;
	
    if((MCPWM_EIF & BIT4) || (MCPWM_EIF & BIT5))//MCPWM_FALL�¼� Ӳ�������ж�
		{
       MCPWM_EIF = BIT4|BIT5;
			 PWMOutputs(DISABLE);
       Test_flag = 1;
		}
		
	  #if(RTT_FUNCTION == FUNCTION_ON)
    {
        Rttstru.data0 = (GET_BUS_CURR_ADC); 
        Rttstru.data1 = (GET_BUS_VOL_ADC);     
        Rttstru.data2 = (GET_5V_VOL_ADC);  
        
        SEGGER_RTT_Write(1, &Rttstru, 6);
    }
    #endif
}

/*******************************************************************************
 �������ƣ�    void TIMER0_IRQHandler(void)
 ����������    TIMER0�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void UTIMER0_IRQHandler(void)
{
    /* ʱ��500us */
    UTIMER_IF |= TIMER_IF_ZERO;

    struTaskScheduler.bTimeCnt1ms++;
    struTaskScheduler.nTimeCnt10ms ++;
    struTaskScheduler.nTimeCnt500ms++;
}

/*******************************************************************************
 �������ƣ�    void UART0_IRQHandler(void)
 ����������    UART1�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
#if 1
extern u8 led_flag;
extern u8 LIN_TX_BUFF[8];
void UART1_IRQHandler(void)
{
	u8 ReceiveData = 0;
	u8 t_Enhance_Check , t_Classical_Check;
	static u8 DtRProcess = 0;
	if (LIN_InitTypeStruct.UARTx->IF & UART_IF_RcvOver) //��������ж�
	{
		 LIN_InitTypeStruct.UARTx->IF = UART_IF_RcvOver; //���������ɱ�־λ 
		 switch(LIN_ReceiveTypeStruct.DataProcess)
     {
		   case 0x00: //֡ͷ��֡ͬ�������ͬ����
			 {
			   if(((LIN_InitTypeStruct.UARTx->IF & UART_IF_StopError) || LIN_ReceiveTypeStruct.Host_SendBreak_flag) && (LIN_InitTypeStruct.UARTx->BUFF == 0))/*�ж�ͬ����*/
				 {
				     LIN_InitTypeStruct.TIMERx->CNT = 1;                    //���㶨ʱ������ֵ
						 TIM_TimerCmd(LIN_InitTypeStruct.TIMERx, ENABLE);       /*ģ��ʹ�� */

					   LIN_InitTypeStruct.UARTx->IF = UART_IF_StopError;//���ֹͣλ�����־λ
             LIN_ReceiveTypeStruct.DataProcess = 0x01 ;		
				 }
				 break ;
			 }
			 case 0x01:
			 {
				 LIN_InitTypeStruct.TIMERx->CNT = 1;                    //���㶨ʱ������ֵ
				 LIN_ReceiveTypeStruct.Host_SendBreak_flag = 0;     //�������ͬ������α�־λ
				 ReceiveData = LIN_InitTypeStruct.UARTx->BUFF;//��ȡ���յ�������
/**********************************ͬ����ֹͣλ�����޸�****************************************/
				 if((ReceiveData != 0x55) || (LIN_InitTypeStruct.UARTx->IF & UART_IF_StopError))//ͬ���δ���
					{
						LIN_ReceiveTypeStruct.DataProcess = 0x00;
						stru_ReceiveFaults.B.SyncSegment = 1; //ͬ���δ���
						LIN_InitTypeStruct.UARTx->IF = UART_IF_StopError;//���ֹͣλ�����־λ
						break;
					}
/*********************************************************************************************/
					
					if(ReceiveData == 0x55)//���յ�ͬ����
					{
						LIN_ReceiveTypeStruct.DataProcess = 0x02 ;	
						DtRProcess = 0;								
						break;
					}
			 }
			 case 0x02: //֡PID�κ������ڴӻ�Ӧ������
			 {	    
				  LIN_InitTypeStruct.TIMERx->CNT = 1;                    //���㶨ʱ������ֵ
					LIN_ReceiveTypeStruct.ReceivePID = LIN_InitTypeStruct.UARTx->BUFF;
					LIN_ReceiveTypeStruct.ReceiveID = LIN_ReceiveTypeStruct.ReceivePID&0x3f;
/**********************************PID��ֹͣλ�����޸�****************************************/
					if((LIN_ReceiveTypeStruct.ReceivePID != Lin_CheckPID(LIN_ReceiveTypeStruct.ReceiveID)) || (LIN_InitTypeStruct.UARTx->IF & UART_IF_StopError))
					{
						LIN_InitTypeStruct.UARTx->IF = UART_IF_StopError;//���ֹͣλ�����־λ
						stru_ReceiveFaults.B.PIDError = 1; //PID����
						LIN_ReceiveTypeStruct.DataProcess = 0x00;
						break;
					}
/*********************************************************************************************/	
					
/****************************�û��Զ�����*********************************************/			
					
					
					if(LIN_ReceiveTypeStruct.ReceiveID == 0x02)				 // ����IDΪ0x02ʱ���ӻ���Ҫ�����ź�
					{
		
						LIN_TX_BUFF[0] = (GET_BUS_CURR_ADC & 0xff);
						LIN_TX_BUFF[1] = ((GET_BUS_CURR_ADC>>8) & 0xff);
						
						LIN_TX_BUFF[2] = (GET_BUS_VOL_ADC & 0xff);
						LIN_TX_BUFF[3] = ((GET_BUS_VOL_ADC>>8) & 0xff);	
						
						LIN_TX_BUFF[4] = (GET_5V_VOL_ADC & 0xff);
						LIN_TX_BUFF[5] = ((GET_5V_VOL_ADC>>8) & 0xff);	
						
						LIN_TX_BUFF[6] = Test_flag;
						TIM_TimerCmd(LIN_InitTypeStruct.TIMERx, DISABLE); /*�ر�ģ��ʹ�� */ 
						Lin_SendAnswer(LIN_ReceiveTypeStruct.ReceiveID,LIN_TX_BUFF,8,Enhance_Check);// ���յ�ID��0x02��������Ӧ������
						
						LIN_ReceiveTypeStruct.TX_Data = LIN_ReceiveTypeStruct.ReceivePID;           //ֻ������Ӧ��ʱPID����У��ʹ��
					}
					if(LIN_ReceiveTypeStruct.ReceiveID == 0x03)
					{ 
					   Lin_Host_Receive_Data(8);
					}
					
/*************************************************************************************/
					LIN_ReceiveTypeStruct.DataReceiveflag = 1;         //֡���ݽ��ձ�־λ
					if(LIN_ReceiveTypeStruct.RX_Data_Flag)             //��������ģʽ
					{
						LIN_ReceiveTypeStruct.RX_Data_Flag = 0;
					  LIN_ReceiveTypeStruct.DataProcess = 0x03 ;
					}else{
						LIN_ReceiveTypeStruct.DataProcess = 0x00;
						TIM_TimerCmd(LIN_InitTypeStruct.TIMERx, DISABLE); /*�ر�ģ��ʹ�� */ 
					}
					break ;
			 }
			 case 0x03: //���ݽ���
			 {
/**********************************���ݳ�ֹͣλ�����޸�****************************************/
				  if(LIN_InitTypeStruct.UARTx->IF & UART_IF_StopError)
					{
					   LIN_InitTypeStruct.UARTx->IF = UART_IF_StopError;//���ֹͣλ�����־λ
						 stru_ReceiveFaults.B.DataCheckSumError = 1; //���ݻ�У��ʹ���
						 LIN_ReceiveTypeStruct.DataProcess = 0x00;
					}
/*********************************************************************************************/				

					if(DtRProcess < LIN_ReceiveTypeStruct.Receive_Len)
					{
						LIN_InitTypeStruct.TIMERx->CNT = 1;                    //���㶨ʱ������ֵ
						LIN_ReceiveTypeStruct.LinReceiveData[DtRProcess] = LIN_InitTypeStruct.UARTx->BUFF ;
						DtRProcess += 1 ;
						if(DtRProcess == LIN_ReceiveTypeStruct.Receive_Len)
						{
							DtRProcess = 0 ;
							LIN_ReceiveTypeStruct.DataProcess = 0x04 ;	
						}
			    }
					break;
			 }
			 case 0x04://У���
			 {   
				  TIM_TimerCmd(LIN_InitTypeStruct.TIMERx, DISABLE); /*�ر�ģ��ʹ�� */ 
				 
					t_Classical_Check = Lin_Checksum(LIN_ReceiveTypeStruct.ReceiveID,LIN_ReceiveTypeStruct.LinReceiveData,LIN_ReceiveTypeStruct.Receive_Len,Classical_Check);/*����У��*/
					t_Enhance_Check = Lin_Checksum(LIN_ReceiveTypeStruct.ReceiveID,LIN_ReceiveTypeStruct.LinReceiveData,LIN_ReceiveTypeStruct.Receive_Len,Enhance_Check);    /*��ǿУ��*/
					LIN_ReceiveTypeStruct.ReceiveCheckSum = LIN_InitTypeStruct.UARTx->BUFF ;
					if(LIN_ReceiveTypeStruct.ReceiveCheckSum == t_Classical_Check)
					{
							LIN_ReceiveTypeStruct.CheckSum_type = Classical_Check;//������У��
					}else if(LIN_ReceiveTypeStruct.ReceiveCheckSum == t_Enhance_Check)
					{
							LIN_ReceiveTypeStruct.CheckSum_type = Enhance_Check;  //��ǿ��У��
					}else{
						stru_ReceiveFaults.B.DataCheckSumError = 1; //���ݻ�У��ʹ���
						LIN_ReceiveTypeStruct.DataProcess = 0x00 ;
						break;
					}
					LIN_ReceiveTypeStruct.FrameReceiveOverFlag = 1 ;
					LIN_ReceiveTypeStruct.DataProcess = 0x00 ;				
					break;
			 }	
		 }	
  		 
	}
	  LIN_InitTypeStruct.UARTx->IF = UART_IF_StopError;//���ֹͣλ�����־λ
}
#endif
/*******************************************************************************
 �������ƣ�    void MCPWM_IRQHandler(void)
 ����������    MCPWM�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
int MCPWM_CH0_P = 0;
int MCPWM_CH0_N = 0;
void MCPWM_IRQHandler(void)
{

}

/*******************************************************************************
 �������ƣ�    void HALL_IRQHandler(void)
 ����������    HALL�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void HALL_IRQHandler(void)
{
}



/*******************************************************************************
 �������ƣ�    void TIMER1_IRQHandler(void)
 ����������    TIMER1�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void UTIMER1_IRQHandler(void)
{
}

/*******************************************************************************
 �������ƣ�    void TIMER2_IRQHandler(void)
 ����������    TIMER2�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void UTIMER2_IRQHandler(void)
{
	if (UTIMER_IF & BIT6) //�ж�UTimer0�Ƿ��������ж�
	{					  //�жϽ���һ��
		UTIMER_IF = BIT6; //���UTimer�жϱ�־λ
	}
	if (UTIMER_IF & BIT7) //�ж�UTimer0�Ƿ��������ж�
	{					  //�жϽ���һ��
		UTIMER_IF = BIT7; //���UTimer�жϱ�־λ
	}
	if (UTIMER_IF & BIT8) //�ж�UTimer0�Ƿ��������ж�
	{					  //�жϽ���һ��
		UTIMER_IF = BIT8; //���UTimer�жϱ�־λ
	}
}

/*******************************************************************************
 �������ƣ�    void TIMER3_IRQHandler(void)
 ����������    TIMER3�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void UTIMER3_IRQHandler(void)
{
	if (UTIMER_IF & BIT9) //�ж�UTimer3�Ƿ��������ж�
	{					  //�жϽ���һ��
		UTIMER_IF = BIT9; //���UTimer�жϱ�־λ
	}
	if (UTIMER_IF & BIT10) //�ж�UTimer3�Ƿ��������ж�
	{					   //�жϽ���һ��
		UTIMER_IF = BIT10; //���UTimer�жϱ�־λ
	}
	if (UTIMER_IF & BIT11) //�ж�UTimer3�Ƿ��������ж�
	{					   //�жϽ���һ��
		UTIMER_IF = BIT11; //���UTimer�жϱ�־λ
	}
}
/*******************************************************************************
 �������ƣ�    void ENCODER0_IRQHandler(void)
 ����������    ENCODER1�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void ENCODER0_IRQHandler(void)
{
}

/*******************************************************************************
 �������ƣ�    void ENCODER0_IRQHandler(void)
 ����������    ENCODER1�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void ENCODER1_IRQHandler(void)
{
}

/*******************************************************************************
 �������ƣ�    void CMP_IRQHandler(void)
 ����������    �Ƚ����жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void CMP_IRQHandler(void)
{
	CMP_IF = BIT0 | BIT1;

	if (CMP_IF & (BIT1))
	{
		volatile u8 t_bi, t_bcnt;

		t_bcnt = 0;
		for (t_bi = 0; t_bi < 5; t_bi++)
		{
			if (SYS_AFE_CMP & BIT15) /* BIT14 CMP0 OUT Flag| BIT15 CMP1 OUT Flag */
			{
				t_bcnt++;
			}
		}

		if (t_bcnt > 3)
		{
			Test_flag = 2;
			PWMOutputs(DISABLE);
			MCPWM_EIF = BIT4 | BIT5;
		}
	}
}

/*******************************************************************************
 �������ƣ�    void UART0_IRQHandler(void)
 ����������    UART1�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void UART0_IRQHandler(void)
{
}

/*******************************************************************************
 �������ƣ�    void SysTick_Handler(void)
 ����������    ϵͳ�δ�ʱ�ж�
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void SysTick_Handler(void)
{
}

/*******************************************************************************
 �������ƣ�    void SleepTimer_IRQHandler(void)
 ����������    ���߻����жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void SleepTimer_IRQHandler(void)
{
	while (1)
		;
}

/*******************************************************************************
 �������ƣ�    void GPIO_IRQHandler(void)
 ����������    GPIO�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void GPIO_IRQHandler(void)
{
}

/*******************************************************************************
 �������ƣ�    void I2C0_IRQHandler(void)
 ����������    I2C0�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void I2C0_IRQHandler(void)
{
}

/*******************************************************************************
 �������ƣ�    void SPI0_IRQHandler(void)
 ����������    SPI0�жϴ�������
 ���������    ��
 ���������    ��
 �� �� ֵ��    ��
 ����˵����
 �޸�����      �汾��          �޸���            �޸�����
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          ����
 *******************************************************************************/
void SPI0_IRQHandler(void)
{
}

/************************ (C) COPYRIGHT LINKO SEMICONDUCTOR **********************/
/* ------------------------------END OF FILE------------------------------------ */
