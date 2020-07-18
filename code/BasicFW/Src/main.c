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
#include "prg_runner.h"
#include "errorLogger.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFF_SIZE  2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
//DMA_HandleTypeDef hdma_usart2_rx;
uint8_t rxBuffer[RX_BUFF_SIZE];
uint8_t uart_cmd = (uint8_t)_PRG_END;
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



/* User - Common Variables -------------------------------------------------- */

/* User - Program Manager Variables ----------------------------------------- */


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

/* User - Data Logger Function Prototyps ------------------------------------ */
uint8_t flash_write32( uint32_t * block_p, uint32_t data);
uint8_t flash_erase32( uint32_t * block_p );
uint8_t flash_eraseHNCS( void );

/* User - Program Manager Function Prototyps */
void sendUartMsgInt(uint32_t num, uint8_t base);
void setTimer(uint32_t value);
uint8_t getButtonState(void);
uint8_t getPrgTimer0Value(void);
uint8_t getCurPrg(void);
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
  if ( hPrg1_init() != PRG_MNG_OK )      
    { sendUartMsg("Initialization of Programm P1 Failed.\n", sizeof("Initialization of Programm P1 Failed.\n")); }                        /* Initialize Program Manager *//* Initialize Program Manager */
  if ( initErrorLog( (uint32_t*)&_hncs_eblock_start , (uint32_t*)&_hncs_eblock_end ) != PRG_MNG_OK )
    { sendUartMsg("Initialization of Error Logger Failed.\n", sizeof("Initialization of Error Logger Failed.\n")); }                    /* Initialize Program Manager */
 
  initUARTMsg();
  
  /* USER CODE END 2 */
  /* Init scheduler */
  osKernelInitialize();

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
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  sysCtrlTaskHandle = osThreadNew(StartSysCtrlTask, NULL, &sysCtrlTask_attributes);
  smplTimerTaskHandle = osThreadNew(StartSmplTimerTask, NULL, &smplTimerTask_attributes);
  if ( sysCtrlTaskHandle == NULL || smplTimerTaskHandle == NULL )
  {
    DBG_Error_Handler( MOD_MAIN_C , FNC_main , RSN_INIT_FAILURE , 0 );
  }
  /* USER CODE END RTOS_THREADS */
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
    DBG_Error_Handler(MOD_MAIN_C , FNC_SystemClock_Config , 0 , 0);
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
    DBG_Error_Handler(MOD_MAIN_C , FNC_SystemClock_Config , 1 , 0);
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
    DBG_Error_Handler( MOD_MAIN_C , FNC_MX_DMA_UART2_RX_Init , 0 , 0 );
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
void StartSysCtrlTask(void *argument)
{
  /* Start UART RX the first time in IRQ mode  */
  sendUartMsg( "Init SysCtrlTask\n" , sizeof( "Init SysCtrlTask\n" ) );
  HAL_UART_Receive_DMA( &huart2 , rxBuffer , RX_BUFF_SIZE );
  for(;;)
  {
    osDelay(250);
  }
}

