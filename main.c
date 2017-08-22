/*
 * This file is part of the ÂµOS++ distribution.
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
void main(int argc, char* argv[]) {
	char fl = 'a';
	char fr = 'a';
	char fu = 'a';
	char fd = 'a';
	RCC->AHB1ENR = 0x00000003;
	GPIOA->MODER = 0xA8001050; //LED, MOTOR »ç¿ë
	GPIOA->OTYPER = 0;
	GPIOA->OSPEEDR = 0;
	GPIOA->PUPDR = 0;

	GPIOB->MODER = 0x00005280;
	GPIOB->OTYPER = 0;
	GPIOB->OSPEEDR = 0;
	GPIOB->PUPDR = 0;
	unsigned int data, cu = 0x0000;
	int flag = 0;

	while (1) {
		data = GPIOB->IDR;
		ms_delay_int_count(200);
		if (data & (0x0001 << 12)) {
			cu = 0x0004;
			GPIOA->ODR = cu;
			fr = 'r';
			fl = 'a';
			fu = 'a';
			fd = 'a';
			flag = 0;
		} else if (data & (0x0001 << 13)) {
			cu = 0x0008;
			GPIOA->ODR = cu;
			fl = 'l';
		} else if (data & (0x0001 << 2)) {
			fr = 'a';
			fl = 'a';
			fu = 'a';
			fd = 'a';
			if (flag == 0) {
				GPIOA->ODR = 0x000c;
				flag = 1;
			} else {
				GPIOA->ODR = 0x0000;
				flag = 0;
			}
		} else if (data & 0x01) {
			fu = 'u';
			fr = 'a';
			fl = 'a';
//			fd = 'a';
			flag = 0;
//			GPIOA->ODR=0x0040;

		} else if (data & (0x01 << 1)) {
			fd = 'd';
			fr = 'a';
			fl = 'a';
//			fu = 'a';
			flag = 0;
//			GPIOA->ODR=0x0040;
		}
		else{
			if(fl=='l'&&fr=='r'){
			cu=0x0000;
			GPIOA->ODR=cu;
			fl='a';
			fr='a';
			}
		}
		if (fu == 'u') {
			GPIOA->ODR^=0x0004;
			ms_delay_int_count(100);
//			GPIOA->ODR=0x0000;
		}
		if(fd=='d'){
			GPIOA->ODR^=0x0008;
			ms_delay_int_count(100);
//			GPIOA->ODR=0x0000;
		}
}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
