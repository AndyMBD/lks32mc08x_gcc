/*******************************************************************************
 * 版权所有 (C)2015, LINKO SEMICONDUCTOR Co.ltd
 *
 * 文件名称： hardware_init.c
 * 文件标识：
 * 内容摘要： 硬件初始化代码
 * 其它说明： 无
 * 当前版本： V 1.0
 * 作    者： Howlet Li
 * 完成日期： 2015年11月5日
 *
 * 修改记录1：
 * 修改日期： 2021年8月27日
 * 版 本 号：
 * 修 改 人： HMG
 * 修改内容：
 *
 *******************************************************************************/
#include "basic.h"
#include "hardware_config.h"
#include "SEGGER_RTT.h"
#include "lks32mc08x_lin.h"
#include "lks32mc08x_nvr.h"

void DAC_init(void);
void TempSensor_Init(void);
void PGA_init(void);
void CMP_init(void);
stru_TaskSchedulerDef struTaskScheduler;    /* 任务调度结构体 */

/*******************************************************************************
 函数名称：    void Hardware_init(void)
 功能描述：    硬件部分初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2021/8/27     V1.0            HMG              创建
 *******************************************************************************/
void Hardware_init(void)
{
    __disable_irq();            /* 关闭中断 中断总开关 */
    SYS_WR_PROTECT = 0x7a83;    /* 使能系统寄存器写操作*/
    FLASH_CFG |= 0x00080000;    /* enable prefetch ，FLASH预取加速使能*/
    DSP_Init();                       /* DSP模块初始化*/
	
    UTimer_init();              /* 通用计数器初始化 */
    GPIO_init();                /* GPIO初始化 */
    DAC_init();                 /* DAC 初始化 */	
    ADC0_init();                /* ADC初始化 */
    MCPWM_init();               /* MCPWM初始化 */
    PGA_init();               /* PGA 初始化 */
    CMP_init();               /* 比较器初始化 */
	  TempSensor_Init();          /* 温度传感器初始化 */
	
	  Lin_init(UART1,19200,TIMER2);    /* lin初始化,使用串口1，波特率38400，定时使用定时器（只能使用定时器2和定时器3）*/
    SoftDelay(100);             /* 等待硬件初始化完毕*/
	
	  NVIC_EnableIRQ(UART1_IRQn);      //使能UART0定时器中断
	  NVIC_EnableIRQ (TIMER0_IRQn);
	  NVIC_EnableIRQ(ADC0_IRQn);  /* 使能ADC0中断*/
//	  NVIC_EnableIRQ (CMP_IRQn);        /* 打开比较器中断 */ 
	
	  NVIC_SetPriority(UART1_IRQn, 3); //UART0中断优先级配置
	  NVIC_SetPriority(TIMER0_IRQn, 2);
    NVIC_SetPriority(ADC0_IRQn, 1); /* ADC0中断优先级配置*/
  	NVIC_SetPriority(CMP_IRQn, 0);    /* 设置CMP_IRQn中断优先级为0 | 共0，1，2，3四级中断优先级，0为最高*/
	  
		

 #if POWER_MODE	
    SYS_VolSelModuleEnableIRQ(MCU_POWER_5v0);/*MCU电源中断使能函数*/ 
	#else
		SYS_VolSelModuleEnableIRQ(MCU_POWER_3v3);/*MCU电源中断使能函数*/ 
 #endif		
 
 #if (RTT_FUNCTION == FUNCTION_ON)
    /* JScope RTT模式初始化 */
    SEGGER_RTT_ConfigUpBuffer(1, "JScope_i2i2i2", bRttBuf, 2048, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif 
 
    __enable_irq();             /* 开启中断 */
}



/*******************************************************************************
 函数名称：    void Reg_Clr(void)
 功能描述：    寄存器清零
 输入参数：    addr ：需要清零寄存器的起始地址 
               nSize：需要清零的寄存器个数
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2021/3/14      V1.0           Howlet Li          创建
 *******************************************************************************/
void Reg_Clr(u32 addr, u8 nSize)
{
   while(nSize --)
   {
      REG32(addr) = 0;
      addr += 4;
   }
}

/*******************************************************************************
 函数名称：    void Reg_Reset(void)
 功能描述：    寄存器复位
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2021/3/14      V1.0           Howlet Li          创建
 *******************************************************************************/
void Reg_Reset(void)
{
   SYS_WR_PROTECT = 0x7a83;   /* 解除系统寄存器写保护 */
   SYS_SFT_RST = 0xfff;
  
   Reg_Clr((u32)(&SYS_AFE_REG0),8); /* 清SYS_AFE_REG0~SYS_AFE_REG7 */
   Reg_Clr((u32)(&ADC0_CHN0),12);
   Reg_Clr((u32)(&ADC0_GAIN0),2);
   Reg_Clr((u32)(&ADC0_IE),6);

   Reg_Clr((u32)(&DMA_CCR0),18);
   Reg_Clr(GPIO0_BASE,12);          /* 清GPIO0 相关的12个寄存器 */
   Reg_Clr(GPIO1_BASE,12);
   Reg_Clr(GPIO2_BASE,12);
   Reg_Clr(GPIO3_BASE,12);   
   Reg_Clr(EXTI_BASE,6);
   Reg_Clr(CMP_BASE,5);
}


/*******************************************************************************
 函数名称：    void PGA_Init(void)
 功能描述：    PGA初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2016/3/15      V1.0           Howlet Li          创建
 *******************************************************************************/
void PGA_init(void)
{
    SYS_AnalogModuleClockCmd(SYS_AnalogModule_OPA3, ENABLE);
    
	  SYS_WR_PROTECT = 0x7a83;
    SYS_AFE_REG0 = OPA3_GIAN; /* 4个OPA增益设置 */
}
/*******************************************************************************
 函数名称：    void CMP_Init(void)
 功能描述：    比较器初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2016/3/15      V1.0           Howlet Li          创建
 *******************************************************************************/
void CMP_init(void)
{
    CMP_InitTypeDef CMP_InitStruct;

    CMP_StructInit(&CMP_InitStruct);

    CMP_InitStruct.CMP0_EN = DISABLE;            /* 比较器0开关 */
    CMP_InitStruct.CMP0_SELN = CMP_SELN_DAC;    /* CMP0_N 内部参考 */
    CMP_InitStruct.CMP0_SELP = CMP0_SELP_IP4;   /* CMP0_P CMP0_OPA0IP */
    CMP_InitStruct.CMP0_IN_EN = DISABLE;         /* 比较器信号输入使能 */
    CMP_InitStruct.CMP0_IE = DISABLE;            /* 比较器0信号中断使能 */

    CMP_InitStruct.CMP1_EN = ENABLE;            /* 比较器1开关 */
    CMP_InitStruct.CMP1_SELN = CMP_SELN_DAC;    /* CMP1_N 内部参考 */
    CMP_InitStruct.CMP1_SELP = CMP1_SELP_OPA3_OUT;    /* CMP1_P CMP1_IP0 */
    CMP_InitStruct.CMP1_IN_EN = ENABLE;         /* 比较器信号输入使能 */  
    CMP_InitStruct.CMP1_IE = ENABLE;            /* 比较器1信号中断使能 */
    
    CMP_InitStruct.FIL_CLK10_DIV16 = 15;        /* 即滤波宽度=tclk 周期*16*CMP_FltCnt */
    CMP_InitStruct.CLK10_EN = ENABLE;           /* 时钟使能 */

    CMP_Init(&CMP_InitStruct);

}

/*******************************************************************************
 函数名称：    void DAC_Init(void)
 功能描述：    DAC初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2016/3/23      V1.0           Howlet Li          创建
 *******************************************************************************/
void DAC_init(void)
{
    SYS_AnalogModuleClockCmd(SYS_AnalogModule_DAC, ENABLE);
    SYS_WR_PROTECT = 0x7a83;
  
    SYS_AFE_REG1 &= ~((u32)0x03 << 6);
    SYS_AFE_REG1 |= (DAC_RANGE_3V0 << 6);  /* 设置DAC满量程为1.2V；00:3V| 01:1.2V| 10:4.85V */
  
    if(((SYS_AFE_REG1 >> 6)&0x03) == DAC_RANGE_1V2)
    {/* 加载DAC 1.2V量程校正值 */
       SYS_AFE_DAC_DC = Read_Trim(0x00000344);
       SYS_AFE_DAC_AMC = Read_Trim(0x00000334);
    }
    else if(((SYS_AFE_REG1 >> 6)&0x03) == DAC_RANGE_3V0)
    { /* 加载DAC 3.0V量程校正值 */
       SYS_AFE_DAC_DC = Read_Trim(0x00000340);
       SYS_AFE_DAC_AMC = Read_Trim(0x00000330);
    }
    else if(((SYS_AFE_REG1 >> 6)&0x03) == DAC_RANGE_4V85)
    { /* 加载DAC 4.85V量程校正值 */
       SYS_AFE_DAC_DC = Read_Trim(0x00000348);
       SYS_AFE_DAC_AMC = Read_Trim(0x00000338);
    }

    SYS_AFE_DAC = 3000;                     /* 比较器负端为DAC电压为(SYS_AFE_DAC/4096)*3单位V；比较器正端为OPA3_OUT:(1.9V+(（OPA输入电压*13.25）/2))单位V*/ 
}

/*******************************************************************************
 函数名称：    void GPIO_init(void)
 功能描述：    GPIO硬件初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2015/11/5      V1.0           Howlet Li          创建
 *******************************************************************************/

void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);

    /* MCPWM P1.4~P1.9 */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIO1, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIO1, GPIO_PinSource_4, AF3_MCPWM);
    GPIO_PinAFConfig(GPIO1, GPIO_PinSource_5, AF3_MCPWM);
    GPIO_PinAFConfig(GPIO1, GPIO_PinSource_6, AF3_MCPWM);
    GPIO_PinAFConfig(GPIO1, GPIO_PinSource_7, AF3_MCPWM);

    /* 内置栅极驱动器的型号，P1.12/ P1.15/ P3.13三个IO口必须设置为输出态 */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_15;
    GPIO_Init(GPIO1, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIO3, &GPIO_InitStruct);
	
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_6;
    GPIO_Init(GPIO2, &GPIO_InitStruct);
		
		Pre_Drive_VCC_ENABLE;
		Sensor_5V_ENABLE;
		BAT_MON_ENABLE;
		LO3_DISABLE;
}

