/*******************************************************************************
 * 版权所有 (C)2015, LINKO SEMICONDUCTOR Co.ltd
 *
 * 文件名称： hardware_config.h
 * 文件标识：
 * 内容摘要： 硬件相关文件
 * 其它说明： 无
 * 当前版本： V 1.0
 * 作    者： Howlet
 * 完成日期： 2020年8月20日
 *
 * 修改记录1：
 * 修改日期： 2020年8月20日
 * 版 本 号： V 2.0
 * 修 改 人： Howlet
 * 修改内容： 创建
 *
 *******************************************************************************/

/*------------------------------prevent recursive inclusion -------------------*/
#ifndef __HARDWARE_CONFIG_H_
#define __HARDWARE_CONFIG_H_

#include "hardware_init.h"


#define P_HIGH__N_HIGH                   0                 /* 预驱预动极性设置 上管高电平有效，下管高电平有效 */
#define P_HIGH__N_LOW                    1                 /* 预驱预动极性设置 上管高电平有效，下管低电平有效 */

#define  CHIP_PART_NUMBER              LKS32MC086D         /* 芯片型号选择，选择不正确将影响芯片模块的初始化 */

#if ((CHIP_PART_NUMBER == LKS32MC084D)||(CHIP_PART_NUMBER == LKS32MC086D)||(CHIP_PART_NUMBER == LKS32MC087D)) 
#define  MCPWM_SWAP_FUNCTION           1
#define  PRE_DRIVER_POLARITY           P_HIGH__N_HIGH     /* 预驱预动极性设置 上管高电平有效，下管高电平有效 */
#else
#define  PRE_DRIVER_POLARITY           P_HIGH__N_LOW      /* 预驱预动极性设置 上管高电平有效，下管低电平有效 */
#endif

/* ----------------------PWM 频率及死区定义----------------------------------- */
#define MCU_MCLK                       (96000000LL)       /* PWM模块运行主频 */
#define PWM_MCLK                       ((u32)MCU_MCLK)    /* PWM模块运行主频 */
#define PWM_PRSC                       ((u8)0)            /* PWM模块运行预分频器 */
#define PWM_FREQ                       ((u16)16000)       /* PWM斩波频率 */


/* 电机控制PWM 周期计数器值 */
#define PWM_PERIOD                     ((u16) (PWM_MCLK / (u32)(2 * PWM_FREQ *(PWM_PRSC+1))))
/* PFC控制PWM 周期计数器值 */
#define PFC_PERIOD                     ((u16) (PWM_MCLK / (u32)(2 * PFC_FREQ *(PWM_PRSC+1))))

#define DEADTIME_NS                    ((u16)1200)       /* 死区时间 */
#define DEADTIME                       (u16)(((unsigned long long)PWM_MCLK * (unsigned long long)DEADTIME_NS)/1000000000uL)
  
#define DEADTIMECOMPVOLTAGE            (u16)(DEADTIME_NS/(1000000000.0/PWM_FREQ)*MAX_MODULE_VALUE)  

                                                                                
/* --------------------------------ADC通道号定义------------------------------ */
#define ADC0_CHANNEL_OPA0              ADC_CHANNEL_0
#define ADC0_CHANNEL_OPA1              ADC_CHANNEL_1
#define ADC0_CHANNEL_OPA2              ADC_CHANNEL_2
#define ADC0_CHANNEL_OPA3              ADC_CHANNEL_3

#define ADC1_CHANNEL_OPA0              ADC_CHANNEL_0
#define ADC1_CHANNEL_OPA1              ADC_CHANNEL_1
#define ADC1_CHANNEL_OPA2              ADC_CHANNEL_2
#define ADC1_CHANNEL_OPA3              ADC_CHANNEL_3

/* ADC相电流采样时序，硬件相关 ------------------------------------------------ */
/* Porting Application Notice 注意采样序列 ------------------------------------ */

#define ADC_1SHUNT_CURR_CH             (ADC0_CHANNEL_OPA3)  /* 单电阻采样电流通道 */

#define ADC_BUS_VOL_CHANNEL            (ADC_CHANNEL_15)      /* 母线电压ADC采样通道 */
#define ADC_5v_VOL_CHANNEL             (ADC_CHANNEL_10)  /* 母线电流ADC采样通道 */

