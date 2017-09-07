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
#include <Init.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_uart.h>
#include <string.h>
#include <sys/_stdint.h>

#define SHT_I2C_ADDR 0x40<<1
#define SHT_TEMPER_HOLD 0xE3
#define SHT_TEMPER_NOHOLD 0xF3
#define SHT_HUM_HOLD 0xE5
#define SHT_HUM_NOHOLD 0xF5
#define SHT_USERR_WRITE 0xE6
#define SHT_USERR_READ 0xE7
#define SHT_RESET_SOFT 0xFE
#define I2C_ADDRESS SHT_I2C_ADDR
GPIO_InitTypeDef GPIO_Init_Struct;

uint8_t TxBuffer[3];
uint8_t RxBuffer[3];
uint8_t UART_InTxBuffer[100];
uint8_t UART_InRxBuffer[100];

int Temp_data = 0;
int PT = 0, WT = 0;
//int PIR_flag=1;
unsigned int period, buf;
unsigned int Mot_flag = 0, Led_flag = 0, LT_flag = 0;


int SHT_Config() {
	double T;
	TxBuffer[0] = SHT_TEMPER_HOLD;
	uint8_t SHRxBuffer[10];
//	HAL_Delay(85);
	HAL_I2C_Master_Transmit(&I2CxHandle, (uint16_t) I2C_ADDRESS,
			(uint8_t*) TxBuffer, 1, 100);
	HAL_I2C_Master_Receive(&I2CxHandle, (uint16_t) I2C_ADDRESS,
			(uint8_t*) SHRxBuffer, 3, 100);
	Temp_data = (SHRxBuffer[0] << 8) | (SHRxBuffer[1] & 0xFC); //0xFC=>stat bit=00
	T = (-46.85) + (175.72 * (Temp_data) / 65536);
	return T;
}
void MOTOR_SW(int flag) {
	if (flag)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
	else
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
}
void LED_SW(int flag) {
	if (flag == 1) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
	} else if (flag == 0) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
	} else if (flag == 2) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
	}
}
void PIEZO_SW(int flag) {
	if (flag) {
		for (period = 0x1000; period >= 1; period--) {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
			buf = period;
			while (buf--)
				;
		}
	}
}
int main(int argc, char* argv[]) {
//	PIR_Config();
	I2C_config();
	UART_config();
	CLCD_Config();
	CLCD_init();
	PIEZO_Config();
	LED_Config();
	MOTOR_Config();


		TxBuffer[0] = SHT_RESET_SOFT; //Trigger T measurement hold master 1110’0011
		HAL_I2C_Master_Transmit(&I2CxHandle, (uint16_t) I2C_ADDRESS,
				(uint8_t*) &TxBuffer, 1, 100);
		HAL_UART_Transmit(&UartHandle1, (uint8_t*) UART_TxBuffer,
				strlen(UART_TxBuffer), 100);

		strcpy(UART_InTxBuffer, "Wanna Temp \n\r");
		HAL_UART_Transmit(&UartHandle1, (uint8_t*) UART_InTxBuffer,
				strlen(UART_InTxBuffer), 100);

		while (1) {
//			if (PIR_flag) {
//			PIEZO_SW(1);
//			}
			PT = SHT_Config();
			sprintf(RxBuffer, "NOW temp = %d C", PT);
			CLCD_write(0, 0x80);
			clcd_put_string(RxBuffer);

			HAL_Delay(300); //ms

			MOTOR_SW(Mot_flag);
			LED_SW(Led_flag);

			HAL_Delay(100);
			HAL_UART_Receive_IT(&UartHandle1, (uint8_t*) UART_InRxBuffer, 2);


		}
}
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//	if(GPIO_Pin==GPIO_PIN_8){
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
//	}
////	PIR_flag=0;
//}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	UART_InRxBuffer[2] = '\n';
	UART_InRxBuffer[3] = '\r';
	HAL_UART_Transmit(huart, (uint8_t*) UART_InRxBuffer, 4, 0xFFFF);
	WT = (UART_InRxBuffer[0] - '0') * 10 + (UART_InRxBuffer[1] - '0');
	sprintf(RxBuffer, "Want temp = %d C", WT);
	CLCD_write(0, 0xc0);
	clcd_put_string(RxBuffer);

	if (WT < PT) {
		Mot_flag = 1;
		Led_flag = 0;
	} else if (WT > PT) {
		Mot_flag = 0;
		Led_flag = 1;
	}
	else if (WT == PT) {
		Mot_flag = 0;
		Led_flag = 2;
	}
	if (PT > 60) {
		PIEZO_SW(1);
	}

//	HAL_UART_Receive_IT(&UartHandle2, (uint8_t*) UART_InRxBuffer, 2); 이 경우에는 여기에 쓰면 안됨.
}

// ----------------------------------------------------------------------------
