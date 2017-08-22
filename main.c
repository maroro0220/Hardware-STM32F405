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
#include "stm32f4xx_hal.h"
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

GPIO_InitTypeDef LD1, LD2, LD3, LD4, SW1, SW2;

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

	// Send a greeting to the trace device (skipped on Release).

	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	LD1.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5; //PC led1~4
	SW1.Pin = GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12; //PA 8:s1 11:s2 12:s3


	LD1.Mode = GPIO_MODE_OUTPUT_PP;
	LD1.Pull = GPIO_NOPULL;
	LD1.Speed = GPIO_SPEED_LOW;
	SW1.Mode = GPIO_MODE_INPUT;
	SW1.Pull = GPIO_NOPULL;
	SW1.Speed = GPIO_SPEED_LOW;
	SW2.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_12 | GPIO_PIN_13; //PB 0:up 1:down 2:center 11:left 12:right
	SW2.Mode = GPIO_MODE_INPUT;
	SW2.Pull = GPIO_NOPULL;
	SW2.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &SW2);

	LD2.Pin = GPIO_PIN_2 | GPIO_PIN_3; //PA 2:led1 3:led2
	LD2.Mode = GPIO_MODE_OUTPUT_PP;
	LD2.Pull = GPIO_NOPULL;
	LD2.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &LD1);
	HAL_GPIO_Init(GPIOA, &SW1);
	HAL_GPIO_Init(GPIOA, &LD2);
	unsigned int up=0,down=0;
	while (1) {
		ms_delay_int_count(300);
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)) { //center
			HAL_GPIO_TogglePin(GPIOA,LD2.Pin);ms_delay_int_count(300);
			up=0;
			down=0;
			//			HAL_GPIO_TogglePin(GPIOA, LD2.Pin);
		} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)) { //up
			up=~up;
		} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)) { //down
			down=~down;
		} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)) { //left
			ms_delay_int_count(300);up=0;
			down=0;
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,0);
			HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_2);
		} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) { //right
			ms_delay_int_count(300);up=0;
			down=0;
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,0);
			HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_3);
		}
		if(up){
			HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_2);ms_delay_int_count(100);
		}		if(down){
			HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_3);ms_delay_int_count(100);
		}
//		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,0);
//		ms_delay_int_count(100);
//		HAL_GPIO_WritePin(GPIOC,LD1.Pin,1);
//		ms_delay_int_count(100);
//		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,0);
//		ms_delay_int_count(100);
//		HAL_GPIO_WritePin(GPIOC,LD1.Pin,1);
//		ms_delay_int_count(100);
//		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,0);
//		ms_delay_int_count(100);
//		HAL_GPIO_WritePin(GPIOC,LD1.Pin,1);
//		ms_delay_int_count(100);
//		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,0);
//		ms_delay_int_count(100);
//		HAL_GPIO_WritePin(GPIOC,LD1.Pin,1);
//		ms_delay_int_count(100);
	}
	// Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
