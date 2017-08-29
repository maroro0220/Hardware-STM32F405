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

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 empty sample (trace via ITM).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

//;**************************************************************************************
//;* 09_EXTI_JOGSW_LED_ON_OFF_HAL
//;*
//;* 이 프로그램은 PB2에 연결된 JOGSW_CEN에서 상승에지가 발생할 때마다 LED1,LED2를 ON/OFF한다
//;* Target: stm32f4
//;**************************************************************************************

#include "stm32f4xx_hal.h"      //관련 레지스터의 주소 지정
#include "stm32f4xx_it.h"	//인터럽트 사용에 필요한 헤더파일

GPIO_InitTypeDef LED;
GPIO_InitTypeDef JOG_CEN;

void EXTILine2_Config(void)
{
	/* Enable GPIOB clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* Configure PB2 pin as input floating */
	JOG_CEN.Pin = GPIO_PIN_2;
	JOG_CEN.Mode = GPIO_MODE_IT_RISING;
	JOG_CEN.Pull = GPIO_NOPULL;
	JOG_CEN.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &JOG_CEN);

	/* Enable and set EXTI Line15 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

/*메인루틴*/
int main(int argc, char* argv[])
{
	/*##-1- Enable GPIOA Clock (to be able to program the configuration registers) */
	__HAL_RCC_GPIOA_CLK_ENABLE();	// LED1(PA2), LED2(PA3)

	/*##-2- Configure PA2, PA3 IO in output push-pull mode to drive external LED ###*/
	LED.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	LED.Mode = GPIO_MODE_OUTPUT_PP;
	LED.Pull = GPIO_NOPULL;
	LED.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &LED);

	/* Configure EXTI Line2 (connected to PB2 pin) in interrupt mode */
	EXTILine2_Config();

	while (1);
}

/*인터럽트 루틴*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
}

// ----------------------------------------------------------------------------
