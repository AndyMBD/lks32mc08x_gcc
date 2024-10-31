#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "lks32mc08x.h"
#include "interrupt.h"

void SleepTimer_IRQHandler(void)
{
	while(1);
}

void GPIO_IRQHandler(void)
{
	while(1);
}

void VC3_IRQHandler(void)
{
	while(1);
}

void CSX_IRQHandler(void)
{
	while(1);
}

void NSX_IRQHandler(void)
{
	while(1);
}

void I2C0_IRQHandler(void)
{
	while(1);
}

void SPI0_IRQHandler(void)
{
	while(1);
}

void HALL_IRQHandler(void)
{
   while(1);
}

void UTIMER0_IRQHandler(void)
{
   while(1);
}

void UTIMER1_IRQHandler(void)
{
    while(1);
}

void UTIMER2_IRQHandler(void)
{
   while(1);
}

void UTIMER3_IRQHandler(void)
{
   while(1);
}

void ENCODER0_IRQHandler(void)
{
    while(1);
}

void ENCODER1_IRQHandler(void)
{
    while(1);
}

void ADC1_IRQHandler(void)
{
	while(1);
}

void ADC0_IRQHandler(void)
{
	while(1);
}

void MCPWM_IRQHandler(void)
{
	while(1);
}

void CMP_IRQHandler(void)
{
	while(1);
}

void UART1_IRQHandler(void)
{
	while(1); 
}

void UART0_IRQHandler(void)
{
	while(1);
}

void SysTick_Handler(void)
{
	while(1);
}

#endif
