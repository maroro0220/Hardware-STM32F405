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
#include "stm32f4xx_hal.h"      //관련 레지스터의 주소 지정
#include "stm32f4xx_it.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"
GPIO_InitTypeDef LED;
GPIO_InitTypeDef JOG;
void ms_delay_int_count(volatile unsigned int nTime) {
	nTime = (nTime * 14000);
	for (; nTime > 0; nTime--)
		;
}

void EXTILine_Config(void) {
	/* Enable GPIOB clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	JOG.Pin = GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0; //2:Center 1:Down 0:UP
	JOG.Mode = GPIO_MODE_IT_RISING;
	JOG.Pull = GPIO_NOPULL;
	JOG.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &JOG);

	HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
unsigned int f0 = 0, f1 = 0, f2 = 0;
int main(int argc, char* argv[]) {
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	LED.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	LED.Mode = GPIO_MODE_OUTPUT_PP;
	LED.Pull = GPIO_NOPULL;
	LED.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &LED);
	EXTILine_Config();
	while (1) {
		if (f0)
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
		else if (!f0 && !f2)
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
		if (f1)
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
		else if (!f1 && !f2)
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);

		ms_delay_int_count(100);
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_2) { //center
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,0);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,0);
		f2 = !f2;
		f0 = 0;
		f1 = 0;
	} else if (GPIO_Pin == GPIO_PIN_1) { //down
		f1 = !f1;
	} else if (GPIO_Pin == GPIO_PIN_0) { //up
		f0 = !f0;
	}
}
