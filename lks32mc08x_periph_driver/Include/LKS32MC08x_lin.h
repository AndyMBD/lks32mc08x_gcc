#ifndef __LKS32MC08x_LIN_H
#define __LKS32MC08x_LIN_H

#include "lks32mc08x_lib.h"

typedef struct
{
	u16 Baud_rate;
	UART_TypeDef* UARTx;
  TIM_TimerTypeDef *TIMERx;
	u32 t_DIVH;
	u32 t_DIVL;
	u32 divCoefficient;
	u32 clkDivUART;
} LIN_InitTypeDef;


typedef struct
{
	u8 ReceivePID;           //接收数据PID
	u8 ReceiveID;            //接收数据ID号
	u8 LinReceiveData[8];    //接收数据信0~8字节息
	u8 ReceiveCheckSum;      //接收数据校验和
	u8 Receive_Len;          //接收数据长度设置
	u8 DataReceiveflag;      //帧数据接收标志位
	u8 FrameReceiveOverFlag; //帧接收结束标志位
	u8 Host_SendBreak_flag;  //发送同步间隔段标志位
	u8 CheckSum_type;        //接收帧校验和类型
	u8 RX_Data_Flag;         //数据接收标志
	u16 TX_Data;              //主机发送数据同时接收回来的数据
	volatile u8 DataProcess;  //接收数据状态机
	u8 Receive_SyncSegment_flag;//接收同步段标志位
} LIN_ReceiveTypeDef;

typedef union
{
    u16 R;
    struct
    { 
			  u16 SendBreakError     : 1;//同步间隔段错误标志位 1
        u16 SyncSegment        : 1;//同步段0x55错误标志位 2
        u16 PIDError           : 1;//PID错误标志位        4
        u16 DataError          : 1;//数据0错误标志位      8    
        u16 CheckSumError      : 1;//校验和错误标志位     16	
        u16                    : 11;//保留，可根据其他需求来添加错误标志位，注意不要超过16位，否则需要修改u16 R --> u32 R
    } B;
} stru_SendFaultStatus_t;

typedef union
{
    u16 R;
    struct
    {
        u16 SendBreakError     : 1;//同步间隔段错误标志位       1
        u16 SyncSegment        : 1;//同步段0x55错误标志位       2
        u16 PIDError           : 1;//PID错误标志位              4
        u16 DataCheckSumError  : 1;//接收数据或校验和错误标志位  8 
			  u16 Reveice_over_time  : 1;//接收数据超时标志位         16 	
        u16                    : 11;//保留，可根据其他需求来添加错误标志位，注意不要超过16位，否则需要修改u16 R --> u32 R
    } B;
} stru_ReceiveFaultStatus_t;

extern stru_SendFaultStatus_t stru_SendFaults;
extern stru_ReceiveFaultStatus_t stru_ReceiveFaults;

#define Classical_Check  0 /*经典校验*/
#define Enhance_Check    1 /*增强校验*/

extern LIN_InitTypeDef LIN_InitTypeStruct;
extern LIN_ReceiveTypeDef LIN_ReceiveTypeStruct;

extern void Lin_init(UART_TypeDef* UARTx , u16 Baud_rate,TIM_TimerTypeDef *TIMERx);
extern int Delay_ms(TIM_TimerTypeDef *TIMERx, u32 ms , u8 resetTim);
extern u8 Lin_CheckPID(u8 id);
extern u8 Lin_Checksum(u8 id , u8 data[] ,u8 len ,u8 id_type);
extern int Lin_SendHead(u8 id);
extern int Lin_SendAnswer(u8 id ,u8 data[],u8 len,u8 id_type);
extern void Lin_Host_Receive_Data(u8 len);
extern void Lin_TIMEIRQ(void);
extern LIN_ReceiveTypeDef LIN_ReceiveTypeStruct;
#endif




