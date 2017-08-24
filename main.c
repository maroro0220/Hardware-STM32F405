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
	RCC->AHB1ENR |= 0x00000002;  //Enable GPIOB clock
	/*configure PB2 pin as input floating*/
	GPIOB->MODER = 0x00000280;
	GPIOB->PUPDR = 0x0000000;
	GPIOB->OSPEEDR = 0x00000030; //0으로 해줘도 됨
	/*EXTI Mode Configuration*/
	RCC->APB2ENR |= 0x00004000; //Enable SYSCFG Clock
	SYSCFG->EXTICR[0] |= 0x00000111; //PB0,1,2 setting
	EXTI->RTSR |= 0x00000007; // Clear Rising Falling Edge
	EXTI->IMR |= 0x00000007; //Clear EXTI line Configure
	/*Enable and set EXTI LIne2 INterrupt tot he lowest priority*/
	NVIC_SetPriority(EXTI2_IRQn, 2);  //0부터 쓸 수 있음 -3~-1 은 정해진거라
	//	NVIC->IP[2]|=0x00000002;
	NVIC_EnableIRQ(EXTI2_IRQn);
	// NVIC->ISER[0]|=0x00000100;
	NVIC_SetPriority(EXTI0_IRQn, 3);
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_SetPriority(EXTI1_IRQn, 4);
	NVIC_EnableIRQ(EXTI1_IRQn);
}

unsigned int f0 = 0, f1 = 0,f2=0;
int main(int argc, char* argv[]) { /*##-1- Enable GPIOA Clock (tobe able to program the configuration registers)*/
	RCC->AHB1ENR |= 0x00000001;	//LED_1(PA2),LED_2(PA3)
	/*## -2- COnfigure PA2, PA3 IO in output push-pull mode to drive external LED##*/
	GPIOA->MODER = 0xA8000050;
	GPIOA->PUPDR = 0x00000000;
	GPIOA->OSPEEDR = 0x000000A0;
	/*Configure EXTI Line2 (connected to PB2 pin)in interrupt mode*/
	EXTILine_Config();
	while (1) {

		if (f0) {
			GPIOA->ODR ^= 0x0004;
		}
		if (f1) {
			GPIOA->ODR ^= 0x0008;
		}
		ms_delay_int_count(100);
	}
}
/*인터럽트 루틴*/
void EXTI0_IRQHandler(void) {
	if (EXTI->PR == 0x0000001) {	//IRQ<2>에서 인터럽트 요청이 들어온건지확인
		EXTI->PR |= (1);	//EXTI pending bits(flag)클리어
		f0 = !f0;
	}
}

void EXTI1_IRQHandler(void) {
	if (EXTI->PR == 0x0000002) {	//IRQ<2>에서 인터럽트 요청이 들어온건지확인
		EXTI->PR |= (1 << 1);	//EXTI pending bits(flag)클리어
		f1 = !f1;
	}
}

void EXTI2_IRQHandler(void) {
	if (EXTI->PR == 0x0000004) {	//IRQ<2>에서 인터럽트 요청이 들어온건지확인
		EXTI->PR |= (1 << 2);	//EXTI pending bits(flag)클리어

		GPIOA->ODR ^= 0x000C;
		f1 = 0;
		f0 = 0;
	}
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