void StartSmplTimerTask(void *argument)
{
  sendUartMsg( "Init SmplTimerTask\n" , sizeof( "Init SmplTimerTask\n" ) );
  for(;;)
  {
    /* Add the needet Timers here */
    if (prgMng_timer0 > 0 )
    {
      prgMng_timer0--;
    }
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

/* User - Callback Functions --------------------------------------------------------- */
void HAL_UART_RxCpltCallback(struct __UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)
  {
    if (uart_cmd == (uint8_t)_PRG_END )
    {
      sendUartMsg((char*)rxBuffer,RX_BUFF_SIZE);
      sendUartMsg("\n",sizeof("\n"));
      uart_cmd = extractUARTCmd(rxBuffer);
    }
    else
    {
      sendUartMsg("Busy\n",sizeof("Busy\n"));
    }
    memset(rxBuffer,0,RX_BUFF_SIZE);
  }
  else
  {
    /* Rx Ring Buffer overflow */
    DBG_Error_Handler( MOD_MAIN_C , FNC_HAL_UART_RxCpltCallback , RSN_BUFFER_OVERFLOW , 0 );
  }
  
}
/* user - UART Communication Function Implementations --------------------------------- */
uint8_t extractUARTCmd( uint8_t * data )
{
  uint8_t res = (uint8_t)_PRG_END;
  res = (uint8_t)atoi((char*)data);
  return res;
}
uint8_t uartCmdIsAvailable(void)
{
  if ( uart_cmd != (uint8_t)_PRG_END )
  {
    return PRG_MNG_OK;
  }
  return PRG_MNG_FAILED;
}
uint8_t getUartCmd(void)
{
  return uart_cmd;
}
void resetUartCom(void)
{
  uart_cmd = (uint8_t)_PRG_END;
}
/* user - Common Function Implementations --------------------------------------------- */
void initUARTMsg( void )
{
  char address[33] = {0};
  sendUartMsg("Bachelorarbeit von gslg\n", sizeof("Bachelorarbeit von gslg\n"));

  
  sendUartMsg("HNCS Test Block: ", sizeof("HNCS Test Block: "));
  sendUartMsg("From: ", sizeof("From: "));
  itoa((uint32_t)&_hncs_tblock_start,address,16);
  sendUartMsg(address, 33);
  sendUartMsg(" To: ", sizeof(" To: "));
  memset(address,0,33);
  itoa((uint32_t)&_hncs_tblock_end,address,16);
  sendUartMsg(address, 33);
  sendUartMsg("\n", sizeof("\n"));
  
  
  sendUartMsg("HNCS Error Block: ", sizeof("HNCS Error Block: "));
  sendUartMsg("From: ", sizeof("From: "));
  memset(address,0,33);
  itoa((uint32_t)&_hncs_eblock_start,address,16);
  sendUartMsg(address, 33);
  sendUartMsg(" To: ", sizeof(" To: "));
  memset(address,0,33);
  itoa((uint32_t)&_hncs_eblock_end,address,16);
  sendUartMsg(address, 33);
  sendUartMsg("\n", sizeof("\n"));
  
  sendUartMsg("HNCS Warning Block: ", sizeof("HNCS Warning Block: "));
  sendUartMsg("From: ", sizeof("From: "));
  memset(address,0,33);
  itoa((uint32_t)&_hncs_wblock_start,address,16);
  sendUartMsg(address, 33);
  sendUartMsg(" To: ", sizeof(" To: "));
  memset(address,0,33);
  itoa((uint32_t)&_hncs_wblock_end,address,16);
  sendUartMsg(address, 33);
  sendUartMsg("\n", sizeof("\n"));

  sendUartMsg("Initilaization completed\n",sizeof("Initilaization: completed\n"));
  sendUartMsg("\n", sizeof("\n"));
}
/* user - Flash Function Implementations ---------------------------------------------- */
uint8_t flash_write32( uint32_t * block_p, uint32_t data )
{
  uint8_t res = PRG_MNG_FAILED;

  sendUartMsg("main.c (1)\n",sizeof("main.c (1)\n"));

  HAL_FLASH_Unlock();
  if ( HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD , (uint32_t)block_p , (uint64_t)data ) == HAL_OK)
  {
    res = PRG_MNG_OK; 
  }
  HAL_FLASH_Lock();
  return res;
}
uint8_t flash_erase32( uint32_t * block_p )
{
  uint8_t res = PRG_MNG_FAILED;
  

  HAL_FLASH_Unlock();
  if ( HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD , (uint32_t)block_p , (uint32_t)0xFFFFFFFF ) == HAL_OK) 
  {  
    sendUartMsg("Successfull flash_erase32\n",sizeof("Successfull flash_erase32\n"));
    res = PRG_MNG_OK; 
  }
  HAL_FLASH_Lock();

  return res;
}
uint8_t flash_eraseHNCS( void )
{
  uint8_t res = PRG_MNG_FAILED;
  static FLASH_EraseInitTypeDef f_eitf;
  uint32_t pE;

  f_eitf.TypeErase = FLASH_TYPEERASE_PAGES;
  f_eitf.PageAddress = (uint32_t)&_hncs_tblock_start;
  f_eitf.NbPages = (uint32_t)2;

  HAL_FLASH_Unlock();
  
  HAL_FLASHEx_Erase( &f_eitf , &pE );
  if (HAL_FLASHEx_Erase( &f_eitf , &pE )== HAL_OK) 
  {  
    sendUartMsg("Flash Erase Successfull\n",sizeof("Flash Erase Successfull\n"));
    res = PRG_MNG_OK; 
  }
  HAL_FLASH_Lock();
  return res;
}
uint8_t flash_writeNext( void )
{
  static uint32_t * address = &_hncs_tblock_start;
  uint8_t res = PRG_MNG_FAILED;
  
  sendUartMsg("Test Write\n",sizeof("Test Write"));
  eLogData testLog = {
    .signature = (uint32_t)address,
    .module = (uint8_t)123,
    .function = (uint8_t)1,
    .number = (uint8_t)2,
    .value = (uint8_t)3
  } ;

  HAL_FLASH_Unlock();
  if ( HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD , (uint32_t)address , *((uint32_t*)&testLog) ) == HAL_OK ) 
  {  
    sendUartMsg("(1)\n",sizeof("(1)\n"));
    if ( HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD , (uint32_t)(address+1) , *(((uint32_t*)&testLog)+1) ) == HAL_OK ) 
    {  
      sendUartMsg("(2)\n",sizeof("(1)\n"));
      res = PRG_MNG_OK;
    }
  }
  sendUartMsg("(3)\n",sizeof("(1)\n"));
  HAL_FLASH_Lock();
  address += 2;
  return res;
}

/* user - Program Manager Function Implementations ------------------------------------ */
void setTimer(uint32_t value)
{
  prgMng_timer0 = value;
}
void sendUartMsg(char * str, uint8_t length)
{
  HAL_UART_Transmit(&huart2,(uint8_t*)str,length,100);
}
void sendUartMsgInt( uint32_t num , uint8_t base )
{
  char numStr[20] = { 0 };
  itoa(num,numStr,base);
  sendUartMsg(numStr, 20);
}
uint8_t getButtonState(void)
{
  return HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin);
}
uint8_t getPrgTimer0Value(void)
{
  return prgMng_timer0;
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
    prgMng_checkTrans( &hPrg1 );
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
