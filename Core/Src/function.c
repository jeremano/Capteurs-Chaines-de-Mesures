/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "stdio.h"
#include <math.h>
#include "Const.h"
#include "i2c.h"
#include "function.h"

#define AdressMPU 0xd0
#define AdressBMP 0xee
#define RegisterResetMPU 0x6b
#define RegisterResetBMP 0xe0
#define RegisterCLKSEL 0x6b
#define ValueResetMPU 0x80
#define ValueResetBMP 0xb6
#define ValueCLKSEL 0x02
uint8_t data[48];


void Init(I2C_HandleTypeDef* i2cHandle){

	  if(i2cHandle->Instance==I2C1)
	  {
	/* MPU Reset */
	data[0]=RegisterResetMPU;
	data[1]=ValueResetMPU;
	HAL_I2C_Master_Transmit(&hi2c1,AdressMPU, data, 2, HAL_MAX_DELAY);

	/* BMP Reset */
	data[0]=RegisterResetBMP;
	data[1]=ValueResetBMP;
	HAL_I2C_Master_Transmit(&hi2c1,AdressBMP, data, 2, HAL_MAX_DELAY);

	HAL_Delay(100);

	/* Choix de l'horloge */
	data[0]=RegisterCLKSEL;
	data[1]=ValueCLKSEL;
	HAL_I2C_Master_Transmit(&hi2c1,AdressMPU, data, 2, HAL_MAX_DELAY);
	  }
}
