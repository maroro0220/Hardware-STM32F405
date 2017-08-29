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
GPIO_InitTypeDef LED, MOTOR, JOG;
void EXTILine_Config() {
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	JOG.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
	JOG.Mode = GPIO_MODE_IT_RISING;
	JOG.Pull = GPIO_NOPULL;
	JOG.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &JOG);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_SetPriority(EXTI2_IRQn, 4, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}
void LED_Config() {
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	LED.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6; //6Àº MOTOR PWM
	LED.Mode = GPIO_MODE_OUTPUT_PP;
	LED.Pull = GPIO_NOPULL;
	LED.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOA, &LED);
}
void MOTOR_Config() {
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	MOTOR.Pin = GPIO_PIN_6 | GPIO_PIN_7; //6:Pos 7:Neg
	MOTOR.Mode = GPIO_MODE_OUTPUT_PP;
	MOTOR.Pull = GPIO_NOPULL;
	MOTOR.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOB, &MOTOR);

}
//unsigned int Pulse = 1000;
unsigned int LED_Pulse=500,MOTOR_Pulse=500;
void TIMER_Config() {
	__HAL_RCC_TIM2_CLK_ENABLE()
	;
	TimHandle.Instance = TIM2;
	TimHandle.Init.Period = 1000 - 1;
	TimHandle.Init.Prescaler = 84;
	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle);
	HAL_TIM_Base_Start_IT(&TimHandle);

	TIM_OCInit.OCMode = TIM_OCMODE_TIMING;
	TIM_OCInit.Pulse = 500 - 1;

	HAL_TIM_OC_Init(&TimHandle);

	HAL_TIM_OC_ConfigChannel(&TimHandle,&TIM_OCInit, TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&TimHandle, TIM_CHANNEL_1);

	TIM_OCInit.Pulse=500-1;
	HAL_TIM_OC_ConfigChannel(&TimHandle,&TIM_OCInit, TIM_CHANNEL_2);
	HAL_TIM_OC_Start_IT(&TimHandle, TIM_CHANNEL_2);

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

}
int main(int argc, char* argv[]) {
	// At this stage the system clock should have already been configured
	// at high speed.

	// Infinite loop
	EXTILine_Config();
	LED_Config();
	MOTOR_Config();
	TIMER_Config();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);	//motor positive on
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);	//motor negative off

	while (1) {
		// Add your code here.
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
	}
	if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_1) {
//		if (Pulse <= 100)
//			Pulse = 100;
//		else
//			Pulse -= 100;
		if (LED_Pulse <= 100)
			LED_Pulse = 100;
		else
			LED_Pulse -= 100;

		if (MOTOR_Pulse >= 1000)
			MOTOR_Pulse = 1000;
		else
			MOTOR_Pulse += 100;

		TIM_OCInit.Pulse = LED_Pulse - 1;
		HAL_TIM_OC_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_1);
		TIM_OCInit.Pulse=MOTOR_Pulse-1;
		HAL_TIM_OC_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_2);
	} else if (GPIO_Pin == GPIO_PIN_0) {
		if (LED_Pulse >= 1000)
			LED_Pulse = 1000;
		else
			LED_Pulse += 100;

		if (MOTOR_Pulse <= 100)
			MOTOR_Pulse = 100;
		else
			MOTOR_Pulse -= 100;

		TIM_OCInit.Pulse = LED_Pulse - 1;
		HAL_TIM_OC_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_1);
		TIM_OCInit.Pulse=MOTOR_Pulse-1;
		HAL_TIM_OC_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_2);
	}
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
