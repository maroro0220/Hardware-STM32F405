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
GPIO_InitTypeDef LED;
void TIMER_Config(){
	__HAL_RCC_TIM3_CLK_ENABLE();
	TimHandle.Instance=TIM3;
	TimHandle.Init.Prescaler=8400-1;
	TimHandle.Init.Period=1000-1;
	TimHandle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	TimHandle.Init.CounterMode=TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle);
	HAL_TIM_Base_Start_IT(&TimHandle);
	HAL_NVIC_SetPriority(TIM3_IRQn,0,0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

}
unsigned int i=0,flag=1;
int main(int argc, char* argv[])
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	LED.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
	LED.Mode=GPIO_MODE_OUTPUT_PP;
	LED.Pull=GPIO_NOPULL;
	LED.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC,&LED);
	TIMER_Config();

  while (1)
  {
   }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	GPIOC->ODR=~(0x004<<i);
	if((flag==1)){
		i++;
		if(i==3)
		flag=0;
	}
	else if(flag==0){
		i--;
		if(i==0)
		flag=1;
	}
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