/*******************************************************************************
 函数名称：    void UTimer_init(void)
 功能描述：    UTimer硬件初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2015/11/5      V1.0           Howlet Li          创建
 *******************************************************************************/
void UTimer_init(void)
{
    TIM_TimerInitTypeDef TIM_InitStruct;
    
   

    TIM_TimerStrutInit(&TIM_InitStruct);
    TIM_InitStruct.Timer_CH0_WorkMode = TIMER_OPMode_CMP;  /* 设置Timer CH0 为比较模式 */
    TIM_InitStruct.Timer_CH0_CapMode = TIMER_CapMode_None;
    TIM_InitStruct.Timer_CH0Output = 0;                    /* 计数器回零时，比较模式输出极性控制 */
    TIM_InitStruct.Timer_CH1_WorkMode = TIMER_OPMode_CMP;  /* 设置Timer CH1 为比较模式 */
    TIM_InitStruct.Timer_CH1_CapMode = TIMER_CapMode_None;
    TIM_InitStruct.Timer_CH1Output = 0;                    /* 计数器回零时，比较模式输出极性控制 */
    TIM_InitStruct.Timer_TH = 48000;                       /* 设置计数器计数模值 */
    TIM_InitStruct.Timer_CMP0 = 24000;                     /* 设置比较模式的CH0比较值 */
    TIM_InitStruct.Timer_CMP1 = 500;
    TIM_InitStruct.Timer_Filter0 = 0;                      /* 设置捕捉模式或编码器模式下对应通道的数字滤波值 */
    TIM_InitStruct.Timer_Filter1 = 0;
    TIM_InitStruct.Timer_ClockDiv = TIM_Clk_Div1;          /* 设置Timer模块数据分频系数 */
    TIM_InitStruct.Timer_IRQEna = Timer_IRQEna_Zero;       /* 开启Timer模块过0中断 */
    TIM_TimerInit(TIMER0, &TIM_InitStruct);
     TIM_TimerCmd(TIMER0, ENABLE);                          /* Timer0 模块使能 */
}
/*******************************************************************************
 函数名称：    void ADC0_init(void)
 功能描述：    ADC0硬件初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2015/11/5      V1.0           Howlet Li          创建
 *******************************************************************************/
