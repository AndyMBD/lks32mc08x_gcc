/*******************************************************************************
 * ??????? (C)2015, LINKO SEMICONDUCTOR Co.ltd
 *
 * ???????? Main.c
 * ????????
 * ???????? ??????????
 * ????????? ??
 * ????��?? V 1.0
 * ??    ??? ????
 * ???????? 2015??11??5??
 *
 * ?????1??
 * ????????2021??8??27??
 * ?? ?? ???V 1.0
 * ?? ?? ???HMG
 * ????????????
 *
 *******************************************************************************/
#include "hardware_config.h"
#include "SEGGER_RTT.h"
#include "lks32mc08x_lin.h"
#include "hardware_init.h"

extern s16 GET_BUS_CURR_ADC;
extern s16 GET_BUS_VOL_ADC;
extern s16 GET_5V_VOL_ADC;

/*******************************************************************************
 ?????????    int main(void)
 ??????????    ?????????
 ?????????    ??
 ?????????    ??
 ?? ?? ???    ??
 ?????????    ?????????????????MCPWM??T0??T1????ADC????????????��?????
			        ??????��?
 ????��?????  ????DEBUG??????ADC?��??????Voltage_Value????????��V
 ???????      ?��??          ?????            ???????
 -----------------------------------------------------------------------------
 2021/2/25
 *******************************************************************************/
float f_BUS_CURR_ADC = 0;
float f_BUS_VOL_ADC = 0;
float f_5V_VOL_ADC = 0;
u8 LIN_TX_BUFF[8] = {0x78, 0x22, 0x33, 0x11, 0x44, 0x66, 0x77, 0x88};
u8 LIN_RX_0 = 0;
extern u8 Test_flag;
u8 test_data_1,test_data_2,test_data_3,test_data_4;
int main(void)
{
	Hardware_init();   /* ???????? */
	while (1)
	{
		
		
    if (struTaskScheduler.bTimeCnt1ms >= TASK_SCHEDU_1MS)
    {   /* 1???????????????? */
        struTaskScheduler.bTimeCnt1ms = 0;
			
			  f_BUS_CURR_ADC = GET_BUS_CURR_ADC * 3.6/32752;
			  f_BUS_VOL_ADC = GET_BUS_VOL_ADC * 3.6/32752;
			  f_5V_VOL_ADC = GET_5V_VOL_ADC * 3.6/32752;
		test_data_1++;
		if (test_data_1>250)
		{
			test_data_1=0;
		}
	}
		
		if (struTaskScheduler.nTimeCnt10ms >= TASK_SCHEDU_10MS)
    {   /* 10???????????????? */
        struTaskScheduler.nTimeCnt10ms = 0;
			
			  
		}
		if (struTaskScheduler.nTimeCnt500ms >= TASK_SCHEDU_500MS)
    {   /* 500???????????????? */
        struTaskScheduler.nTimeCnt500ms = 0;
        
    }
		
		if(struTaskScheduler.bPWM_UpdateFlg)
    {   /* ???PWM?????????? */
        struTaskScheduler.bPWM_UpdateFlg = 0;
			
			if(LIN_ReceiveTypeStruct.FrameReceiveOverFlag)//?????????
			{
			  LIN_ReceiveTypeStruct.FrameReceiveOverFlag = 0;
				LIN_RX_0 = LIN_ReceiveTypeStruct.LinReceiveData[0];//?????????????????		
				
			 
		   if(Test_flag == 0)		
			 {
					if(LIN_ReceiveTypeStruct.LinReceiveData[0] == 1)
					{
						 if(LIN_ReceiveTypeStruct.LinReceiveData[1])
						 {
							 Motor_CCW(96);
						 }else{
							 Motor_CW(96);
						 }
					}else if(LIN_ReceiveTypeStruct.LinReceiveData[0] == 0){
						PWMOutputs(DISABLE);
					}else if(LIN_ReceiveTypeStruct.LinReceiveData[0] == 2){
						 Motor_BRAKE();
					}
			 }
			 
			 if(LIN_ReceiveTypeStruct.LinReceiveData[2])
			 {
			    Test_flag = 0;
			 }
			 
			}
		}
	}
}

/************************ (C) COPYRIGHT LINKO SEMICONDUCTOR **********************/
/* ------------------------------END OF FILE------------------------------------ */
