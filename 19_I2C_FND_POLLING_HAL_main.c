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

//FND
#define FND_I2C_ADDR 0x20<<1
#define IN_PORT0 0x00
#define IN_PORT1 0x01
#define OUT_PORT0 0x02
#define OUT_PORT1 0x03
#define POLARITY_IVE_PORT0 0x04
#define POLARITY_IVE_PORT1 0x05
#define CONFIG_PORT0 0x06
#define CONFIG_PORT1 0x07

#define I2C_ADDRESS FND_I2C_ADDR

GPIO_InitTypeDef GPIO_Init_Struct;
I2C_HandleTypeDef I2CxHandle;
// FND 출력 데이터 선언
unsigned char FND_data[10] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0x3E, 0xE0, 0xFE, 0xF6};
// I2C 통傷 변수 선언
uint8_t TxBuffer[5];

void I2C_config(){
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();//I2C2
	//GPIO B포트 8, 9번 핀을 I2C 전환기능으로 설정
	GPIO_Init_Struct.Pin=GPIO_PIN_10|GPIO_PIN_11;
	GPIO_Init_Struct.Mode=GPIO_MODE_AF_OD;/*!< Alternate Function Open Drain Mode    */
	GPIO_Init_Struct.Pull=GPIO_PULLUP; //pullup mode
	GPIO_Init_Struct.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init_Struct.Alternate=GPIO_AF4_I2C2;/* I2C2 Alternate Function mapping */
	HAL_GPIO_Init(GPIOB,&GPIO_Init_Struct);

	//I2c init
	I2CxHandle.Instance=I2C2;
	I2CxHandle.Init.ClockSpeed=400000;
	I2CxHandle.Init.DutyCycle=I2C_DUTYCYCLE_16_9;
	I2CxHandle.Init.OwnAddress1=I2C_ADDRESS;//FND's ADDRESS
	I2CxHandle.Init.AddressingMode=I2C_ADDRESSINGMODE_7BIT;
//	I2CxHandle.Init.DualAddressMode=I2C_DUALADDRESS_DISABLE;
//	I2CxHandle.Init.OwnAddress2=0;
//	I2CxHandle.Init.GeneralCallMode=I2C_GENERALCALL_DISABLE;
//	I2CxHandle.Init.NoStretchMode=I2C_NOSTRETCH_DISABLE;
	//위에 4개 없어도됨 지금은
	HAL_I2C_Init(&I2CxHandle);
}
void i2cSendValue(int tempCycles){
	unsigned int temp;
	temp = tempCycles/100000;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0x7F; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)I2C_ADDRESS,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=100000;
	FND_clear();

	temp = tempCycles/10000;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xBF; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)I2C_ADDRESS,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=10000;
	FND_clear();

	temp = tempCycles/1000;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xDF; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)I2C_ADDRESS,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=1000;
	FND_clear();

	temp = tempCycles/100;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xEF; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)I2C_ADDRESS,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=100;
	FND_clear();

	temp = tempCycles/10;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xF7; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)I2C_ADDRESS,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	tempCycles%=10;
	FND_clear();

	temp = tempCycles;
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xFB; TxBuffer[2]=FND_data[temp];
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)I2C_ADDRESS,(uint8_t*)&TxBuffer,3,100);// 3이면 0,1,2 data를 여러개 하려면 더 늘려주면됨. 지금은 데이터 2개. 0은 command
	FND_clear();
}
// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"
void FND_clear(){
	TxBuffer[0]=OUT_PORT0; TxBuffer[1]=0xFF; TxBuffer[2]=0x00;
	HAL_I2C_Master_Transmit(&I2CxHandle,(uint16_t)I2C_ADDRESS,(uint8_t*)&TxBuffer,3,100);
	HAL_Delay(0.01);//system tick?을 이용한 지연함수. systick.  _it.c에 들어가보면 있음. system값을 증가
}
int
main(int argc, char* argv[])
{
	I2C_config();
	TxBuffer[0]=CONFIG_PORT0; TxBuffer[1]=0x00; TxBuffer[2]=0x00;
	HAL_I2C_Master_Transmit(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)TxBuffer, 3, 100);
  while (1)
    {
	  i2cSendValue(123456); // FND에 12345를 출력
    }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
