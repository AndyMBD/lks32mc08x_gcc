  .syntax unified
  .cpu cortex-m0
  .fpu softvfp
  .thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function


Reset_Handler:

/* Copy the data segment initializers from flash to SRAM */
  movs r1, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r3, =_sidata
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4

LoopCopyDataInit:
  ldr r0, =_sdata
  ldr r3, =_edata
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyDataInit
  ldr r2, =_sbss
  b LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs r3, #0 /* 将寄存器r3写0 */
  str r3, [r2]	/* 采用后变址，先将r3中的值写入r2，也就是bss数据区首地址中 */
  /* M0内核不能直接自加这种汇编写法，需要拆分 */
  adds r2, r2, #4 /* 然后r2自加4 */

LoopFillZerobss:
  ldr r3, = _ebss
  cmp r2, r3
  bcc FillZerobss

/* Call the clock system intitialization function.*/
/*    bl  SystemInit */
/* Call static constructors */
	bl __libc_init_array
/* Call the application's entry point.*/
	bl main
    bx lr
.size Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
    .section .text.Default_Handler,"ax",%progbits

Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M0.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word 0
  .word 0  
  .word PendSV_Handler
  .word SysTick_Handler
  
  /* External Interrupts 0-3 */
    .word UTIMER0_IRQHandler
    .word UTIMER1_IRQHandler
	.word UTIMER2_IRQHandler
	.word UTIMER3_IRQHandler
	
	/* External Interrupts 4-7 */
	.word ENCODER0_IRQHandler
	.word ENCODER1_IRQHandler
	.word I2C0_IRQHandler
	.word GPIO_IRQHandler
	
	/* External Interrupts 8-11 */
	.word UART0_IRQHandler
	.word HALL_IRQHandler
	.word SPI0_IRQHandler
	.word ADC0_IRQHandler
	
	/* External Interrupts 12-15 */
	.word DSP_IRQHandler
	.word MCPWM_IRQHandler
	.word UART1_IRQHandler
	.word CMP_IRQHandler
	
	/* External Interrupts 16-19 */
	.word WAKE_IRQHandler
	.word PWRDN_IRQHandler
	.word DMA_IRQHandler
	.word CAN_IRQHandler
	
	/* External Interrupts 20-23 */
	.word SIF_IRQHandler
	.word 0
	.word 0
	.word 0
	
	/* External Interrupts 24-27 */
	.word 0
	.word 0
	.word 0
	.word 0
	
	/* External Interrupts 28-31 */
	.word 0
	.word 0
	.word 0
	.word 0

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler
  
  .weak SVC_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak PendSV_Handler
  .thumb_set HardFault_Handler,Default_Handler
  
  .weak SysTick_Handler
  .thumb_set HardFault_Handler,Default_Handler
  
	.weak UTIMER0_IRQHandler
	.weak UTIMER1_IRQHandler
	.weak UTIMER2_IRQHandler
	.weak UTIMER3_IRQHandler
	
	.weak ENCODER0_IRQHandler
	.weak ENCODER1_IRQHandler
	.weak I2C0_IRQHandler
	.weak GPIO_IRQHandler
	
	.weak UART0_IRQHandler
	.weak HALL_IRQHandler
	.weak SPI0_IRQHandler
	.weak ADC0_IRQHandler
	
	.weak DSP_IRQHandler
	.weak MCPWM_IRQHandler
	.weak UART1_IRQHandler
	.weak CMP_IRQHandler
	
	.weak WAKE_IRQHandler
	.weak PWRDN_IRQHandler
	.weak DMA_IRQHandler
	.weak CAN_IRQHandler
	
	.weak SIF_IRQHandler

/************************ (C) COPYRIGHT LINKO SEMICONDUCTOR CO.,LTD *****END OF FILE****/