void ADC0_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    ADC_StructInit(&ADC_InitStructure);                          //初始化结构体
    ADC_InitStructure.Align = ADC_LEFT_ALIGN;                    //ADC数据输出左对齐
    ADC_InitStructure.UTIMER_Trigger_En = DISABLE;               //UTIMER_T0硬件触发ADC采样
    ADC_InitStructure.MCPWM_Trigger_En = ADC_MCPWM_T0_TRG;       //关闭MCPWM的硬件中断
    ADC_InitStructure.FirSeg_Ch = ADC_3_TIMES_SAMPLE;            //第一段采样的总通道数: 1~20
    ADC_InitStructure.Trigger_Cnt = 0;                           /*单段触发模式下触发一次采样所需要的事件数:0~15 0表示需要一次触发，15表示需要16次*/
    ADC_InitStructure.Trigger_Mode = ADC_1SEG_TRG;               /*ADC采样1段模式*/
    ADC_InitStructure.IE = ADC_EOS0_IRQ_EN;                     /* ADC第0段采样结束中断使能 */  
	  ADC_Init(ADC0, &ADC_InitStructure);
    ADC0_IF = 0xff;                                    //清中断标志位
    ADC0_GAIN0 = 0x0;                                  //开启2/3增益
    ADC0_CHN0 = ADC_1SHUNT_CURR_CH | (ADC_BUS_VOL_CHANNEL<< 8);  //采样通道通道1和2
    ADC0_CHN1 = ADC_5v_VOL_CHANNEL | (ADC_5v_VOL_CHANNEL<< 8); //采样通道通道3和4	
	
}



