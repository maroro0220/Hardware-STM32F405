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

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via ITM).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

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
int main(int argc, char* argv[]) {
	RCC->AHB1ENR = 0x00000005; //GPIOC에 RCC clock 공급
	GPIOC->MODER = 0x00000550; //GPIOC(2)를 general purpose output mode로
//	GPIOC->MODER = 0x00000010;
	GPIOC->OTYPER = 0; //GPIOC(2)는 모두 output push-pull
	GPIOC->PUPDR = 0;
	GPIOC->OSPEEDR = 0;

	GPIOA->MODER = 0xA8000050; //GPIOC(2)를 general purpose output mode로
	GPIOA->OTYPER = 0; //GPIOC(2)는 모두 output push-pull
	GPIOA->PUPDR = 0;
	GPIOA->OSPEEDR = 0;
	// Send a greeting to the trace device (skipped on Release).

//	GPIOC->BSRR=0x00380004;
	// Infinite loop
	int a = 0x0004000A;
//	int b=0x0004;
	int c = 0x0004;
//	GPIOC->ODR=a;
	while (1) {
//		GPIOC->ODR=a<<1;
		for (int i = 0; i < 4; i++) {
			a = a << 1;
			GPIOC->BSRR = a;
//			GPIOC->ODR=~(b<<i);
			//	// Infinite loop, never return.
			//		GPIOC->ODR = 0x0004;		//LD1
			//		ms_delay_int_count(100);
			//		GPIOC->ODR = 0x0000; //LD1 on
			if (i % 2 == 0)
				GPIOA->ODR = c << 1;
			else
				GPIOA->ODR = 0x0004;
			ms_delay_int_count(100);
		}
		for (int i = 3; i >= 0; i--) {
			a = a >> 1;
			GPIOC->BSRR = a;
			if (i % 2 == 0)
				GPIOA->ODR = c << 1;
			else
				GPIOA->ODR = c;
//				GPIOC->ODR=~(b<<i);
			ms_delay_int_count(100);
		}

	}
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
