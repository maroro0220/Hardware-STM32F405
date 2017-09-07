/**
 ******************************************************************************
 * @file    GPIO/GPIO_EXTI/Src/stm32f4xx_it.c
 * @author  MCD Application Team
 * @version V1.2.6
 * @date    06-May-2016
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

/** @addtogroup STM32F4xx_HAL_Examples
 * @{
 */

/** @addtogroup GPIO_EXTI
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
// TIM 읶터럽트 ISR을 위한 TimHandler 변수를 외부정의 변수로 선언

//extern GPIO_InitTypeDef LED, VR, CLCD, SONIC_T, SONIC_E, PWM, MOTOR, JOG, PI; // GPIO의 초기화를 위한 구조체형의 변수를 선언
extern ADC_HandleTypeDef AdcHandler; // ADC의 초기화를 위한 구조체형의 변수를 선언
//extern ADC_ChannelConfTypeDef sConfig;
extern TIM_HandleTypeDef TimHandle;
extern GPIO_InitTypeDef PIR;
extern UART_HandleTypeDef UartHandle1;
/* 기본 예외 처리 핸들러 함수 구현 */

void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void) {
	HAL_IncTick();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles External line 0 interrupt request.
 * @param  None
 * @retval None
 */

//void EXTI0_IRQHandler(){
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
//}
//void EXTI1_IRQHandler(){
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
//}
//void EXTI2_IRQHandler(){
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
//}
//void EXTI15_10_IRQHandler(){
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
//}
void EXTI9_5_IRQHandler(){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}
/* 타이머 읶터럽트 처리 핸들러 함수 구현 */

//void TIM2_IRQHandler(void) // TIM2_IRQHandler 추가
//{
//	HAL_TIM_IRQHandler(&TimHandle); // TIM 읶터럽트 Callback 함수
//}
//
//void ADC_IRQHandler(){
//	HAL_ADC_IRQHandler(&AdcHandler);
//}
void USART1_IRQHandler(void) // USART2_IRQHandler 추가
{
HAL_UART_IRQHandler(&UartHandle1); // UART 읶터럽트 Callback 함수
}
/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
 {
 }*/

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
