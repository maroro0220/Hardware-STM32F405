// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "Timer.h"
#include "BlinkLed.h"

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

GPIO_InitTypeDef PA, MOTOR, SW2;

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void ms_delay_int_count(volatile unsigned int nTime) {
	nTime = (nTime * 14000);
	for (; nTime > 0; nTime--)
		;
}

int main(int argc, char* argv[]) {

	// Send a greeting to the trace device (skipped on Release).
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;

	PA.Pin = GPIO_PIN_6 | GPIO_PIN_2 | GPIO_PIN_3; //PA 2:led1 3:led2 6:PWM
	PA.Mode = GPIO_MODE_OUTPUT_PP;
	PA.Pull = GPIO_NOPULL;
	PA.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &PA);

	MOTOR.Pin = GPIO_PIN_6 | GPIO_PIN_7; //PB 6:Positive 7: Negative
	MOTOR.Mode = GPIO_MODE_OUTPUT_PP;
	MOTOR.Pull = GPIO_NOPULL;
	MOTOR.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &MOTOR);

	SW2.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_12 | GPIO_PIN_13; //PB 0:up 1:down 2:center 12:left 13:right
	SW2.Mode = GPIO_MODE_INPUT;
	SW2.Pull = GPIO_NOPULL;
	SW2.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &SW2);
	unsigned int up = 0, down = 0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
	while (1) {
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)) { //center   short brake
			HAL_GPIO_WritePin(GPIOB, MOTOR.Pin, 1);
			ms_delay_int_count(300);
			up = 0;
			down = 0;
			//	HAL_GPIO_TogglePin(GPIOA, LD2.Pin);
		} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)) { //up
			ms_delay_int_count(300);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
			up = !up;
		} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)) { //down
			ms_delay_int_count(300);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
			down = !down;
		} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) { //left
			ms_delay_int_count(300);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			up = 0;
			down = 0;
		} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) { //right
			ms_delay_int_count(300);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
			up = 0;
			down = 0;
		}
//		if (cw&&!ccw) {
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, ccw);
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, cw);
//		} else {
//			HAL_GPIO_WritePin(GPIOB, MOTOR.Pin, 0);
//		}
//		if (ccw) {
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, ccw);
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, cw);
//		} else {
//			HAL_GPIO_WritePin(GPIOB, MOTOR.Pin, 0);
//		}

		if (up) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
			ms_delay_int_count(200);
		} else {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
		}
		if (down) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
			ms_delay_int_count(200);
		} else {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
		}

	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
