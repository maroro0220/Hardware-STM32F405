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

TIM_HandleTypeDef TimHandle2, TimHandle3;
GPIO_InitTypeDef MOT, PWM, JOG;
GPIO_InitTypeDef LED;
void TIMER_Config() {
	__HAL_RCC_TIM2_CLK_ENABLE()
	;
	TimHandle2.Instance = TIM2;
	TimHandle2.Init.Prescaler = 8400 - 1;
	TimHandle2.Init.Period = 1000 - 1;
	TimHandle2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle2.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle2);
	HAL_TIM_Base_Start_IT(&TimHandle2);
	HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	__HAL_RCC_TIM3_CLK_ENABLE()
	;
	TimHandle3.Instance = TIM3;
	TimHandle3.Init.Prescaler = 8400 - 1;
	TimHandle3.Init.Period = 1500 - 1;
	TimHandle3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle3.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle3);
	HAL_TIM_Base_Start_IT(&TimHandle3);
	HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

}
void EXTILine_Config(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	PWM.Pin = GPIO_PIN_6;
	PWM.Mode = GPIO_MODE_OUTPUT_PP;
	PWM.Pull = GPIO_NOPULL;
	PWM.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &PWM);

	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	MOT.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	MOT.Mode = GPIO_MODE_OUTPUT_PP;
	MOT.Pull = GPIO_NOPULL;
	MOT.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &MOT);

	/* Configure PB2 pin as input floating */
	JOG.Pin = GPIO_PIN_2 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_0 | GPIO_PIN_1; //2:Center 12:L 13:R
	JOG.Mode = GPIO_MODE_IT_RISING;
	JOG.Pull = GPIO_NOPULL;
	JOG.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &JOG);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 4, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}
unsigned int f0 = 0, f1 = 0;
int main(int argc, char* argv[]) {
	// At this stage the system clock should have already been configured
	// at high speed.

	// Infinite loop
	//JOG_UP를 누르면 LED1 100ms점멸(TIM2)
	//dnd은 150ms (3ms)
	//cen은 tomotor onoff
	//lt는 CW
	//RT는 ccw
//	__HAL_RCC_GPIOA_CLK_ENABLE();
	TIMER_Config();
	EXTILine_Config();
	LED.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	LED.Mode = GPIO_MODE_OUTPUT_PP;
	LED.Pull = GPIO_NOPULL;
	LED.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &LED);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
	while (1) {

	}

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_12) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
	}
	else if (GPIO_Pin == GPIO_PIN_13) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);
	}
	else if (GPIO_Pin == GPIO_PIN_2) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
		f0 = 0;
		f1 = 0;
	}
	else if (GPIO_Pin == GPIO_PIN_0) {
		f0 = ~f0;

	}
	else if (GPIO_Pin == GPIO_PIN_1) {
		f1 = ~f1;
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if((htim->Instance==TIM2)&&f0){
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_2);

	}
	if((htim->Instance==TIM3)&&f1){
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_3);
	}
}
// ----------------------------------------------------------------------------
