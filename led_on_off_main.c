/*
 * This file is part of the µOS++ distribution.
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
#define RCC_AHB1ENR (*(volatile unsigned long*)0x40023830)
#define GPIOA_MODER (*(volatile unsigned long*)0x40020000)
#define GPIOA_OTYPER (*(volatile unsigned long*)0x40020004)
#define GPIOA_OSPEEDR (*(volatile unsigned long*)0x40020008)
#define GPIOA_PUPDR (*(volatile unsigned long*)0x4002000C)
#define GPIOA_ODR (*(volatile unsigned long*)0x40020014)

#define GPIOC_MODER (*(volatile unsigned long*)0x40020800)
#define GPIOC_OTYPER (*(volatile unsigned long*)0x40020804)
#define GPIOC_OSPEEDR (*(volatile unsigned long*)0x40020808)
#define GPIOC_PUPDR (*(volatile unsigned long*)0x4002080C)
#define GPIOC_ODR (*(volatile unsigned long*)0x40020814)
// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void ms_delay_int_count(volatile unsigned int nTime) {
	nTime = (nTime * 14000);
	for (; nTime > 0; nTime--);
}
int main(int argc, char* argv[]) {
	int cnt=0;
	RCC_AHB1ENR = 0x00000005;
	GPIOA_MODER = 0xA8000050;
	GPIOA_OTYPER = 0;
	GPIOA_PUPDR = 0;
	GPIOA_OSPEEDR = 0;

	GPIOC_MODER = 0x00000550;
	GPIOC_OTYPER = 0;
	GPIOC_PUPDR = 0;
	GPIOC_OSPEEDR = 0;
	while (1) {
		if(cnt%2==0)
		{
		GPIOA_ODR=0x0004;
		GPIOC_ODR=0x0028;
		}

		else{
			GPIOA_ODR=0x0008;
			GPIOC_ODR=0x0014;}
		ms_delay_int_count(500);
		cnt++;
	}

	// Send a greeting to the trace device (skipped on Release).
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
