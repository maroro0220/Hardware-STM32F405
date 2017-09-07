/*
 * Init.h
 *
 *  Created on: 2017. 9. 5.
 *      Author: student
 */

#ifndef INIT_H_
#define INIT_H_
#include "stm32f4xx_hal.h"
#define UART_TxBufferSize (countof(UART_TxBuffer) - 1) // 송싞 버퍼 사이즈 정의
#define UART_RxBufferSize 0xFF // 수싞 버퍼 사이즈를 0xFF로 정의
#define countof(a) (sizeof(a) / sizeof(*(a))) // 데이터 사이즈

#define FND_I2C_ADDR 0x20<<1
#define IN_PORT0 0x00
#define IN_PORT1 0x01
#define OUT_PORT0 0x02
#define OUT_PORT1 0x03
#define POLARITY_IVE_PORT0 0x04
#define POLARITY_IVE_PORT1 0x05
#define CONFIG_PORT0 0x06
#define CONFIG_PORT1 0x07

#define SHT_I2C_ADDR 0x40<<1
#define SHT_TEMPER_HOLD 0xE3
#define SHT_TEMPER_NOHOLD 0xF3
#define SHT_HUM_HOLD 0xE5
#define SHT_HUM_NOHOLD 0xF5
#define SHT_USERR_WRITE 0xE6
#define SHT_USERR_READ 0xE7
#define SHT_RESET_SOFT 0xFE
//#define I2C_ADDRESS FND_I2C_ADDR

// UART 통싞용 변수 선언

unsigned char FND_data[10] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0x3E, 0xE0, 0xFE, 0xF6};


GPIO_InitTypeDef LED, LD, VR, CLCD, SONIC_T, SONIC_E, PWM, MOTOR, JOG, PI,USART_1,USART_2, I2C_2,PIR; // GPIO의 초기화를 위한 구조체형의 변수를 선언
ADC_HandleTypeDef AdcHandler; // ADC의 초기화를 위한 구조체형의 변수를 선언
ADC_ChannelConfTypeDef sConfig;
TIM_HandleTypeDef TimHandle,TimHandle2, TimHandle3;
TIM_OC_InitTypeDef TIM_OCInit;
UART_HandleTypeDef UartHandle1,UartHandle2;
I2C_HandleTypeDef I2CxHandle;
int adc_value; // ADC값 저장 변수

/*지연루틴*/
static void ms_delay_int_count(volatile unsigned int nTime) // ms 지연
{
	nTime = (nTime * 14000);
	for (; nTime > 0; nTime--)
		;
}
static void us_delay_int_count(volatile unsigned int nTime) // us 지연
		{
	nTime = (nTime * 12);
	for (; nTime > 0; nTime--)
		;
}
void LED_Config() {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	LED.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	LED.Mode = GPIO_MODE_OUTPUT_PP;
	LED.Pull = GPIO_NOPULL;
	LED.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &LED);
}
void VR_Config() {
	__HAL_RCC_ADC1_CLK_ENABLE(); // ADC 클럭 홗성화
	__HAL_RCC_GPIOA_CLK_ENABLE(); // ADC 핀으로 사용할 GPIOx 홗성화(VR:PA4)
	VR.Pin = GPIO_PIN_4; // GPIO에서 사용할 PIN 설정
	VR.Mode = GPIO_MODE_ANALOG; // Input Analog Mode 모드
	HAL_GPIO_Init(GPIOA, &VR);
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
void CLCD_Config() {

	// CLCD용 GPIO (GPIOC)의 초기설정을 함
	__HAL_RCC_GPIOC_CLK_ENABLE();
	// CLCD_RS(PC8), CLCD_E(PC9, DATA 4~5(PC12~15)
	CLCD.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14
			| GPIO_PIN_15;
	CLCD.Mode = GPIO_MODE_OUTPUT_PP;
	CLCD.Pull = GPIO_NOPULL;
	CLCD.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOC, &CLCD);
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
//	CLCD_write(0, 0x01);
	while (*buf) {
		CLCD_write(1, *buf++);
	}

}
//int SHT_Config() {
//	double T;
//	TxBuffer[0] = SHT_TEMPER_HOLD;
//	uint8_t SHRxBuffer[10];
////	HAL_Delay(85);
//	HAL_I2C_Master_Transmit(&I2CxHandle, (uint16_t) I2C_ADDRESS, (uint8_t*) TxBuffer, 1, 100);
//	HAL_I2C_Master_Receive(&I2CxHandle, (uint16_t) I2C_ADDRESS, (uint8_t*) SHRxBuffer, 3, 100);
//	Temp_data = (SHRxBuffer[0] << 8) | (SHRxBuffer[1] & 0xFC); //0xFC=>stat bit=00
//	T = (-46.85) + (175.72 * (Temp_data)/65536);
//	return T;
//}
void SONIC_Config() {
	__HAL_RCC_GPIOC_CLK_ENABLE();
	SONIC_T.Pin = GPIO_PIN_0;
	SONIC_T.Mode = GPIO_MODE_OUTPUT_PP;
	SONIC_T.Pull = GPIO_NOPULL;
	SONIC_T.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOC, &SONIC_T);
}
void SONICE_Config() {
	__HAL_RCC_GPIOC_CLK_ENABLE();
	SONIC_E.Pin = GPIO_PIN_1;
	SONIC_E.Mode = GPIO_MODE_IT_RISING;
	SONIC_E.Pull = GPIO_NOPULL;
	SONIC_E.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &SONIC_E);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}
