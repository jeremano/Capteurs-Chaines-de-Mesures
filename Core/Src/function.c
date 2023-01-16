/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : function.h
  * @brief          : Header for function.c file.
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

/*!< Definition des adresses des registres */
#define AdressBMP 0xee
#define RegisterResetMPU 0x6b
#define RegisterResetBMP 0xe0
#define RegisterCLKSEL 0x6b
#define ValueResetMPU 0x80
#define ValueResetBMP 0xb6
#define ValueCLKSEL 0x02
#define AccelGain 0.938
#define GyroSensitivity 131
double XOffSet = 0;
double YOffSet = 0;
double ZOffSet = 0;
uint8_t data[10];

/**
 * @brief			: Fonction d'initialisation du capteur MPU
 * @var				: Instance I2C
 * @retval			: None
 */
void InitCapteur(I2C_HandleTypeDef* i2cHandle)
{
	if(i2cHandle->Instance==I2C1)
	{
	/*! MPU Reset */
	data[0]=RegisterResetMPU;
	data[1]=ValueResetMPU;
	HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY);

	/*! BMP Reset */
	data[0]=RegisterResetBMP;
	data[1]=ValueResetBMP;
	HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY);

	/*Activation du capteur AK8963C*/
	data[0]=INT_PIN_CFG;
	data[1]=0x02;
	HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY);

	data[0]=AK8963_ST1;
	data[1]=0b00000001;
	HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 2, HAL_MAX_DELAY);

	data[0]=AK8963_ST2;
	data[1]=0b00010000;
	HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 2, HAL_MAX_DELAY);

	data[0]=AK8963_CNTL;
	data[1]=0b00010110;
	HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 2, HAL_MAX_DELAY);

	HAL_Delay(100);

	/*! Choix de l'horloge */
	data[0]=RegisterCLKSEL;
	data[1]=ValueCLKSEL;
	HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY);
	}
}

/**
 * @brief			: Fonction de mesure de la temperature
 * 					  du capteur MPU
 * @var				: Instance I2C
 * 					  Pointeur vers la variable temperature a modifier
 * @retval			: La temperature au travers du pointeur transmis
 */
void Measure_T(I2C_HandleTypeDef* i2cHandle,double *Temp){
	if(i2cHandle->Instance==I2C1)
		  {
		data[0]=TEMP_OUT_H;
		if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }
		*Temp = (((data[0]<<8)+(data[1]) - ROOM_TEMP_OFFSET)/TEMP_SENS) + ROOM_TEMP_OFFSET;
		  }
}

void AccelOffSet(void){
    data[0]=0x77;
    if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
    	Error_Handler();
    	}
    if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
    	Error_Handler();
    	}
    int XOffSetH = data[0];
    data[0]=0x78;
    if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
    	Error_Handler();
    	}
    if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
    	Error_Handler();
    	}
    int XOffSetL = data[0];
    XOffSet = ((XOffSetH<<7) + (XOffSetL>>1));
	data[0]=0x7A;
	if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    Error_Handler();
	    }
	if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    Error_Handler();
	    }
	int YOffSetH = data[0];
	data[0]=0x7B;
	if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
		Error_Handler();
	    }
	if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    Error_Handler();
	    }
	int YOffSetL = data[0];
    YOffSet = ((YOffSetH<<7) + (YOffSetL>>1));
	data[0]=0x7D;
	if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
		Error_Handler();
		}
	if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
		Error_Handler();
		}
	int ZOffSetH = data[0];
	data[0]=0x7E;
	if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
		Error_Handler();
		}
	if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
		Error_Handler();
		}
	int ZOffSetL = data[0];
    ZOffSet = ((ZOffSetH<<7) + (ZOffSetL>>1));
}

/**
 * @brief			: Fonction de mesure de l'acceleration
 * 					  suivant X du capteur MPU
 * @var				: Instance I2C
 * 					  Pointeur vers la variable acceleration de X a modifier
 * @retval			: L'acceleration au travers du pointeur transmis
 */
void Measure_AX(I2C_HandleTypeDef* i2cHandle,double* AccelX){
	if(i2cHandle->Instance==I2C1){
		data[0]=ACCEL_XOUT_H;
		if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }
		int16_t AccelXRaw = ((data[0]<<8) | data[1]);
		*AccelX = (AccelGain*AccelXRaw*2)/(32767.0);/*((AccelGain * AccelXRaw) - XOffSet)/32767.0;*/
	}
}

/**
 * @brief			: Fonction de mesure de l'acceleration
 * 					  suivant Y du capteur MPU
 * @var				: Instance I2C
 * 					  Pointeur vers la variable acceleration de Y a modifier
 * @retval			: L'acceleration au travers du pointeur transmis
 */
void Measure_AY(I2C_HandleTypeDef* i2cHandle,double* AccelY){
	if(i2cHandle->Instance==I2C1){
		data[0]=ACCEL_YOUT_H;
		if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }
		int16_t AccelYRaw = ((data[0]<<8) | data[1]);
		*AccelY = (AccelGain*AccelYRaw*2)/(32767.0);/*((AccelGain * AccelYRaw) - YOffSet)/32767.0;*/
	}
}

/**
 * @brief			: Fonction de mesure de l'acceleration
 * 					  suivant Z du capteur MPU
 * @var				: Instance I2C
 * 					  Pointeur vers la variable acceleration de Z a modifier
 * @retval			: L'acceleration au travers du pointeur transmis
 */