/*******************************************************************************
 函数名称：    void Clock_Init(void)
 功能描述：    时钟配置
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2023/10/23      V2.0         HuangMG        增加MCU电源检测功能
 *******************************************************************************/
void Clock_Init(void)
{
    SYS_WR_PROTECT = 0x7a83;   /* 解除系统寄存器写保护 */
    SYS_AFE_REG5 |= BIT15;     /* BIT15:PLLPDN */
	
		#if POWER_MODE
		SYS_VolSelModule(MCU_POWER_5v0);       /* MCU供电电压：1：5V，0;3.3V*/
		#else
		SYS_VolSelModule(MCU_POWER_3v3);       /* MCU供电电压：1：5V，0;3.3V*/
		#endif
	  SoftDelay(100);            /* 等待PLL稳定*/
	  SYS_WR_PROTECT = 0x7a83;       /* 解除系统寄存器写保护 */
	
    SYS_CLK_CFG = 0x000001ff; /* BIT8:0: CLK_HS,1:PLL  | BIT[7:0]CLK_DIV  | 1ff对应96M时钟 */
	  SYS_WR_PROTECT = 0x0;      /*关闭系统寄存器写操作*/
}

/*******************************************************************************
 函数名称：    void SystemInit(void)
 功能描述：    硬件系统初始化，调用时钟初始化函数
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2016/3/14      V1.0           Howlet Li          创建
 *******************************************************************************/
void SystemInit(void)
{
    Clock_Init(); /* 时钟初始化 */
}



/*******************************************************************************
 函数名称：    void SystemInit(void)
 功能描述：    硬件系统初始化，调用时钟初始化函数
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2016/3/14      V1.0           Howlet Li          创建
 *******************************************************************************/

