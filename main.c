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
#include "stm32f4xx_hal.h" //관련 레지스터의 주소 지정
#include "stm32f4xx_it.h" //읶터럽트 사용에 필요한 헤더파읷
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
TIM_OC_InitTypeDef TIM_OCInit; // 타이머 Channel(OC)의 초기화용 구조체 변수를 선언
GPIO_InitTypeDef GPIO_Init_Struct; // GPIO의 초기화를 위한 구조체형의 변수를 선언
unsigned int Pulse = 10000, Pulse_LED = 10000;

// -- LED의 초기설정용 함수의 선언
void LED_Config(void)
{
// LED용 GPIO (PA2, PA3)의 초기설정을 함
__HAL_RCC_GPIOA_CLK_ENABLE();
GPIO_Init_Struct.Pin = GPIO_PIN_2 | GPIO_PIN_3; // GPIO에서 사용할 PIN 설정
GPIO_Init_Struct.Mode = GPIO_MODE_AF_PP; // Alternate Function Push-Pull 모드
GPIO_Init_Struct.Alternate = GPIO_AF1_TIM2; // TIM2 Alternate Function mapping
GPIO_Init_Struct.Pull = GPIO_PULLUP; // Pull Up 모드
GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_HIGH; // 동작속도를 HIGH로
HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);
}

void SW_EXTILine_Config(void)
{
/* Enable GPIOB clock */
__HAL_RCC_GPIOB_CLK_ENABLE();
/* Configure JOG_DN(PB1),JOG_UP(PB0) pin as input floating */
GPIO_Init_Struct.Mode = GPIO_MODE_IT_RISING;
GPIO_Init_Struct.Pull = GPIO_NOPULL;
GPIO_Init_Struct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
GPIO_Init_Struct.Speed = GPIO_SPEED_HIGH;
HAL_GPIO_Init(GPIOB, &GPIO_Init_Struct);
/* Enable and set EXTI Line0,1 Interrupt to the lowest priority */
HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
HAL_NVIC_EnableIRQ(EXTI0_IRQn);
HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}
// -- TIMER의 초기설정용 함수의 선언
void TIMER_Config(void)
{
/* TIMx Peripheral clock enable */
__HAL_RCC_TIM2_CLK_ENABLE();
/* Set TIMx instance */
TimHandle.Instance = TIM2; // TIM2 사용
TimHandle.Init.Period = Pulse - 1; // 업데이트 이밲드 발생시 ARR=9999로 설정
TimHandle.Init.Prescaler = 0; // 최고치의 타이머 주파수를 얻기 위해 Prescaler = 0으로 설정
TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // division을 사용하지 않음
TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP; // Up Counter 모드 설정
/* Set TIMx PWM instance */
TIM_OCInit.OCMode = TIM_OCMODE_PWM1; // PWM mode 1 동작 모드 설정
/*PWM mode 1
up-counting읷 떄
•CNT < CCRx이면 active출력
•CNT ≥ CCRx이면 inactive출력
down-counting읷 때
•CNT ≤ CCRx이면 “active”출력
•CNT > CCRx이면 “inactive”출력*/
TIM_OCInit.Pulse = Pulse - 1; // CCR의 설정값
//TIM_OCInit.OCPolarity = TIM_OCPOLARITY_LOW; // OC 출력을 LOW로 설정
HAL_TIM_PWM_Init(&TimHandle); // TIM PWM을 TimHandle에 설정된 값으로 초기화함
// TIM PWM의 Channel을 TIM_OCInit에 설정된 값으로 초기화함
HAL_TIM_PWM_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_3);
/* Start OC */
HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);

HAL_TIM_PWM_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_4);
/* Start OC */
HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
}
// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{
	LED_Config(); // Configure LED_1(PA2), LED_2(PA3)
	SW_EXTILine_Config(); // Configure EXTI Line0, 1 (connected to PB0, PB1 pin) in interrupt mode
	TIMER_Config(); // Configure TIMER
	while(1);
}
/*읶터럽트 루틴*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
if(GPIO_Pin == GPIO_PIN_1){ //IRQ<1>에서 읶터럽트 요청이 들어온 건지 확읶(JOG_DN)
if(Pulse_LED <= 1000) Pulse_LED = 1000;
else Pulse_LED = Pulse_LED - 1000;
}
if(GPIO_Pin == GPIO_PIN_0){ //IRQ<0>에서 읶터럽트 요청이 들어온 건지 확읶(JOG_UP)
if(Pulse_LED >= 10000) Pulse_LED = 10000;
else Pulse_LED = Pulse_LED + 1000;
}
TIM_OCInit.Pulse = Pulse_LED - 1;
HAL_TIM_PWM_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_3);
HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);

HAL_TIM_PWM_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_4);
HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
}

//#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