void TIMER_Config() {
	__HAL_RCC_TIM2_CLK_ENABLE();
	TimHandle2.Init.Prescaler = 8400 - 1;
	TimHandle2.Instance = TIM2;
	TimHandle2.Init.Period = 100 - 1;
	TimHandle2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle2.Init.CounterMode = TIM_COUNTERMODE_UP;

	HAL_TIM_Base_Init(&TimHandle2);
	HAL_TIM_Base_Start_IT(&TimHandle2);
	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	__HAL_RCC_TIM3_CLK_ENABLE();
	TimHandle3.Instance = TIM3;
	TimHandle3.Init.Prescaler = 8400 - 1;
	TimHandle3.Init.Period = 100 - 1;
	TimHandle3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle3.Init.CounterMode = TIM_COUNTERMODE_UP;

	HAL_TIM_Base_Init(&TimHandle3);
	HAL_TIM_Base_Start_IT(&TimHandle3);
	HAL_NVIC_SetPriority(TIM3_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

//	//Set TIMx OC instance
//	TIM_OCInit.OCMode = TIM_OCMODE_TIMING; // OC 동작 모드 설정
//	TIM_OCInit.Pulse = 5000 - 1; // CCR의 설정값 ARR(Period) 10000일 때
//	// TIM OC의 Channel을 TIM_OCInit에 설정된 값으로 초기화함
//	HAL_TIM_OC_Init (&TimHandle);
//	HAL_TIM_OC_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_1);
//	HAL_TIM_OC_Start_IT(&TimHandle, TIM_CHANNEL_1);

	/*OC 2Channel*/
//	TIM_OCInit.Pulse = 500 - 1;
//	HAL_TIM_OC_Init(&TimHandle);

//	HAL_TIM_OC_ConfigChannel(&TimHandle,&TIM_OCInit, TIM_CHANNEL_1);
//	HAL_TIM_OC_Start_IT(&TimHandle, TIM_CHANNEL_1);

//	TIM_OCInit.Pulse=500-1;
//	HAL_TIM_OC_ConfigChannel(&TimHandle,&TIM_OCInit, TIM_CHANNEL_2);
//	HAL_TIM_OC_Start_IT(&TimHandle, TIM_CHANNEL_2);


	/*PWM*/
//	TIM_OCInit.Pulse = Pulse - 1; // CCR의 설정값
//	TIM_OCInit.OCPolarity = TIM_OCPOLARITY_LOW; // OC 출력을 LOW로 설정 없어도 동작함
//	HAL_TIM_PWM_Init(&TimHandle); // TIM PWM을 TimHandle에 설정된 값으로 초기화함
//	// TIM PWM의 Channel을 TIM_OCInit에 설정된 값으로 초기화함
//	HAL_TIM_PWM_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_3);
//	/* Start OC */
//	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
//
//	HAL_TIM_PWM_ConfigChannel(&TimHandle, &TIM_OCInit, TIM_CHANNEL_4);
//	/* Start OC */
//	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);

	//205p, 267p

}
void MOTOR_Config() {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	PWM.Pin = GPIO_PIN_6; // 6:PWM
	PWM.Mode = GPIO_MODE_OUTPUT_PP;
	PWM.Pull = GPIO_NOPULL;
	PWM.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &PWM);

	MOTOR.Pin = GPIO_PIN_6 | GPIO_PIN_7; //PB 6:Positive 7: Negative
	MOTOR.Mode = GPIO_MODE_OUTPUT_PP;
	MOTOR.Pull = GPIO_NOPULL;
	MOTOR.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &MOTOR);
}
void JOG_Config() {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	JOG.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_12 | GPIO_PIN_13; //PB 0:up 1:down 2:center 12:left 13:right
	JOG.Mode = GPIO_MODE_IT_RISING;
	JOG.Pull = GPIO_NOPULL;
	JOG.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &JOG);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 4, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//	void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {}
}
void PIEZO_Config() {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	PI.Pin = GPIO_PIN_15; //PortB 15:PIEZO
	PI.Mode = GPIO_MODE_OUTPUT_PP;
	PI.Pull = GPIO_NOPULL;
	PI.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &PI);
