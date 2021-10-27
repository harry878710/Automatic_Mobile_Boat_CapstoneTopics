/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

double dis_F = 0.0;
double dis_rF = 0.0;
double dis_lF = 0.0;
double dis_rR = 0.0;
double dis_lR = 0.0;

char* int2char(int input) {
	int power = 0;
	if (input == 0)
		power = 4;
	else {
		for (int i = 0; i < 20; i++) {
			if ((input / (int) pow(10, i)) == 0) {
				power = i + 3;
				break;
			}
		}
	}
	char* output = (char*) malloc(power * sizeof(char));
	for (int i = 0; i < power - 3; i++) {
		output[power - i - 4] = input % 10 + '0';
		input = input / 10;
	}
	output[power - 1] = '\0';
	output[power - 2] = '\r';
	output[power - 3] = '\n';
	return output;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 1) {
			__HAL_TIM_SET_COUNTER(&htim2, 0);
		} else {
			int cnt = __HAL_TIM_GET_COUNTER(&htim2);
			dis_lF = cnt / (double) 29 / (double) 2;
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 1) {
			__HAL_TIM_SET_COUNTER(&htim2, 0);
		} else {
			int cnt = __HAL_TIM_GET_COUNTER(&htim2);
			dis_F = cnt / (double) 29 / (double) 2;
		}

		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 1) {
			__HAL_TIM_SET_COUNTER(&htim2, 0);
		} else {
			int cnt = __HAL_TIM_GET_COUNTER(&htim2);
			dis_rF = cnt / (double) 29 / (double) 2;
		}
	}
	if (htim->Instance == TIM4) {
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 1) {
			__HAL_TIM_SET_COUNTER(&htim4, 0);
		} else {
			int cnt = __HAL_TIM_GET_COUNTER(&htim4);
			dis_lR = cnt / (double) 29 / (double) 2;
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 1) {
			__HAL_TIM_SET_COUNTER(&htim4, 0);
		} else {
			int cnt = __HAL_TIM_GET_COUNTER(&htim4);
			dis_rR = cnt / (double) 29 / (double) 2;
		}
	}
}

void fast_forward(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 80);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 88);
}

void medium_forward(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 75);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 83);
}

void slow_forward(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 72);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 80);
}

void left_turn(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 80);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 80);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 0);
}

void adjust_left(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 100);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 0);
}

void right_turn(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 80);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 100);
}

void adjust_right(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 100);
}

void reverse(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 100);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 100);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 0);

	HAL_Delay(t);
}

void right_reverse(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 100);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 80);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 0);

	HAL_Delay(t);
}

void left_reverse(int t) {
	//	right wheel
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 80);
	//	left wheel
	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 100);
	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 0);

	HAL_Delay(t);
}

/**
 * Determine posture function
 *
 */

double F[5], rF[5], lF[5], rR[5], lR[5];
int state = 0;
int posture = 0;
int LED_rF = 0;
int LED_lF = 0;
int LED_right = 0;
int LED_left = 0;

void determine(int posture) {

	switch (posture) {
	case 0:
		fast_forward(500);
		break;
	case 10:
		medium_forward(500);
		break;
	case 11:
		right_reverse(1000);
		break;
	case 12:
		left_reverse(1000);
		break;
	case 20:
		medium_forward(500);
		break;
	case 21:
		medium_forward(500);
		break;
	case 22:
		left_turn(500);
		break;
	case 23:
		medium_forward(500);
		break;
	case 24:
		medium_forward(500);
		break;
	case 25:
		medium_forward(500);
		break;
	case 26:
		right_turn(500);
		break;
	case 30:
		right_turn(500);
		break;
	case 31:
		right_turn(500);
		break;
	case 32:
		left_turn(500);
		break;
	case 33:
		left_turn(500);
		break;
	case 34:
		left_turn(500);
		break;
	case 35:
		left_turn(500);
		break;
	case 40:
		left_turn(500);
		break;
	case 41:
		adjust_left(500);
		break;
	case 42:
		right_turn(500);
		break;
	case 43:
		right_turn(500);
		break;
	case 44:
		right_turn(500);
		break;
	case 45:
		right_turn(500);
		break;
	case 50:
		slow_forward(500);
		break;
	case 51:
		adjust_left(500);
		break;
	case 52:
		adjust_right(500);
		break;
	case 60:
		slow_forward(500);
		break;
	case 61:
		adjust_left(500);
		break;
	case 62:
		adjust_right(500);
		break;
	case 70:
		left_turn(500);
		break;
	case 71:
		slow_forward(500);
		break;
	case 72:
		right_turn(500);
		break;
	case 73:
		reverse(500);
		break;
	case 80:
		right_turn(500);
		break;
	case 81:
		slow_forward(500);
		break;
	case 82:
		left_turn(500);
		break;
	case 83:
		reverse(500);
		break;
	case 1001:
		state = 1;
		break;
	case 1012:
		state = 2;
		break;
	case 1023:
		state = 3;
		break;
	case 1024:
		state = 4;
		break;
	case 1032:
		state = 2;
		break;
	case 1035:
		state = 5;
		break;
	case 1037:
		state = 7;
		break;
	case 1042:
		state = 2;
		break;
	case 1046:
		state = 6;
		break;
	case 1048:
		state = 8;
		break;
	case 1051:
		state = 1;
		break;
	case 1053:
		state = 3;
		break;
	case 1057:
		state = 7;
		break;
	case 1061:
		state = 1;
		break;
	case 1064:
		state = 4;
		break;
	case 1068:
		state = 8;
		break;
	case 1072:
		state = 2;
		break;
	case 1082:
		state = 2;
		break;
	}

	if (F[0] < 15)
		reverse(500);
}

