/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "function.h"
#include "math.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

//#include "../../Drivers/BSP/inc/stm32746g_discovery_lcd.h"
#include "stdio.h"
#include <math.h>
#include "Const.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AdressBMP 0xee
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//!< Variables
char mess[30];
double Temp = 0;
double AccelX = 0;
double AccelY = 0;
double AccelZ = 0;
double GyroX = 0;
double GyroY = 0;
double GyroZ = 0;
double MagnetoX = 0;
double MagnetoY = 0;
double MagnetoZ = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//!<fonction de transmission UART lors de l'utilisation de "printf"
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
return ch;
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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
    HAL_Delay(2000);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);

    InitCapteur(&hi2c1);

    /*Test écran LCD*/
    /*
    BSP_LCD_DisplayStringAt(0, LINE(1), (uint8_t *)"CAPTEURS", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, LINE(2), (uint8_t *)"TP", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, LINE(3), (uint8_t *)"2019/2020", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, LINE(4), (uint8_t *)"MSC !", CENTER_MODE);

    sprintf(mess,"Pi = %1.4f",PI);
    BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)mess,CENTER_MODE);
    HAL_Delay(1000);
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    */
    // Go through all possible i2c addresses

    /**
     * Scan du BUS I2C
     * Renvoie les adresses disponibles sur la console UART
     */
	printf("\r\n");
	printf("\r\n");
    printf("-- Test des adresses disponibles --\r\n");
    	for (uint8_t i = 0; i < 128; i++) {
    	  if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 5) == HAL_OK) {
    		  printf("%2x ", i);
    	  } else {
    		  printf("-- ");
    	  }
    	  if (i > 0 && (i + 1) % 16 == 0) printf("\n\r");
      }

      printf("\n\r");

      /**
       * Requete d'identification du capteur gyroscopique
       */
      printf("-- Test de l'identite du capteur MPU-9250 --\r\n");
      uint8_t data[48];
      data[0]=0x75;
      printf("Registre d'identification : %x\r\n",data[0]);
      if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
    	  Error_Handler();
      }
      if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
    	  Error_Handler();
      }
      printf("Valeur recu : %x\r\n",data[0]);
      if (data[0]!=0x71){
    	  printf("Error !\r\n");
    	  Error_Handler();
      }
      else{
    	  printf("Ok !\r\n");
      }

      /**
       * Requete d'identification du capteur temperature/pression
       */
      printf("\r\n");
      printf("-- Test de l'identite du capteur BPM-280 --\r\n");
      data[0]=0xd0;
      printf("Registre d'identification : %x\r\n",data[0]);
      if(HAL_I2C_Master_Transmit(&hi2c1,AdressBMP, data, 1, HAL_MAX_DELAY) != HAL_OK){
    	  Error_Handler();
      }
      if(HAL_I2C_Master_Receive(&hi2c1,AdressBMP, data, 1, HAL_MAX_DELAY) != HAL_OK){
    	  Error_Handler();
      }
      printf("Valeur recu : %x\r\n",data[0]);
      if (data[0]!=0x58){
    	  printf("Error !\r\n");
    	  Error_Handler();
      }
      else{
    	  printf("Ok !\r\n");
      }

      /**
       * Mesure cyclique de la temperature et de l'acceleration
       * La mesure se fait toutes les secondes
       * Les fonctions de mesures sont ecrites dans le fichier function.c
       */
      while (1)
      {
    	/***** Température *****/
      	/*Measure_T(&hi2c1,&Temp);
        printf("Temperature : %f\r\n",&Temp);*/

    	/***** Accélération *****/
    	/*AccelOffSet();
    	Measure_AX(&hi2c1,&AccelX);
        printf("AX %f\r\n",&AccelX);
    	Measure_AY(&hi2c1,&AccelY);
        printf("AY %f\r\n",&AccelY);
    	Measure_AZ(&hi2c1,&AccelZ);
        printf("AZ %f\r\n",&AccelZ);
		float Norme = sqrt((AccelX*AccelX)+(AccelY*AccelY)+(AccelZ*AccelZ));
		printf("Norme = %f\r\n",Norme);*/


    	/****** Gyroscope *****/
      	/*Measure_GX(&hi2c1,&GyroX);
        printf("GX %f deg/s\r\n",&GyroX);
      	Measure_GY(&hi2c1,&GyroY);
        printf("GY %f deg/s\r\n",&GyroY);
      	Measure_GZ(&hi2c1,&GyroZ);
        printf("GZ %f deg/s\r\n",&GyroZ);*/


    	/******	Magnetometre ******/

    	/* Test WIA*/

    	/*
    	data[0]=0x00;
  		if(HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
  	    	  	  Error_Handler();
  			  }
  		if(HAL_I2C_Master_Receive(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
  		    	  Error_Handler();
  		      }
  		printf("data %x",data[0]);
        printf("\r\n");
        */

      	Measure_MX(&hi2c1,&MagnetoX);
        printf("MX %f µT \r\n",&MagnetoX);
      	Measure_MY(&hi2c1,&MagnetoY);
        printf("MY %f µT \r\n",&MagnetoY);
      	Measure_MZ(&hi2c1,&MagnetoZ);
        printf("MZ %f µT \r\n",&MagnetoZ);
        printf("\r\n");
    	HAL_Delay(500);

      }
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
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
	printf("Une erreur est survenue ! Arret du programme\r\n");
	while(1){
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
	    HAL_Delay(200);
	    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
	    HAL_Delay(200);
	}

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