//	unsigned int period, buf,flag=0;
//	while(1){
//
//		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)){
////			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_15);
//			ms_delay_int_count(300);
//			flag=~flag;
//		}
//		if(flag){
//			for (period = 0x1000; period >= 1; period--) {
//					HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_15);
//					buf = period;
//					while (buf--)
//						;
////					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,0);
////					buf = period;
////					while (buf--)
////						;
//				}
//		}
//	}
}
void LD4_Config() {
	__HAL_RCC_GPIOC_CLK_ENABLE();
	LD.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	LD.Mode = GPIO_MODE_OUTPUT_PP;
	LD.Pull = GPIO_NOPULL;
	LD.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &LED);
}
void PIR_Config(){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	PIR.Pin=GPIO_PIN_8;
	PIR.Mode=GPIO_MODE_IT_RISING;
	PIR.Pull=GPIO_NOPULL;
	PIR.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC,&PIR);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 8, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


// -- UART의 초기설정을 위한 함수
	uint8_t UART_TxBuffer[] = "UART Example 1 (Transmission Success!!)\n\r";
	uint8_t UART_RxBuffer[UART_RxBufferSize];
void UART_config() { // USART2_TX(PA2), USART2_RX(PA3)
	// UART의 클럭을 홗성화

	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_USART2_CLK_ENABLE();
//	// GPIO A포트 2번 핀을 USART Tx, 3번 핀을 USART Rx 로 설정
	USART_2.Pin=GPIO_PIN_2|GPIO_PIN_3;
	USART_2.Mode=GPIO_MODE_AF_PP;
	USART_2.Pull=GPIO_NOPULL;
	USART_2.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	USART_2.Alternate=GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA,&USART_2);

	UartHandle2.Instance= USART2;
	UartHandle2.Init.BaudRate = 9600;
	UartHandle2.Init.WordLength = UART_WORDLENGTH_8B;	//data크기 8비트?
	UartHandle2.Init.StopBits = UART_STOPBITS_1;
	UartHandle2.Init.Parity = UART_PARITY_NONE;
	UartHandle2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartHandle2.Init.Mode = UART_MODE_TX_RX;
	UartHandle2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&UartHandle2);
	HAL_NVIC_SetPriority(USART2_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	__HAL_RCC_USART1_CLK_ENABLE()
	;
	USART_1.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	USART_1.Mode = GPIO_MODE_AF_PP;
	USART_1.Pull = GPIO_NOPULL;
	USART_1.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	USART_1.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &USART_1);
//	// UART의 동작 조건 설정
	UartHandle1.Instance = USART1;
	UartHandle1.Init.BaudRate = 9600;
	UartHandle1.Init.WordLength = UART_WORDLENGTH_8B;	//data크기 8비트?
	UartHandle1.Init.StopBits = UART_STOPBITS_1;
	UartHandle1.Init.Parity = UART_PARITY_NONE;
	UartHandle1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartHandle1.Init.Mode = UART_MODE_TX_RX;
	UartHandle1.Init.OverSampling = UART_OVERSAMPLING_16;

	// UART 구성정보를 UartHandle에 설정된 값으로 초기화 함
	HAL_UART_Init(&UartHandle1);
	HAL_NVIC_SetPriority(USART1_IRQn, 7, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

//	HAL_UART_Transmit(&UartHandle1, (uint8_t*) TxBuffer, TxBufferSize, 0xFFFF);
//
//		while (1) {
//			// 받은 데이터를 RxBuffer에 담는다.
//			if (HAL_UART_Receive(&UartHandle1, (uint8_t*) RxBuffer, 3, 5)== HAL_OK) {
//				RxBuffer[3] = '\n';
//				RxBuffer[4] = '\r';
//				// RxBuffer에 저장되어 있는 내용을 UART로 보낸다.
//				HAL_UART_Transmit(&UartHandle1, (uint8_t*) RxBuffer, 5, 5);
//			}
//		}
}