int determine_posture() {

	int posture = 0;

	// 初始化
	if (state == 0) {
		if (F[1] - F[0] > 1 && F[2] - F[1] > 1)
			posture = 1001; //進入state 1
		else
			posture = 0; //其他(預設靜止)
	}

	// 直線區
	if (state == 1) {
//		if (rR[0] < 20 && rF[0] < 100)
//			posture = 11; //嚴重右斜
//		else if (lR[0] < 20 && lF[0] < 100)
//			posture = 12; //嚴重左斜
		if (lR[0] < 30 || rR[0] < 30)
			posture = 1012; //進入state 2
		else
			posture = 10; //其他(預設走直線)
	}

	// 狹窄區
	if (state == 2) {
		if (F[0]<80 && F[1]-F[0]<10 && F[2]-F[1]<10 && F[3]-F[2]<10) {
			if (rR[0] - lR[0] > 10)
				posture = 1023; //進入state 4
			else
				posture = 1023; //進入state 3
		}
		else if (rR[0] - lR[0] > 10) { //偏左邊
			if (lR[0] < lR[1] && lR[0] < lR[2])
				posture = 26; //左斜偏左
			else if (lR[0] > lR[1] && lR[0] > lR[2])
				posture = 25; //右斜偏左
			else
				posture = 24; //正偏左
		} else if (lR[0] - rR[0] > 10) { //偏右邊
			if (rR[0] < rR[1] && rR[0] < rR[2])
				posture = 22; //右斜偏右
			else if (rR[0] > rR[1] && rR[0] > rR[2])
				posture = 23; //左斜偏右
			else
				posture = 21; //正偏右
		} else
			posture = 20; //其他(預設走直線)
	}

	// 轉彎區偏右邊
	if (state == 3) {
//		if (rR[0] > 100 && rR[1] > 100)
//			posture = 1037; //進入state 7
		if (F[0]>100 && F[1]>100 && F[2]>100)
			posture = 1032;
		else if (lR[0] > 85) { //過完第一彎  還沒考慮中間板子會影響左後方測距的問題
			if (F[0] < 35 && rF[0] < 35)
				posture = 34; //彎內接近牆壁
			else if (F[0] > 70 || lF[0] > 70)
				posture = 1035; //彎內呈出彎位置(進入state 5)
			else
				posture = 35; //其他(預設呈中間角度)
		} else { // 還沒過第一彎
			if (F[0] > 50 && lF[0] > 60)
				posture = 31; //中角度
			else if (F[0] < 30 && rF[0] < 30 && lF[0] < 40)
				posture = 33; //大角度且離牆近
			else if (F[0] < 50 && rF[0] < 50)
				posture = 32; //大角度
			else
				posture = 30; //其他(預設小角度)
		}
	}

	// 小直線區逆時針
	if (state == 5) {
		if (F[0]>100 && F[1]>100 && F[2]>100)
			posture = 1051;
		else if (F[0] < 45)
			posture = 1053; //進入state 3
		else if (rF[1] - rF[0] > 1 && rF[0] < rF[1] && rF[0] < rF[2])
			posture = 51; //右斜
		else if (rF[0] - rF[1] > 1 && rF[0] > rF[1] && rF[0] > rF[2])
			posture = 52; //左斜
		else
			posture = 50; //其他(預設走直線)
	}

	// 出彎區逆時針
	if (state == 7) {
		if (F[0] > 100 || rF[0] > 100)
			posture = 1072; //進入state 2
		else if (rR[0] > 50)
			posture = 73; //倒退
		else
			posture = 72; //呈出彎位置(準備右轉)
//		else if (rF[0] < 60)
//			posture = 71; //中角度(可走直線)
//		else
//			posture = 70; //其他(預設小角度，還要繼續左轉)
	}

	// 轉彎區偏左邊
	if (state == 4) {
//		if (lR[0] > 100 && lR[1] > 100)
//			posture = 1048; //進入state 8
		if (F[0]>180 && F[1]>180 && F[2]>180)
			posture = 1042;
		else if (rR[0] > 85) { //過完第一彎  還沒考慮中間板子會影響左後方測距的問題
			if (F[0] < 35 && lF[0] < 35)
				posture = 44; //彎內接近牆壁
			else if (F[0] > 70 && rF[0] > 70)
				posture = 1046; //彎內呈出彎位置(進入state 6)
			else
				posture = 45; //其他(預設呈中間角度)
		} else { // 還沒過第一彎
			if (F[0] > 50 && rF[0] > 60)
				posture = 41; //中角度
			else if (F[0] < 30 && lF[0] < 30 && rF[0] < 40)
				posture = 43; //大角度且離牆近
			else if (F[0] < 50 && lF[0] < 50)
				posture = 42; //大角度
			else
				posture = 40; //其他(預設小角度)
		}
	}

	// 小直線區順時針
	if (state == 6) {
		if (F[0]>180 && F[1]>180 && F[2]>180)
			posture = 1061;
		else if (F[0] < 45)
			posture = 1064; //進入state 4
		else if (lF[1] - lF[0] > 1 && lF[0] < lF[1] && lF[0] < lF[2])
			posture = 61; //右斜
		else if (lF[0] - lF[1] > 1 && lF[0] > lF[1] && lF[0] > lF[2])
			posture = 62; //左斜
		else
			posture = 60; //其他(預設走直線)
	}

	// 出彎區順時針
	if (state == 8) {
		if (F[0] > 150)
			posture = 1082; //進入state 2
		else if (lR[0] > 50)
			posture = 83; //倒退
		else
			posture = 82; //呈出彎位置(準備左轉)
//		else if (lF[0] < 60)
//			posture = 81; //中角度(可走直線)
//		else
//			posture = 80; //其他(預設小角度，還要繼續右轉)
	}

	return posture;
}