void MCPWM_init(void)
{
    MCPWM_InitTypeDef MCPWM_InitStructure;

    MCPWM_StructInit(&MCPWM_InitStructure);
    
    MCPWM_InitStructure.CLK_DIV = 0;                          /* MCPWM时钟分频设置 */
    MCPWM_InitStructure.MCLK_EN = ENABLE;                     /* 模块时钟开启 */
    MCPWM_InitStructure.MCPWM_Cnt_EN = ENABLE;                /* 主计数器开始计数使能开关 */
    MCPWM_InitStructure.MCPWM_WorkModeCH0 = CENTRAL_PWM_MODE;
    MCPWM_InitStructure.MCPWM_WorkModeCH1 = CENTRAL_PWM_MODE; /* 通道工作模式设置，中心对齐或边沿对齐 */
    MCPWM_InitStructure.MCPWM_WorkModeCH2 = CENTRAL_PWM_MODE;

    MCPWM_InitStructure.GPIO_BKIN_Filter = 12;                /* 急停事件(来自IO口信号)数字滤波器时间设置 */
    MCPWM_InitStructure.CMP_BKIN_Filter = 12;                 /* 急停事件(来自比较器信号)数字滤波器时间设置 */

    MCPWM_InitStructure.MCPWM_PERIOD = PWM_PERIOD;            /* 计数周期设置 */
    MCPWM_InitStructure.TriggerPoint0 = (u16)(10 - PWM_PERIOD); /* MCPWM_TMR0 ADC触发事件T0 设置 */
    MCPWM_InitStructure.TriggerPoint1 = (u16)(PWM_PERIOD - 1);/* MCPWM_TMR1 ADC触发事件T1 设置 */
    MCPWM_InitStructure.DeadTimeCH0N = DEADTIME;
    MCPWM_InitStructure.DeadTimeCH0P = DEADTIME;
    MCPWM_InitStructure.DeadTimeCH1N = DEADTIME;
    MCPWM_InitStructure.DeadTimeCH1P = DEADTIME;
    MCPWM_InitStructure.DeadTimeCH2N = DEADTIME;
    MCPWM_InitStructure.DeadTimeCH2P = DEADTIME;              /* 死区时间设置 */

#if (PRE_DRIVER_POLARITY == P_HIGH__N_LOW)                    /* CHxP 高有效， CHxN低电平有效 */
    MCPWM_InitStructure.CH0N_Polarity_INV = ENABLE;           /* CH0N通道输出极性设置 | 正常输出或取反输出*/
    MCPWM_InitStructure.CH0P_Polarity_INV = DISABLE;          /* CH0P通道输出极性设置 | 正常输出或取反输出 */
    MCPWM_InitStructure.CH1N_Polarity_INV = ENABLE;
    MCPWM_InitStructure.CH1P_Polarity_INV = DISABLE;
    MCPWM_InitStructure.CH2N_Polarity_INV = ENABLE;
    MCPWM_InitStructure.CH2P_Polarity_INV = DISABLE;

    MCPWM_InitStructure.Switch_CH0N_CH0P =  DISABLE;           /* 通道交换选择设置 | CH0P和CH0N是否选择信号交换 */
    MCPWM_InitStructure.Switch_CH1N_CH1P =  DISABLE;           /* 通道交换选择设置 */
    MCPWM_InitStructure.Switch_CH2N_CH2P =  DISABLE;           /* 通道交换选择设置 */

    /* 默认电平设置 默认电平输出不受MCPWM_IO01和MCPWM_IO23的 BIT0、BIT1、BIT8、BIT9、BIT6、BIT14
                                                     通道交换和极性控制的影响，直接控制通道输出 */
    MCPWM_InitStructure.CH0P_default_output = LOW_LEVEL;
    MCPWM_InitStructure.CH0N_default_output = HIGH_LEVEL;
    MCPWM_InitStructure.CH1P_default_output = LOW_LEVEL;      /* CH1P对应引脚在空闲状态输出低电平 */
    MCPWM_InitStructure.CH1N_default_output = HIGH_LEVEL;     /* CH1N对应引脚在空闲状态输出高电平 */
    MCPWM_InitStructure.CH2P_default_output = LOW_LEVEL;
    MCPWM_InitStructure.CH2N_default_output = HIGH_LEVEL;
#else
#if (PRE_DRIVER_POLARITY == P_HIGH__N_HIGH)                    /* CHxP 高有效， CHxN高电平有效 */
    MCPWM_InitStructure.CH0N_Polarity_INV = DISABLE;           /* CH0N通道输出极性设置 | 正常输出或取反输出*/
    MCPWM_InitStructure.CH0P_Polarity_INV = DISABLE;          /* CH0P通道输出极性设置 | 正常输出或取反输出 */
    MCPWM_InitStructure.CH1N_Polarity_INV = DISABLE;
    MCPWM_InitStructure.CH1P_Polarity_INV = DISABLE;
    MCPWM_InitStructure.CH2N_Polarity_INV = DISABLE;
    MCPWM_InitStructure.CH2P_Polarity_INV = DISABLE;

    MCPWM_InitStructure.Switch_CH0N_CH0P =  DISABLE;           /* 通道交换选择设置 | CH0P和CH0N是否选择信号交换 */
    MCPWM_InitStructure.Switch_CH1N_CH1P =  DISABLE;           /* 通道交换选择设置 */
    MCPWM_InitStructure.Switch_CH2N_CH2P =  DISABLE;           /* 通道交换选择设置 */

    /* 默认电平设置 默认电平输出不受MCPWM_IO01和MCPWM_IO23的 BIT0、BIT1、BIT8、BIT9、BIT6、BIT14
                                                     通道交换和极性控制的影响，直接控制通道输出 */
    MCPWM_InitStructure.CH0P_default_output = LOW_LEVEL;
    MCPWM_InitStructure.CH0N_default_output = LOW_LEVEL;
    MCPWM_InitStructure.CH1P_default_output = LOW_LEVEL;      /* CH1P对应引脚在空闲状态输出低电平 */
    MCPWM_InitStructure.CH1N_default_output = LOW_LEVEL;     /* CH1N对应引脚在空闲状态输出高电平 */
    MCPWM_InitStructure.CH2P_default_output = LOW_LEVEL;
    MCPWM_InitStructure.CH2N_default_output = LOW_LEVEL;
#endif
#endif

    MCPWM_InitStructure.DebugMode_PWM_out = ENABLE;           /* 在接上仿真器debug程序时，暂停MCU运行时，选择各PWM通道正常输出调制信号
                                                                 还是输出默认电平，保护功率器件 ENABLE:正常输出 DISABLE:输出默认电平*/

    MCPWM_InitStructure.MCPWM_T0_UpdateEN = ENABLE;           /* MCPWM T0事件更新使能 */
    MCPWM_InitStructure.MCPWM_T1_UpdateEN = DISABLE;          /* MCPWM T1事件更新 禁止*/

#if (CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_1SHUNT)
    MCPWM_InitStructure.T1_Update_INT_EN = ENABLE;           /* T0更新事件 中断使能或关闭 */
#else
#if (CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_2SHUNT)
    MCPWM_InitStructure.T0_Update_INT_EN = DISABLE;           /* T0更新事件 中断使能或关闭 */
#else
#if ((CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_3SHUNT)||(CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_MOSFET)) 
    MCPWM_InitStructure.T0_Update_INT_EN = DISABLE;           /* T0更新事件 中断使能或关闭 */
#endif    
#endif
#endif

    MCPWM_InitStructure.FAIL0_INT_EN = DISABLE;               /* FAIL0事件 中断使能或关闭 */
    MCPWM_InitStructure.FAIL0_INPUT_EN = DISABLE;             /* FAIL0通道急停功能打开或关闭 */
    MCPWM_InitStructure.FAIL0_Signal_Sel = FAIL_SEL_CMP;      /* FAIL0事件信号选择，比较器或IO口 */
    MCPWM_InitStructure.FAIL0_Polarity = HIGH_LEVEL_ACTIVE;   /* FAIL0事件极性选择，高有效 */

    MCPWM_InitStructure.FAIL1_INT_EN = ENABLE;                /* FAIL1事件 中断使能或关闭 */
    MCPWM_InitStructure.FAIL1_INPUT_EN = ENABLE;              /* FAIL1通道急停功能打开或关闭 */
    MCPWM_InitStructure.FAIL1_Signal_Sel = FAIL_SEL_CMP;      /* FAIL1事件信号选择，比较器或IO口 */
    MCPWM_InitStructure.FAIL1_Polarity = HIGH_LEVEL_ACTIVE;   /* FAIL1事件极性选择，高有效或低有效 */ 
  
    MCPWM_Init(&MCPWM_InitStructure);
    

}