void FND_clear(){
	uint8_t TxBuffer[5];
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xFF; TxBuffer[2]=0x00;
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)FND_I2C_ADDR,(uint8_t*)&TxBuffer,3,100);
	HAL_Delay(0.01);//system tick?을 이용한 지연함수. systick.  _it.c에 들어가보면 있음. system값을 증가
}
void I2C_config(){
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();//I2C2
	//GPIO B포트 8, 9번 핀을 I2C 전환기능으로 설정
	I2C_2.Pin=GPIO_PIN_10|GPIO_PIN_11;
	I2C_2.Mode=GPIO_MODE_AF_OD;/*!< Alternate Function Open Drain Mode    */
	I2C_2.Pull=GPIO_PULLUP; //pullup mode
	I2C_2.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	I2C_2.Alternate=GPIO_AF4_I2C2;/* I2C2 Alternate Function mapping */
	HAL_GPIO_Init(GPIOB,&I2C_2);

	//I2c init
	I2CxHandle.Instance=I2C2;
	I2CxHandle.Init.ClockSpeed=400000;
	I2CxHandle.Init.DutyCycle=I2C_DUTYCYCLE_16_9;
	I2CxHandle.Init.OwnAddress1=FND_I2C_ADDR;//FND's ADDRESS
	I2CxHandle.Init.AddressingMode=I2C_ADDRESSINGMODE_7BIT;
//	I2CxHandle.Init.DualAddressMode=I2C_DUALADDRESS_DISABLE;
//	I2CxHandle.Init.OwnAddress2=0;
//	I2CxHandle.Init.GeneralCallMode=I2C_GENERALCALL_DISABLE;
//	I2CxHandle.Init.NoStretchMode=I2C_NOSTRETCH_DISABLE;
	//위에 4개 없어도됨 지금은
	HAL_I2C_Init(&I2CxHandle);

	/*
	 main(){I2C_config();
	TxBuffer[0]=CONFIG_PORT0; TxBuffer[1]=0x00; TxBuffer[2]=0x00;
	HAL_I2C_Master_Transmit(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)TxBuffer, 3, 100);
	i2cSendValue(123456);}
	*/
}
void i2cFNDSendValue(int tempCycles){
	unsigned int temp;
	uint8_t TxBuffer[5];
	temp = tempCycles/100000;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0x7F; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)FND_I2C_ADDR,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=100000;
	FND_clear();

	temp = tempCycles/10000;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xBF; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)FND_I2C_ADDR,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=10000;
	FND_clear();

	temp = tempCycles/1000;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xDF; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)FND_I2C_ADDR,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=1000;
	FND_clear();

	temp = tempCycles/100;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xEF; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)FND_I2C_ADDR,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=100;
	FND_clear();

	temp = tempCycles/10;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xF7; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)FND_I2C_ADDR,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=10;
	FND_clear();

	temp = tempCycles;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xFB; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)FND_I2C_ADDR,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	FND_clear();
}
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//	if((htim->Instance==TIM2)&&(fu)){
//		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_2);
//	}
//	if((htim->Instance==TIM3)&&(fd)){
//		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_3);
//	}
//}
//void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
//{if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1){}
//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
//}
//}
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//	if (flag==1) {
//		cnt=0;
//		HAL_TIM_Base_Start_IT(&TimHandle);
//		flag = 0;
//		SONIC_E.Pin = GPIO_PIN_1;
//		SONIC_E.Mode = GPIO_MODE_IT_FALLING;
//		HAL_GPIO_Init(GPIOC, &SONIC_E);
//	} else if(flag==0) {
//		HAL_TIM_Base_Stop_IT(&TimHandle);
//		flag = 1;
//		SONIC_E.Pin = GPIO_PIN_1;
//		SONIC_E.Mode = GPIO_MODE_IT_RISING;
//		HAL_GPIO_Init(GPIOC, &SONIC_E);
//		sprintf(output_buffer, "Distance=%d cm", cnt);
//		clcd_put_string(output_buffer);
//	}
//}
//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandler) {
//	adc_value = HAL_ADC_GetValue(AdcHandler); // ADC 변홖 결과 값을 저장
//	// 변홖 결과값을 이용하여 LED의 주기를 변경
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
//	us_delay_int_count(adc_value);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
//	us_delay_int_count(3000);
//}

/*UART IT*/
//HAL_UART_Transmit(&UartHandle, (uint8_t*) TxBuffer, TxBufferSize, 0xFFFF);
//HAL_UART_Receive_IT(&UartHandle, (uint8_t*) RxBuffer, 2);
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {}
#endif /* INIT_H_ */
