/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MIN_PULSE 1000      // microseconds
#define MAX_PULSE 2000      // microseconds
#define MID_PULSE 1500      // microseconds

#define STAY_LOW_TIME 5000  // milliseconds
#define RAMP_TIME 15000     // milliseconds
#define EN_PIN     GPIO_PIN_2
#define STEP_PIN   GPIO_PIN_1
#define DIR_PIN    GPIO_PIN_0
#define EN_PORT    GPIOA
#define STEP_PORT  GPIOA
#define DIR_PORT   GPIOA

#define UP_LIMIT_PIN      GPIO_PIN_10   // PB10
#define DOWN_LIMIT_PIN    GPIO_PIN_12   // PB12
#define LIMIT_PORT        GPIOB
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
volatile uint8_t up_limit_reached = 0;
volatile uint8_t down_limit_reached = 0;
int right = 0;
int left = 0;
int base_pulse = 1490;
int right_min = 1300;
int left_max  = 1600;

float progress;

/* “reported�? flags so we send LIMIT:UP only once until state changes */
volatile uint8_t up_limit_reported = 0;
volatile uint8_t down_limit_reported = 0;


/* debounce timers */
uint32_t startTime = 0;
uint32_t speed = 0;
int fw_bw = 1;
int stop = 1;
int direction = 1;
unsigned long lastDebounceUp = 0;
unsigned long lastDebounceDown = 0;
const unsigned long debounceDelay = 50;
uint8_t light = 0;
uint8_t fw_mot_start = 0;
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//void check_limit_switches(void) {
//    static uint8_t last_up_state = 0;
//    static uint8_t last_down_state = 0;
//
//    uint8_t up_state = HAL_GPIO_ReadPin(LIMIT_PORT, UP_LIMIT_PIN);
//    uint8_t down_state = HAL_GPIO_ReadPin(LIMIT_PORT, DOWN_LIMIT_PIN);
//
//    if (up_state && !last_up_state) {
//        up_limit_reached = 1;
//        CDC_Transmit_FS((uint8_t*)"LIMIT:UP\n", strlen("LIMIT:UP\n"));
//    }
//    else if (!up_state && last_up_state) {
//        up_limit_reached = 0;
//        CDC_Transmit_FS((uint8_t*)"LIMIT:CLEAR\n", strlen("LIMIT:CLEAR\n"));
//    }
//
//    if (down_state && !last_down_state) {
//        down_limit_reached = 1;
//        CDC_Transmit_FS((uint8_t*)"LIMIT:DOWN\n", strlen("LIMIT:DOWN\n"));
//    }
//    else if (!down_state && last_down_state) {
//        down_limit_reached = 0;
//        CDC_Transmit_FS((uint8_t*)"LIMIT:CLEAR\n", strlen("LIMIT:CLEAR\n"));
//    }
//
//    last_up_state = up_state;
//    last_down_state = down_state;
//}

void delay_us(uint32_t us)
{
  uint32_t start = DWT->CYCCNT;
  uint32_t ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000);
  while ((DWT->CYCCNT - start) < ticks);
}
void singleStep(int dir, int speed_us)
{
  HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, dir ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_SET);
  delay_us(speed_us);
  HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_RESET);
  delay_us(speed_us);
}