void Motor_CW(u8 duty)
{
	    u32 MCPWM_temp01 = 0 , MCPWM0_THTemp = 0;

	    MCPWM_temp01 = MCPWM_IO01;
	    MCPWM_temp01 &= ~0x3C3C; //清除软件配置
	    MCPWM_temp01 |= 0x1C;    //CH1 PWM输出，CH0上下管软件控制输出，上管关闭，下管恒通
			MCPWM_PRT = 0xDEAD;
	    MCPWM_IO01 = MCPWM_temp01;
	    MCPWM_PRT = 0;
	
	    MCPWM0_THTemp = (duty * PWM_PERIOD * 655)>>16;
	    MCPWM_TH00 = -(MCPWM0_THTemp);
	    MCPWM_TH01 =  MCPWM0_THTemp;
	    MCPWM_TH10 = -(MCPWM0_THTemp);
	    MCPWM_TH11 =  MCPWM0_THTemp;
	    PWMOutputs(ENABLE);

}

void Motor_CCW(u8 duty)
{
	    u32 MCPWM_temp01 = 0 , MCPWM0_THTemp = 0;

	    MCPWM_temp01 = MCPWM_IO01;
	    MCPWM_temp01 &= ~0x3C3C;  //清除软件配置
	    MCPWM_temp01 |= 0x1C00;   //CH0 PWM输出，CH1上下管软件控制输出，上管关闭，下管恒通
			MCPWM_PRT = 0xDEAD;
	    MCPWM_IO01 = MCPWM_temp01;
	    MCPWM_PRT = 0;
	
	    MCPWM0_THTemp = (duty * PWM_PERIOD * 655)>>16;
	    MCPWM_TH00 = -(MCPWM0_THTemp);
	    MCPWM_TH01 =  MCPWM0_THTemp;
	    MCPWM_TH10 = -(MCPWM0_THTemp);
	    MCPWM_TH11 =  MCPWM0_THTemp;
	
	
	    PWMOutputs(ENABLE);
}