void Measure_AZ(I2C_HandleTypeDef* i2cHandle,double* AccelZ){
	if(i2cHandle->Instance==I2C1){
		data[0]=ACCEL_ZOUT_H;
		if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }
		int16_t AccelZRaw = ((data[0]<<8) | data[1]);  /*= ((data[0]<<8)+(data[1]));*/
		*AccelZ = (AccelGain*AccelZRaw*2)/(32767.0);/*((AccelGain * AccelZRaw) - ZOffSet)/32767.0;*/
	}
}

/**
 * @brief			: Fonction de mesure de la rotation
 * 					  suivant X du capteur MPU
 * @var				: Instance I2C
 * 					  Pointeur vers la variable rotation de X a modifier
 * @retval			: La rotation au travers du pointeur transmis
 */
void Measure_GX(I2C_HandleTypeDef* i2cHandle,double* GyroX){
	if(i2cHandle->Instance==I2C1){
		data[0]=GYRO_XOUT_H;
		if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }
		int16_t GyroXRaw = ((data[0]<<8) | data[1]);
		*GyroX = ((GyroXRaw)/(32767.0))*GyroSensitivity;
	}
}

/**
 * @brief			: Fonction de mesure de la rotation
 * 					  suivant Y du capteur MPU
 * @var				: Instance I2C
 * 					  Pointeur vers la variable rotation de Y a modifier
 * @retval			: La rotation au travers du pointeur transmis
 */
void Measure_GY(I2C_HandleTypeDef* i2cHandle,double* GyroY){
	if(i2cHandle->Instance==I2C1){
		data[0]=GYRO_YOUT_H;
		if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }
		int16_t GyroYRaw = ((data[0]<<8) | data[1]);
		*GyroY = ((GyroYRaw)/(32767.0))*GyroSensitivity;
	}
}

/**
 * @brief			: Fonction de mesure de la rotation
 * 					  suivant Z du capteur MPU
 * @var				: Instance I2C
 * 					  Pointeur vers la variable rotation de Z a modifier
 * @retval			: La rotation au travers du pointeur transmis
 */
void Measure_GZ(I2C_HandleTypeDef* i2cHandle,double* GyroZ){
	if(i2cHandle->Instance==I2C1){
		data[0]=GYRO_ZOUT_H;
		if(HAL_I2C_Master_Transmit(&hi2c1,MPU_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MPU_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }
		int16_t GyroZRaw = ((data[0]<<8) | data[1]);
		*GyroZ = ((GyroZRaw)/(32767.0))*GyroSensitivity;
	}
}

/**
 * @brief			: Fonction de mesure du champs magnétique
 * 					  suivant X du capteur AK8963
 * @var				: Instance I2C
 * 					  Pointeur vers la variable magnétique selon X a modifier
 * @retval			: Le champs magnétique au travers du pointeur transmis
 */
void Measure_MX(I2C_HandleTypeDef* i2cHandle,double* MagnetoX){
	if(i2cHandle->Instance==I2C1){
		data[0]=AK8963_XOUT_L;
		if(HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MAGNETO_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }

		int16_t MagnetoXRaw = ((data[0]<<8) | data[1]);

		data[0]=AK8963_ASAX;
		if(HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }

		int8_t asaX = data[0];

		*MagnetoX = (MagnetoXRaw*((((asaX-128)*0.5)/128)+1)*196)/(32767.0);;
	}
}

/**
 * @brief			: Fonction de mesure du champs magnétique
 * 					  suivant Y du capteur AK8963
 * @var				: Instance I2C
 * 					  Pointeur vers la variable magnétique selon Y a modifier
 * @retval			: Le champs magnétique au travers du pointeur transmis
 */
void Measure_MY(I2C_HandleTypeDef* i2cHandle,double* MagnetoY){
	if(i2cHandle->Instance==I2C1){
		data[0]=AK8963_YOUT_L;
		if(HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MAGNETO_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }

		int16_t MagnetoYRaw = ((data[0]<<8) | data[1]);

		data[0]=AK8963_ASAY;
		if(HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }

		int8_t asaY = data[0];

		*MagnetoY = (MagnetoYRaw*((((asaY-128)*0.5)/128)+1)*196)/(32767.0);
	}
}

/**
 * @brief			: Fonction de mesure du champs magnétique
 * 					  suivant Z du capteur AK8963
 * @var				: Instance I2C
 * 					  Pointeur vers la variable magnétique selon Z a modifier
 * @retval			: Le champs magnétique au travers du pointeur transmis
 */
void Measure_MZ(I2C_HandleTypeDef* i2cHandle,double* MagnetoZ){
	if(i2cHandle->Instance==I2C1){
		data[0]=AK8963_ZOUT_L;
		if(HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MAGNETO_ADD, data, 2, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }

		int16_t MagnetoZRaw = ((data[0]<<8) | data[1]);

		data[0]=AK8963_ASAZ;
		if(HAL_I2C_Master_Transmit(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
	    	  	  Error_Handler();
			  }
		if(HAL_I2C_Master_Receive(&hi2c1,MAGNETO_ADD, data, 1, HAL_MAX_DELAY) != HAL_OK){
		    	  Error_Handler();
		      }

		int8_t asaZ = data[0];

		*MagnetoZ = (MagnetoZRaw*((((asaZ-128)*0.5)/128)+1)*196)/(32767.0);;
	}
}

void Noise_GX(I2C_HandleTypeDef* i2cHandle,double* BruitGX){

}

void Noise_GY(I2C_HandleTypeDef* i2cHandle,double* BruitGY){

}

void Noise_GZ(I2C_HandleTypeDef* i2cHandle,double* BruitGZ){

}