/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t current = HAL_GetTick();

    if (GPIO_Pin == UP_LIMIT_PIN)
    {
        if ((current - lastDebounceUp) > debounceDelay)
        {
            lastDebounceUp = current;
            up_limit_reached = 1;         // tell main loop we hit top limit
            //up_limit_reported = 0;// allow main to report once
            down_limit_reached = 0;
        }
    }
    else if (GPIO_Pin == DOWN_LIMIT_PIN)
    {
        if ((current - lastDebounceDown) > debounceDelay)
        {
            lastDebounceDown = current;
            down_limit_reached = 1;
            //down_limit_reported = 0;
            up_limit_reached = 0;
        }
    }
}
int set_motor_speed_fw(uint32_t speed_percent) {
    if (speed_percent > 100) speed_percent = 100;

    // Map 0–100 → 1000–1500 µs
    uint32_t pulse = MID_PULSE + ((MAX_PULSE - MID_PULSE) * speed_percent) / 100;

    return pulse;

}
int set_motor_speed_bw(uint32_t speed_percent) {
    if (speed_percent > 100) speed_percent = 100;

    // Map 0–100 → 1000–1500 µs
    uint32_t pulse = MID_PULSE - ((MAX_PULSE - MID_PULSE) * speed_percent) / 100;

    return pulse;

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
  MX_USB_DEVICE_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_SET);  // Disable
  HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_RESET);

      //HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_RESET); // Enable driver

      // Enable DWT Cycle Counter for microsecond delay
      CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
      DWT->CYCCNT = 0;
      DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

      HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
      HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
        startTime = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//	  check_limit_switches();
	  if (up_limit_reached)
	      {
	          // disable driver (active low enable in your code)
	          HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_SET); // disable motor driver

//	          if (!up_limit_reported)
//	          {
//	              // send event to host once
//	              CDC_Transmit_FS((uint8_t*)"LIMIT:UP\n", (uint16_t)strlen("LIMIT:UP\n"));
//	              up_limit_reported = 1;
//	          }

	          // do not step
	          HAL_Delay(1); // small delay to keep loop friendly
	      }
	      else if (down_limit_reached)
	      {
	          HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_SET); // disable driver

//	          if (!down_limit_reported)
//	          {
//	              CDC_Transmit_FS((uint8_t*)"LIMIT:DOWN\n", (uint16_t)strlen("LIMIT:DOWN\n"));
//	              down_limit_reported = 1;
//	          }
	          HAL_Delay(1);
	      }
	      else
	      {
	          // normal stepping: ensure driver enabled (active low)

	          if(stop) HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_SET); // disable motor driver
	          else HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_RESET);
	          // do one step (existing function)
	          singleStep(direction, 1000);
	      }

	  uint32_t elapsed = HAL_GetTick() - startTime;
	  	  	      uint32_t pulse_width;
	  	  	      uint32_t pulse;


	  	  	      if (elapsed < STAY_LOW_TIME)
	  	  	      {
	  	  	        pulse_width = MID_PULSE;
	  	  	    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 1490);
	  	  	      }
	  	  	     else
	  	  	      {
	  	  	    	 if (fw_bw) pulse_width = set_motor_speed_fw(speed);
	  	  	    	 else pulse_width = set_motor_speed_bw(speed);
	  	  	    	if (right && !left)
	  	  	    	{
	  	  	    	    progress = (float)(elapsed - STAY_LOW_TIME) / RAMP_TIME;
	  	  	    	    if (progress < 0) progress = 0;
	  	  	    	    if (progress > 1) progress = 1;

	  	  	    	    pulse = base_pulse - progress * (base_pulse - right_min);

	  	  	    	    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pulse);
	  	  	    	}

	  	  	    	// ----------- LEFT ----------------
	  	  	    	else if (left && !right)
	  	  	    	{
	  	  	    	    progress = (float)(elapsed - STAY_LOW_TIME) / RAMP_TIME;
	  	  	    	    if (progress < 0) progress = 0;
	  	  	    	    if (progress > 1) progress = 1;

	  	  	    	    pulse = base_pulse + progress * (left_max - base_pulse);

	  	  	    	    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pulse);
	  	  	    	}

	  	  	    	// ----------- NEUTRAL -------------
	  	  	    	else
	  	  	    	{
	  	  	    	    pulse = base_pulse;
	  	  	    	    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pulse);
	  	  	    	}
	  	  	      }

	  	  	      // Set pulse width (duty cycle)
	  	  	      if(fw_mot_start) __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse_width);
	  	  	      else __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, MID_PULSE);
	  	  	      if(light) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 5000);
	  	  	      else __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 95;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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
  htim3.Init.Prescaler = 95;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 19999;
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
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