void Motor_BRAKE(void)
{
	    u32 MCPWM_temp01 = 0;
	    MCPWM_temp01 = MCPWM_IO01;
	    MCPWM_temp01 &= ~0x3C3C; //A相上管PWM，A相下管软件控制输出，B相上下管软件控制输出
	    MCPWM_temp01 |= 0x1C1C;  //A相下管软件控制输出，A相下管悬空，B相上下管软件控制输出，B上管悬空，下管恒通
			MCPWM_PRT = 0xDEAD;
	    MCPWM_IO01 = MCPWM_temp01;
	    MCPWM_PRT = 0;  
	    PWMOutputs(ENABLE);
}



/*******************************************************************************
 函数名称：    void SoftDelay(void)
 功能描述：    软件延时函数
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          创建
 *******************************************************************************/
void SoftDelay(u32 cnt)
{
    volatile u32 t_cnt;

    for(t_cnt = 0; t_cnt < cnt; t_cnt++)
    {
        __NOP();
    }
}
/*******************************************************************************
 函数名称：    void Invers_GPIO(void)
 功能描述：    LED电平翻转函数
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2015/11/5      V1.0           Howlet Li          创建
 *******************************************************************************/
uint8_t Invers_GPIO(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  u16 reg;
	reg = GPIO_ReadOutputDataBit(GPIOx,GPIO_Pin);//读GPIO电平
	if(reg)
	{
		GPIO_ResetBits(GPIOx,GPIO_Pin);
		return 0;
	}
	else
	{
		GPIO_SetBits(GPIOx,GPIO_Pin);
		return 1;
	}
}