void determine_LED() {
	switch (state) {
	case 1:
		LED_lF = 0;
		LED_rF = 0;
		LED_left = 0;
		LED_right = 0;
		break;
	case 2:
		LED_lF = 3;
		LED_rF = 3;
		LED_left = 0;
		LED_right = 0;
		break;
	case 3:
		LED_lF = 0;
		LED_rF = 3;
		if (posture == 30 || posture == 31) {
			LED_left = 0;
			LED_right = 2;
		} else {
			LED_left = 2;
			LED_right = 0;
		}
		break;
	case 4:
		LED_lF = 3;
		LED_rF = 0;
		if (posture == 40 || posture == 41) {
			LED_left = 2;
			LED_right = 0;
		} else {
			LED_left = 0;
			LED_right = 2;
		}
		break;
	case 5:
		LED_lF = 2;
		LED_rF = 2;
		LED_left = 0;
		LED_right = 0;
		break;
	case 6:
		LED_lF = 1;
		LED_rF = 1;
		LED_left = 0;
		LED_right = 0;
		break;
	case 7:
		if (posture == 73) {
			LED_lF = 0;
			LED_rF = 3;
			LED_left = 0;
			LED_right = 3;
		} else {
			LED_lF = 0;
			LED_rF = 1;
			LED_left = 0;
			LED_right = 1;
		}
		break;
	case 8:
		if (posture == 83) {
			LED_lF = 3;
			LED_rF = 0;
			LED_left = 3;
			LED_right = 0;
		} else {
			LED_lF = 1;
			LED_rF = 0;
			LED_left = 1;
			LED_right = 0;
		}
		break;
	default:
		LED_lF = 2;
		LED_rF = 2;
		LED_left = 2;
		LED_right = 2;
		break;
	}

}

void LED_control() {
	switch (LED_lF) {
	case 0:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
		break;
	case 1:
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
		break;
	}

	switch (LED_rF) {
	case 0:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		break;
	case 1:
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
		break;
	}

	switch (LED_right) {
	case 0:
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0);
		break;
	case 1:
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_12);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 1);
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 1);
		break;
	}

	switch (LED_left) {
	case 0:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
		break;
	case 1:
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
		break;
	}

}

