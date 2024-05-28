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
  * @file    stm32c0xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32c0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// Need access to variables in these files so we can use them in interrupts
#include "registers.h"
#include "time.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

// Flag to know if the LSM6DSO has new data ready
volatile bool lsmDrdy = false;

// Flag to know whether the next byte received is the register index or data
volatile bool nextByteIsRegisterIndex = true;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  // SysTick_Handler() is called once every millisecond, so increment the
  // millisecond counter
  millisCounter++;

  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32C0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32c0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 2 and line 3 interrupts.
  */
void EXTI2_3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_3_IRQn 0 */

  /* USER CODE END EXTI2_3_IRQn 0 */
  if (LL_EXTI_IsActiveRisingFlag_0_31(LL_EXTI_LINE_3) != RESET)
  {
    LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_LINE_3);
    /* USER CODE BEGIN LL_EXTI_LINE_3_RISING */

    // This is triggered by the LSM6DSO interrupt pin, which is configured with
    // the Data Ready condition, so set flag to indicate new data is available
    lsmDrdy = true;

    /* USER CODE END LL_EXTI_LINE_3_RISING */
  }
  /* USER CODE BEGIN EXTI2_3_IRQn 1 */

  /* USER CODE END EXTI2_3_IRQn 1 */
}

/**
  * @brief This function handles I2C1 interrupt (combined with EXTI 23).
  */
void I2C1_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_IRQn 0 */

  // Check if our address was matched. This will only occur after the first byte
  // of a transmission was sent by the host, and if the host sent our address
  if(LL_I2C_IsActiveFlag_ADDR(I2C1))
	{
    // Clear the ADDR flag
		LL_I2C_ClearFlag_ADDR(I2C1);

		// Check the read/write bit to see if the host wants to read or write
    if(LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_READ)
    {
      // Host wants to read data from us, enable TX interrupt
      LL_I2C_EnableIT_TX(I2C1);
    }
    else
    {
      // Host wants to send data to us, enable RX interrupt
      LL_I2C_EnableIT_RX(I2C1);
      
      // Also set flag to know that the next byte will be the register pointer
      nextByteIsRegisterIndex = true;
    }
  }

  // Check if we're ready to transmit the next byte
	if(LL_I2C_IsActiveFlag_TXIS(I2C1) && LL_I2C_IsEnabledIT_TX(I2C1))
	{
    // Check which register we're currently sending
    if(registerIndex >= sizeof(registerBufferMain))
    {
      // We've reached the end of the register buffer, just send a dummy byte
      LL_I2C_TransmitData8(I2C1, 0);
    }
    else
    {
      // We're not at the end yet, so send the next byte of the register buffer
      LL_I2C_TransmitData8(I2C1, registerBufferMain[registerIndex]);

      // Increment the register index for the next byte
      registerIndex++;
    }
	}

  // Check if there's data in the receive buffer
	if(LL_I2C_IsActiveFlag_RXNE(I2C1))
	{
    // Receive buffer not empty, read next byte
		uint8_t data = LL_I2C_ReceiveData8(I2C1);
    
    // Check if this is the first byte sent to us
    if(nextByteIsRegisterIndex)
    {
      // This is the first byte, so it's the register index
      registerIndex = data;

      // Reset the flag
      nextByteIsRegisterIndex = false;
    }
    else
    {
      // This is not the first byte, so it's data to be written to a register.
      // Check if we've reached the end of the register buffer
      if(registerIndex < sizeof(registerBufferMain))
      {
        // We're not at the end yet, so read the next byte into the register
        // buffer and update shadow flags
        registerShadowHost[registerIndex] = data;
        registerShadowHostWritten[registerIndex] = true;
        shadowHostWritten = true;

        // Increment the register index for the next byte
        registerIndex++;
      }
    }
	}

  // Check if the host sent a stop bit or NACKed our last byte
	if(LL_I2C_IsActiveFlag_STOP(I2C1) || LL_I2C_IsActiveFlag_NACK(I2C1))
	{
    // Clear the stop and NACK flags
		LL_I2C_ClearFlag_STOP(I2C1);
		LL_I2C_ClearFlag_NACK(I2C1);

    // Check if we were transmitting
    if(LL_I2C_IsEnabledIT_TX(I2C1))
    {
      // Disable TX interrupt so this interrupt handler stops firing
      LL_I2C_DisableIT_TX(I2C1);

      // The STOP or NACK interrupts fire after the TX interrupt, meaning we
      // will have written one extra byte to the TX buffer. We can fix this by
      // clearing the TXE flag to flush the TX buffer, and decrementing the
      // register index to get it back to the right spot
      LL_I2C_ClearFlag_TXE(I2C1);
      registerIndex--;
    }
    
    // Check if we were receiving
    if(LL_I2C_IsEnabledIT_RX(I2C1))
    {
      // Disable RX interrupt
      LL_I2C_DisableIT_RX(I2C1);
    }
	}

  /* USER CODE END I2C1_IRQn 0 */

  /* USER CODE BEGIN I2C1_IRQn 1 */

  /* USER CODE END I2C1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
