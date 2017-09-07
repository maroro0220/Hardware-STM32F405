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
GPIO_InitTypeDef SONIC_T, SONIC_E, GPIO_Init_Struct;
;
static void ms_delay_int_count(volatile unsigned int nTime) // ms 지연
{
nTime = (nTime * 14000);
for(; nTime > 0; nTime--);
}
static void us_delay_int_count(volatile unsigned int nTime) // us 지연
{
nTime = (nTime * 12);
for(; nTime > 0; nTime--);
}
void CLCD_Config() {

	// CLCD용 GPIO (GPIOC)의 초기설정을 함
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	// CLCD_RS(PC8), CLCD_E(PC9, DATA 4~5(PC12~15)
	GPIO_Init_Struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13
			| GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_Init_Struct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Struct.Pull = GPIO_NOPULL;
	GPIO_Init_Struct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOC, &GPIO_Init_Struct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // CLCD_E = 0
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // CLCD_RW = 0
}
void CLCD_write(unsigned char rs, char data) {

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, rs); // CLCD_RS
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // CLCD_E = 0
	us_delay_int_count(2);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, (data >> 4) & 0x1); // CLCD_DATA = LOW_bit
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, (data >> 5) & 0x1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, (data >> 6) & 0x1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, (data >> 7) & 0x1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // CLCD_E = 1
	us_delay_int_count(2);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // CLCD_E = 0
	us_delay_int_count(2);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, (data >> 0) & 0x1); // CLCD_DATA = HIGH_bit
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, (data >> 1) & 0x1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, (data >> 2) & 0x1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, (data >> 3) & 0x1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // CLCD_E = 1
	us_delay_int_count(2);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // CLCD_E = 0
	ms_delay_int_count(2);
	//시간 엄수. 처리하는데 필요한 시간
}
void CLCD_init() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
	CLCD_write(0, 0x33); // 4비트 설정 특수 명령
	CLCD_write(0, 0x32); // 4비트 설정 특수 명령
	CLCD_write(0, 0x28); // _set_function
	CLCD_write(0, 0x0F); // _set_display
	CLCD_write(0, 0x01); // clcd_clear
	CLCD_write(0, 0x06); // _set_entry_mode
	CLCD_write(0, 0x02); // Return home
}
void clcd_put_string(char *buf) {
	CLCD_write(0, 0x01);
	while (*buf) {
		CLCD_write(1, *buf++);
	}

}
char output_buffer[256] = { 0, };
int flag = 1, cnt=0;
void SONIC_Config() {
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	SONIC_T.Pin = GPIO_PIN_0;
	SONIC_T.Mode = GPIO_MODE_OUTPUT_PP;
	SONIC_T.Pull = GPIO_NOPULL;
	SONIC_T.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOC, &SONIC_T);
}

void EXTILine_Config() {
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	SONIC_E.Pin = GPIO_PIN_1;
	SONIC_E.Mode = GPIO_MODE_IT_RISING;
	SONIC_E.Pull = GPIO_NOPULL;
	SONIC_E.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &SONIC_E);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}
void TIMER_Config() {
	__HAL_RCC_TIM2_CLK_ENABLE()
	;
	TimHandle.Instance = TIM2;
	TimHandle.Init.Period = 58 - 1;
	TimHandle.Init.Prescaler = 84 - 1;
	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle);
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

}
int main(int argc, char* argv[]) {
	CLCD_Config();
	CLCD_init();
	EXTILine_Config();
	TIMER_Config();
	SONIC_Config();

	while (1) {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 1);
		us_delay_int_count(10);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 0);
		ms_delay_int_count(1000);
		// Add your code here.
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
cnt++;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (flag==1) {
		cnt=0;
		HAL_TIM_Base_Start_IT(&TimHandle);
		flag = 0;
		SONIC_E.Pin = GPIO_PIN_1;
		SONIC_E.Mode = GPIO_MODE_IT_FALLING;
		HAL_GPIO_Init(GPIOC, &SONIC_E);
	} else if(flag==0) {
		HAL_TIM_Base_Stop_IT(&TimHandle);
		flag = 1;
		SONIC_E.Pin = GPIO_PIN_1;
		SONIC_E.Mode = GPIO_MODE_IT_RISING;
		HAL_GPIO_Init(GPIOC, &SONIC_E);
		sprintf(output_buffer, "Distance=%d cm", cnt);
		clcd_put_string(output_buffer);
	}
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
