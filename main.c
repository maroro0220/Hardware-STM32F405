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

GPIO_InitTypeDef GPIO_Init_Struct; // GPIO의 초기화를 위한 구조체형의 변수를 선언
ADC_HandleTypeDef AdcHandler; // ADC의 초기화를 위한 구조체형의 변수를 선언
ADC_ChannelConfTypeDef sConfig;
int adc_value;// ADC값 저장 변수

/*지연루틴*/
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
void LED_Config() {
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	GPIO_Init_Struct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_Init_Struct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Struct.Pull = GPIO_NOPULL;
	GPIO_Init_Struct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);
}
void ADC_Config(){
//	__HAL_RCC_ADC1_CLK_ENABLE();
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	GPIO_Init_Struct.Pin=GPIO_PIN_4;
//	GPIO_Init_Struct.Mode=GPIO_MODE_ANALOG;
//	HAL_GPIO_Init(GPIOA,&GPIO_Init_Struct);
//	AdcHandler.Instance=ADC1;
//	AdcHandler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV2;
//	AdcHandler.Init.Resolution=ADC_RESOLUTION_12B;
//	AdcHandler.Init.DataAlign=ADC_DATAALIGN_RIGHT;
//	AdcHandler.Init.ScanConvMode=DISABLE;
//	AdcHandler.Init.ContinuousConvMode=ENABLE;
//	AdcHandler.Init.NbrOfConversion=1;
//	AdcHandler.Init.ExternalTrigConv=ADC_INJECTED_SOFTWARE_START;
//	HAL_ADC_Init(&AdcHandler);
//
//	sConfig.Channel=ADC_CHANNEL_4;
//	sConfig.Rank=1;
//	sConfig.SamplingTime=ADC_SAMPLETIME_3CYCLES;
//	HAL_ADC_ConfigChannel(&AdcHandler,&sConfig);
//
//	HAL_NVIC_SetPriority(ADC_IRQn,0,0);
//	HAL_NVIC_EnableIRQ(ADC_IRQn);

	__HAL_RCC_ADC1_CLK_ENABLE(); // ADC 클럭 홗성화
	__HAL_RCC_GPIOA_CLK_ENABLE(); // ADC 핀으로 사용할 GPIOx 홗성화(VR:PA4)
	GPIO_Init_Struct.Pin = GPIO_PIN_4; // GPIO에서 사용할 PIN 설정
	GPIO_Init_Struct.Mode = GPIO_MODE_ANALOG; // Input Analog Mode 모드
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);
	AdcHandler.Instance = ADC1; // ADC1 설정
	AdcHandler.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2; // ADC clock prescaler
	AdcHandler.Init.Resolution = ADC_RESOLUTION_12B; // ADC resolution
	AdcHandler.Init.DataAlign = ADC_DATAALIGN_RIGHT; // ADC data alignment
	AdcHandler.Init.ScanConvMode = DISABLE; // ADC scan 모드 비홗성화
	AdcHandler.Init.ContinuousConvMode = ENABLE; // ADC 연속 모드 홗성화
	AdcHandler.Init.NbrOfConversion = 1; // ADC 변홖 개수 설정
	AdcHandler.Init.ExternalTrigConv = ADC_SOFTWARE_START; // ADC 외부 트리거 OFF
	HAL_ADC_Init(&AdcHandler); // ADC를 설정된 값으로 초기화
	sConfig.Channel = ADC_CHANNEL_4; // ADC 채널 설정(PA는 채널 4번)
	sConfig.Rank = 1; // ADC 채널 순위 설정
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES; // ADC 샘플링 타임 설정(3클럭)
	HAL_ADC_ConfigChannel(&AdcHandler, &sConfig); // 채널 설정
	/* NVIC configuration */
	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
}
int
main(int argc, char* argv[])
{
//	LED_Config();
//	ADC_Config();
//	HAL_ADC_Start_IT(&AdcHandler);
	/* Configure LED_1(PA2), LED_2(PA3) */
	LED_Config();
	/* Configure ADC */
	ADC_Config();
	// Interrupt 방식으로 ADC 변홖 수행
	HAL_ADC_Start_IT(&AdcHandler); // ADC를 동작시킴, 읶터럽트 홗성화
  // At this stage the system clock should have already been configured
  // at high speed.

  // Infinite loop
  while (1)
    {
       // Add your code here.


    }
}
/*읶터럽트 루틴*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandler)
{
	  adc_value = HAL_ADC_GetValue(AdcHandler); // ADC 변홖 결과 값을 저장
	  // 변홖 결과값을 이용하여 LED의 주기를 변경
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,1);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,1);
		  us_delay_int_count(adc_value);
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,0);
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,0);
		  us_delay_int_count(3000);
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
