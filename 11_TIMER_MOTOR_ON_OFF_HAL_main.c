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
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
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
TIM_HandleTypeDef TimHandle; // 타이머의 초기화용 구조체 변수를 선언
GPIO_InitTypeDef MOT, PWM; // GPIO의 초기화를 위한 구조체 변수를 선언

void TIMER_Config(void) {
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* TIMx Peripheral clock enable */
	__HAL_RCC_TIM3_CLK_ENABLE()
	;
	/* Set TIMx instance */
	TimHandle.Instance = TIM3; // TIM3사용
	TimHandle.Init.Period = 10000 - 1; // 업데이트 이X드 발생시 ARR=9999로 설정
	TimHandle.Init.Prescaler = 8400 - 1; // Prescaler = 8399로 설정
	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // division을 사용하지 않음
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP; //up Counter 모드 설정
	HAL_TIM_Base_Init(&TimHandle);
	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	HAL_TIM_Base_Start_IT(&TimHandle); /* Start Channel1 */
	/*##-3- Configure the NVIC for TIMx ########################################*/
	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0); /* Set Interrupt Group Priority */
	HAL_NVIC_EnableIRQ(TIM3_IRQn); /* Enable the TIMx global Interrupt */
}

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[]) {
	/*##-1- Enable GPIOC Clock (to be able to program the configuration registers) */
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	PWM.Pin = GPIO_PIN_6; //PWM
	PWM.Mode = GPIO_MODE_OUTPUT_PP;
	PWM.Pull = GPIO_NOPULL;
	PWM.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &PWM);
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	/*##-2- Configure PC2 IO in output push-pull mode to drive external LED ###*/
	MOT.Pin = GPIO_PIN_6 | GPIO_PIN_7; // pb6p pb7n
	MOT.Mode = GPIO_MODE_OUTPUT_PP;
	MOT.Pull = GPIO_NOPULL;
	MOT.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &MOT);
	/* Configure TIMER */
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,1);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,1);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,0);
	TIMER_Config();
	/*메米羚씩【 하는 */
	while (1)
		;
}\
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//	if(htim->Instance == TIM2)

//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
}
// ----------------------------------------------------------------------------
