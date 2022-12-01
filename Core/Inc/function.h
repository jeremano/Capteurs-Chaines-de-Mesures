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



void InitCapteur(I2C_HandleTypeDef*);
void Measure_T(I2C_HandleTypeDef* i2cHandle,double* Temp);
void Measure_AX(I2C_HandleTypeDef* i2cHandle,double* AccelX);
void Measure_AY(I2C_HandleTypeDef* i2cHandle,double* AccelY);
void Measure_AZ(I2C_HandleTypeDef* i2cHandle,double* AccelY);
