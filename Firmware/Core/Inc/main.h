/* USER CODE BEGIN Header */
/**
  * In USER CODE sections, the following license applies:
  * 
  * SPDX-License-Identifier: MIT
  * 
  * Copyright (c) 2024 SparkFun Electronics
  * 
  * Otherwise, the following license applies:

  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32c0xx_ll_adc.h"
#include "stm32c0xx_ll_i2c.h"
#include "stm32c0xx_ll_rcc.h"
#include "stm32c0xx_ll_bus.h"
#include "stm32c0xx_ll_system.h"
#include "stm32c0xx_ll_exti.h"
#include "stm32c0xx_ll_cortex.h"
#include "stm32c0xx_ll_utils.h"
#include "stm32c0xx_ll_pwr.h"
#include "stm32c0xx_ll_dma.h"
#include "stm32c0xx_ll_spi.h"
#include "stm32c0xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// Inlcude standard libraries here so we don't need to include everwhere else
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

// Arduino style setup() and loop() functions
void setup();
void loop();

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VCSEL_POW_Pin LL_GPIO_PIN_15
#define VCSEL_POW_GPIO_Port GPIOC
#define MEAS_1V8_Pin LL_GPIO_PIN_0
#define MEAS_1V8_GPIO_Port GPIOA
#define LSM_INT_Pin LL_GPIO_PIN_3
#define LSM_INT_GPIO_Port GPIOA
#define LSM_INT_EXTI_IRQn EXTI2_3_IRQn
#define PAA_RST_Pin LL_GPIO_PIN_4
#define PAA_RST_GPIO_Port GPIOA
#define PAA_CS_Pin LL_GPIO_PIN_5
#define PAA_CS_GPIO_Port GPIOA
#define PAA_MOTION_Pin LL_GPIO_PIN_7
#define PAA_MOTION_GPIO_Port GPIOA
#define LSM_CS_Pin LL_GPIO_PIN_8
#define LSM_CS_GPIO_Port GPIOA
#define PAA_POW_Pin LL_GPIO_PIN_14
#define PAA_POW_GPIO_Port GPIOC
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
