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
void Measure_A(I2C_HandleTypeDef*,double* AccelX,double* AccelY,double* AccelZ);