#define ADC0_TEMP_CHANNEL              (ADC_CHANNEL_7)      /* 温度检测 */

#define BEMF_CH_A                      ADC_CHANNEL_17       /* A相反电势检测ADC通道号 */
#define BEMF_CH_B                      ADC_CHANNEL_16       /* B相反电势检测ADC通道号 */
#define BEMF_CH_C                      ADC_CHANNEL_15       /* C相反电势检测ADC通道号 */

/* 电流内环相电流采样，ADC通道采样结果宏定义 */
#if (CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_1SHUNT)
#define GET_ADC0_DATA(value)           {value.nData0 = (s16)ADC0_DAT0;}
#define GET_ADC1_DATA(value)           {value.nData0 = (s16)ADC0_DAT2;}
#else
#define GET_ADC0_DATA(value)           {value.nData0 = (s16)ADC0_DAT0;}
#define GET_ADC1_DATA(value)           {value.nData0 = (s16)ADC0_DAT1;}
#endif

/* 母线电流ADC通道采样结果宏定义 */
#define GET_BUS_CURR_ADC_RESULT         (ADC0_DAT0)

/* 母线电压ADC通道采样结果宏定义 */
#define GET_BUS_VOL_ADC_RESULT          (ADC0_DAT1)

/* 5V电压ADC通道采样结果宏定义 */
#define GET_5V_VOL_ADC_RESULT           (ADC0_DAT2)

#define ADC_STATE_RESET()              {ADC0_CFG |= BIT2;}   /* ADC0 状态机复位,用以极限情况下确定ADC工作状态 */
#define ADC_SOFTWARE_TRIG_ONLY()       {ADC0_TRIG = 0;}      /* ADC设置为仅软件触发 */


/* ------------------------------PGA操作相关定义 ------------------------------ */                                                                                 
#define OPA0_GIAN                      (PGA_GAIN_20)
#define OPA1_GIAN                      (PGA_GAIN_20 << 2)
#define OPA2_GIAN                      (PGA_GAIN_20 << 4)
#define OPA3_GIAN                      (PGA_GAIN_20 << 6)

/* ------------------------------DAC操作相关定义 ------------------------------ */
#define DAC_RANGE_1V2                  1                   /* DAC 1.2V量程 */
#define DAC_RANGE_3V0                  0                   /* DAC 3.0V量程 */
#define DAC_RANGE_4V85                 2                   /* DAC 4.85V量程 */

#define Pre_Drive_VCC_ENABLE    (GPIO2_PDO |= BIT1)
#define Pre_Drive_VCC_DISABLE   (GPIO2_PDO &= ~BIT1)

#define Sensor_5V_ENABLE        (GPIO2_PDO |= BIT3)
#define Sensor_5V_DISABLE       (GPIO2_PDO &= ~BIT3)

#define BAT_MON_ENABLE          (GPIO2_PDO |= BIT6)
#define BAT_MON_DISABLE         (GPIO2_PDO &= ~BIT6)

#define LO3_ENABLE              (GPIO1_PDO |= BIT9)
#define LO3_DISABLE             (GPIO1_PDO &= ~BIT9)

typedef struct
{
    u16 nTimeBaseFlg;        /* Timer 中断标记，取值 0或1 */
    u8  bPWM_UpdateFlg;      /* PWM周期更新标志，一次间隔为一个PWM周期 */
    u8  bTimeCnt1ms;         /* 1mS计数器 */
    u16 nTimeCnt10ms;        /* 10mS计数器 */
    u16 nTimeCnt500ms;       /* 500mS计数器 */
    const char* sVersion;    /* 程序版本号 */
} stru_TaskSchedulerDef;

extern stru_TaskSchedulerDef struTaskScheduler;    /* 任务调度结构体 */

#define TASK_SCHEDU_1MS                   (2)                                      /* 任务调度1ms计数时长 */
#define TASK_SCHEDU_10MS                  (20)                                     /* 任务调度1ms计数时长 */
#define TASK_SCHEDU_500MS                 (1000)    

/* 任务调度500ms计数时长 */
#define FUNCTION_ON                    1
#define FUNCTION_OFF                   0

#define RTT_FUNCTION                   FUNCTION_ON   /* RTT 调试功能 */


#endif
/************************ (C) COPYRIGHT LINKO SEMICONDUCTOR **********************/
/* ------------------------------END OF FILE------------------------------------ */
 
