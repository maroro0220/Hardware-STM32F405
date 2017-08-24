/*
 * This file is part of the 쨉OS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx.h"
#include "Timer.h"
#include "BlinkLed.h"

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"
void ms_delay_int_count(volatile unsigned int nTime) {
	nTime = (nTime * 14000);
	for (; nTime > 0; nTime--)
		;
}

void EXTILine_Config(void) {
	RCC->AHB1ENR |= 0x00000001;  //Enable GPIOA clock
	/*configure PB2 pin as input floating*/
	GPIOA->MODER = 0xA8000000;
	GPIOA->PUPDR = 0x0000000;
	GPIOA->OSPEEDR = 0x0C000000; //0으로 해줘도 됨
	/*EXTI Mode Configuration*/
	RCC->APB2ENR |= 0x00004000; //Enable SYSCFG Clock
	SYSCFG->EXTICR[2] |= 0x00000000; //PA8,11 setting
	SYSCFG->EXTICR[3] |= 0x00000000; //PA12 setting
	EXTI->RTSR |= 0x00001900; // Clear Rising Falling Edge
	EXTI->IMR |= 0x00001900; //Clear EXTI line Configure
	/*Enable and set EXTI LIne2 INterrupt tot he lowest priority*/
	NVIC_SetPriority(EXTI9_5_IRQn, 2);  //PA8 interrupt
	//	NVIC->IP[2]|=0x00000002;
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	// NVIC->ISER[0]|=0x00000100;
	NVIC_SetPriority(EXTI15_10_IRQn, 3);//PA11,12 interrupt
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

unsigned int f0=0,f1=0;

int main(int argc, char* argv[]) {
	/*##-1- Enable GPIOA Clock (to be able to program the configuration registers) */
	RCC->AHB1ENR |= 0x00000004; // PC2, PC3   LD1, LD2
	/*##-2- Configure PA2, PA3 IO in output push-pull mode to drive external LED ###*/
	GPIOC->MODER = 0x000000050;
	GPIOC->PUPDR = 0x00000000;
	GPIOC->OSPEEDR = 0x00000000;
	/* Configure EXTI Line2 (connected to PB2 pin) in interrupt mode */
	EXTILine_Config();
	//GPIOC->ODR=0xF;
	while (1)
	{
		if(f1){
			GPIOC->ODR^=0x04;
		}
		else{
			GPIOC->ODR|=0x04;
		}
		if(f0){
			GPIOC->ODR^=0x08;
		}
		else{
			GPIOC->ODR|=0x08;
		}
		ms_delay_int_count(100);
	}
}
void EXTI9_5_IRQHandler(void) {
	if (EXTI->PR == 0x00000001<<8) //IRQ<8>에서 읶터럽트 요청이 들어온 건지 확읶   1<<8
			{
		EXTI->PR |= (1 << 8); //EXTI pending bits(flag) 클리어
		f1=!f1;
	}
}
void EXTI15_10_IRQHandler(void) {
	if (EXTI->PR == 0x00000001<<11) //IRQ<11>에서 읶터럽트 요청이 들어온 건지 확읶  1<<11
			{
		EXTI->PR |= (1 << 11); //EXTI pending bits(flag) 클리어
		f0=!f0;
	}
	else if (EXTI->PR == 0x00000001<<12) //IRQ<12>에서 읶터럽트 요청이 들어온 건지 확읶  1<<12
				{
			EXTI->PR |= (1 << 12); //EXTI pending bits(flag) 클리어
			GPIOC->ODR ^= 0x000C;
			f0=0;
			f1=0;
		}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