char* distances2char(double* input) {
	int digit = 0;
	for (int i = 0; i < sizeof(input) + 1; i++) {
		int decimal = (int) input[i];
		int fraction = (int) ((input[i] - decimal) * 100);
		for (int j = 0; j < 20; j++) {
			if ((decimal / (int) pow(10, j)) == 0) {
				digit += j;
				break;
			}
		}
		if (fraction != 0)
			digit += 3;
	}
	digit = digit + 2 * sizeof(input) + 3;
	char* output = (char*) malloc(digit * sizeof(char));

	int current = sizeof(input);
	int decimal = (int) input[current];
	int fraction = (int) ((input[current] - decimal) * 100);
	for (int i = 0; i < digit - 3; i++) {
		if (fraction != 0) {
			for (int j = 0; j < 2; j++) {
				output[digit - i - 4] = fraction % 10 + '0';
				fraction = fraction / 10;
				i++;
			}
			output[digit - i - 4] = '.';
			i++;
		}

		output[digit - i - 4] = decimal % 10 + '0';
		decimal = decimal / 10;
		if (decimal == 0) {
			current--;
			if (current >= 0) {
				i++;
				output[digit - i - 4] = ' ';
				i++;
				output[digit - i - 4] = ',';
				decimal = (int) input[current];
				fraction = (int) ((input[current] - decimal) * 100);
			}
		}
	}

	output[digit - 1] = '\0';
	output[digit - 2] = '\r';
	output[digit - 3] = '\n';
	return output;
}

void print_data() {

	char* s;

	s = " F distances = ";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);
	s = distances2char(F);
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);

	s = "rF distances = ";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);
	s = distances2char(rF);
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);

	s = "lF distances = ";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);
	s = distances2char(lF);
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);

	s = "rR distances = ";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);
	s = distances2char(rR);
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);

	s = "lR distances = ";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);
	s = distances2char(lR);
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);

	s = "state = ";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);
	s = int2char(state);
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);

	s = "posture = ";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);
	s = int2char(posture);
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);

	s = "\n";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);

	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);

	char* s;
	s = "connection success.\n\r";
	HAL_UART_Transmit(&huart2, (uint8_t *) s, strlen(s), 0xFFF);

	int index = 0;

	for (int i = 0; i < 5; i++) {
		F[i] = 150;
		rF[i] = 150;
		lF[i] = 150;
		rR[i] = 150;
		lR[i] = 150;
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		for (int i = 4; i > 0; i--) {
			F[i] = F[i - 1];
			rF[i] = rF[i - 1];
			lF[i] = lF[i - 1];
			rR[i] = rR[i - 1];
			lR[i] = lR[i - 1];
		}

		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_Delay(70);
		if (dis_lF > 150)
			lF[0] = lF[1];
		else
			lF[0] = dis_lF;
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		HAL_Delay(70);
		if (dis_F > 120 && dis_F < 180)
			F[0] = F[1];
		else
			F[0] = dis_F;
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		HAL_Delay(70);
		if (dis_rF > 150)
			rF[0] = rF[1];
		else
			rF[0] = dis_rF;
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);

		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
		HAL_Delay(70);
		if (dis_lR > 150)
			lR[0] = lR[1];
		else
			lR[0] = dis_lR;
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);

		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
		HAL_Delay(70);
		if (dis_rR > 150)
			rR[0] = rR[1];
		else
			rR[0] = dis_rR;
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);

		index++;
		if (index <= 10)
			continue;

		posture = determine_posture();
		determine(posture);
		determine_LED();
		LED_control();

//		print_data();
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_TIM1
                              |RCC_PERIPHCLK_TIM16|RCC_PERIPHCLK_TIM17
                              |RCC_PERIPHCLK_TIM8|RCC_PERIPHCLK_TIM2
                              |RCC_PERIPHCLK_TIM34;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim16ClockSelection = RCC_TIM16CLK_HCLK;
  PeriphClkInit.Tim17ClockSelection = RCC_TIM17CLK_HCLK;
  PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
  PeriphClkInit.Tim2ClockSelection = RCC_TIM2CLK_HCLK;
  PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 79;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 6999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffffffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0xffff;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 79;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 6999;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 7;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 100;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 7;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 100;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim17, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */
  HAL_TIM_MspPostInit(&htim17);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

  /*Configure GPIO pins : PA6 PA8 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 PC5 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
