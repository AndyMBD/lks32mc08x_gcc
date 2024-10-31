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
	u8 ReceivePID;           //��������PID
	u8 ReceiveID;            //��������ID��
	u8 LinReceiveData[8];    //����������0~8�ֽ�Ϣ
	u8 ReceiveCheckSum;      //��������У���
	u8 Receive_Len;          //�������ݳ�������
	u8 DataReceiveflag;      //֡���ݽ��ձ�־λ
	u8 FrameReceiveOverFlag; //֡���ս�����־λ
	u8 Host_SendBreak_flag;  //����ͬ������α�־λ
	u8 CheckSum_type;        //����֡У�������
	u8 RX_Data_Flag;         //���ݽ��ձ�־
	u16 TX_Data;              //������������ͬʱ���ջ���������
	volatile u8 DataProcess;  //��������״̬��
	u8 Receive_SyncSegment_flag;//����ͬ���α�־λ
} LIN_ReceiveTypeDef;

typedef union
{
    u16 R;
    struct
    { 
			  u16 SendBreakError     : 1;//ͬ������δ����־λ 1
        u16 SyncSegment        : 1;//ͬ����0x55�����־λ 2
        u16 PIDError           : 1;//PID�����־λ        4
        u16 DataError          : 1;//����0�����־λ      8    
        u16 CheckSumError      : 1;//У��ʹ����־λ     16	
        u16                    : 11;//�������ɸ���������������Ӵ����־λ��ע�ⲻҪ����16λ��������Ҫ�޸�u16 R --> u32 R
    } B;
} stru_SendFaultStatus_t;

typedef union
{
    u16 R;
    struct
    {
        u16 SendBreakError     : 1;//ͬ������δ����־λ       1
        u16 SyncSegment        : 1;//ͬ����0x55�����־λ       2
        u16 PIDError           : 1;//PID�����־λ              4
        u16 DataCheckSumError  : 1;//�������ݻ�У��ʹ����־λ  8 
			  u16 Reveice_over_time  : 1;//�������ݳ�ʱ��־λ         16 	
        u16                    : 11;//�������ɸ���������������Ӵ����־λ��ע�ⲻҪ����16λ��������Ҫ�޸�u16 R --> u32 R
    } B;
} stru_ReceiveFaultStatus_t;

extern stru_SendFaultStatus_t stru_SendFaults;
extern stru_ReceiveFaultStatus_t stru_ReceiveFaults;

#define Classical_Check  0 /*����У��*/
#define Enhance_Check    1 /*��ǿУ��*/

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




