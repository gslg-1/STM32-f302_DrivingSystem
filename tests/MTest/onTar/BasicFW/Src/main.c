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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uTestFW.h"
#include "example1.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum uartCmdRespond_e uartCmdRespond;
typedef enum uartCmdRespond_e uartCmdRespond;

enum uartRxState {
  UART_PACKAGE_AV,
  UART_CMD_AV,
  UART_FREE
} ;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFF_SIZE  2

#define RESP_SIZE 15
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
//DMA_HandleTypeDef hdma_usart2_rx;


/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128
};

/* USER CODE BEGIN PV */
/* User - Task Variables ---------------------------------------------------- */

osThreadId_t sysCtrlTaskHandle;
const osThreadAttr_t sysCtrlTask_attributes = {
  .name = "sysCtrlTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 128
};
osThreadId_t smplTimerTaskHandle;
const osThreadAttr_t smplTimerTask_attributes = {
  .name = "smplTimerTask",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 32
};
/* User - Linker Symboles ----------------------------------------------------*/
/* Flash - HNCS Error Block*/
extern uint32_t _hncs_wblock_start, _hncs_wblock_end;
/* Flash - HNCS Warning Block*/
extern uint32_t _hncs_eblock_start, _hncs_eblock_end;
/* Flash - HNCS Error Block*/
extern uint32_t _hncs_tblock_start, _hncs_tblock_end;


/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DMA1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */
/* User - MX Function Prototyps ------------------------------------------- */
/* User - Task Function Prototyps ------------------------------------------- */
void StartSysCtrlTask(void *argument);
void StartSmplTimerTask(void *args);

/* User - Common Function Prototyps ----------------------------------------- */
void initUARTMsg( void );

/* User - Callback Function Prototyps --------------------------------------- */
void HAL_UART_RxCpltCallback(struct __UART_HandleTypeDef *huart);   
/* User - UART Communication Function Prototyps ----------------------------- */
uint8_t extractUARTCmd( uint8_t * data );
uint8_t uartCmdIsAvailable(void);
uint8_t getUartCmd(void);
void resetUartCom(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
#ifndef __UTEST
  /* USER CODE END 1 */
  
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* User - Programm Manager initialization ---------------------------------- */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */  
  
  /* USER CODE END 2 */
  /* Init scheduler */


  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
  /* Start scheduler */
 
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  sendUartMsg("Start on target Unit-Test\n\n",sizeof("Start on target Unit-Test\n\n"));
  
  //Testgruppe 1:
  sendUartMsg("Testgruppe 1:\n\n",sizeof("Testgruppe 1:\n\n"));

  assertionEqual_uint8("Test getGreatest default", sizeof("Test getGreatest default") , 33 , getGreatest(12,33));
  assertionEqual_uint8("Test getGreatest upper boarder", sizeof("Test getGreatest upper boarder") , 255 , getGreatest(255,33));
  assertionEqual_uint8("Test getGreatest lower boarder", sizeof("Test getGreatest lower boarder") , 1 , getGreatest(1,0));
  sendUartMsg("\n\n",sizeof("\n\n"));
  
  //Testgruppe 2:
  sendUartMsg("Testgruppe 2:\n\n",sizeof("Testgruppe 1:\n\n"));

  assertionEqual_uint8("Test getLeast default", sizeof("Test getLeast default") , 12 , getLeast(12,33));
  assertionEqual_uint8("Test getLeast upper boarder", sizeof("Test getLeast upper boarder") , 33 , getLeast(255,33));
  assertionEqual_uint8("Test getLeast lower boarder", sizeof("Test getLeast lower boarder") , 0 , getLeast(1,0));
  sendUartMsg("\n\n",sizeof("\n\n"));

  sendUartMsg("Finished on target Unit-Test\n",sizeof("Finished on target Unit-Test\n"));
  sendUartMsg("<Done>\n",sizeof("<Done>\n"));   //Command for Host Sys
  while (1)
  {
    /* USER CODE END WHILE */
    HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    HAL_Delay(500);
    /* USER CODE BEGIN 3 */
  }

#else
  /* Do some crazy Unit Testing*/

#endif
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  huart2.Init.BaudRate = 38400;
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
  
  /* todo: Init DMA1_TX*/

  /* todo: Init Callback*/
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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}


/* Get started every x ms or, after some especial initilaization via Interrupt. */

void StartSmplTimerTask(void *argument)
{
  sendUartMsg( "Init SmplTimerTask\n" , sizeof( "Init SmplTimerTask\n" ) );
  for(;;)
  {
    /* Add the needet Timers here */
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);         
    osDelay(125);
  }
}

static void MX_DMA1_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  
  HAL_NVIC_SetPriority( DMA1_Channel6_IRQn , 14 , 0 );
  HAL_NVIC_EnableIRQ( DMA1_Channel6_IRQn );
}


/* user - Program Manager Function Implementations ------------------------------------ */
void sendUartMsg(char * str, uint8_t length)
{
  HAL_UART_Transmit(&huart2,(uint8_t*)str,length,100);
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  sendUartMsg( "Init DefaultTask\n" , sizeof( "Init DefaultTask\n" ) );
  /* Infinite loop */  
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler( void )
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,GPIO_PIN_SET);
  while(1)
  {
    
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
