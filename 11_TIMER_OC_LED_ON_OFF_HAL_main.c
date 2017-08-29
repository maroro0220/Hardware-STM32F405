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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
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
TIM_HandleTypeDef TimHandle;
TIM_OC_InitTypeDef TIM_OCInit;
GPIO_InitTypeDef GPIO_Init_Struct;
void LED_Config(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_Init_Struct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_Init_Struct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Struct.Pull=GPIO_PULLUP;
	GPIO_Init_Struct.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_Init_Struct);
}

void TIMER_Config(){
	//TIMx Peripheral clock enable
	__HAL_RCC_TIM2_CLK_ENABLE();

	//Set TIMx instance
	TimHandle.Instance=TIM2;// TIM2 사용
	TimHandle.Init.Period=10000-1;// 업데이트 이X드 발생시 ARR=9999로 설정
	TimHandle.Init.Prescaler=8400-1;// Prescaler = 8399로 설정
	TimHandle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;// division을 사용하지 않음
	TimHandle.Init.CounterMode=TIM_COUNTERMODE_UP;// Up Counter 모드 설정
	HAL_TIM_Base_Init(&TimHandle);
	HAL_TIM_Base_Start_IT(&TimHandle);

	//Set TIMx OC instance
	TIM_OCInit.OCMode=TIM_OCMODE_TIMING;// OC 동작 모드 설정
	TIM_OCInit.Pulse=5000-1;// CCR의 설정값
	// TIM OC의 Channel을 TIM_OCInit에 설정된 값으로 초기화함
	HAL_TIM_OC_Init(&TimHandle);
	HAL_TIM_OC_ConfigChannel(&TimHandle,&TIM_OCInit,TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&TimHandle,TIM_CHANNEL_1);

	HAL_NVIC_SetPriority(TIM2_IRQn,0,0);/* Set Interrupt Group Priority */
	HAL_NVIC_EnableIRQ(TIM2_IRQn);/* Enable the TIMx global Interrupt */
}
int
main(int argc, char* argv[])
{
LED_Config();
TIMER_Config();
  while (1);
}
//void HAL_TIM_PeriodElapseCallback(TIM_HandleTypeDef *htim){
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,1);
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,1);
//}
//void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,0);
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,0);
//}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
}
