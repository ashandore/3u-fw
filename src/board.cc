#include "board.h"
#include <stm32g4xx_hal_uart.h>
#include <stm32g4xx_hal_gpio.h>
#include <stdarg.h>
#include <platform/board.hh>
// #include "stm32g4xx_hal_rcc.h"

/* Set up and initialize all required blocks and functions related to the
   board hardware */
extern "C" void Board_Init(void)
{
	
}

extern "C" void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    if(huart->Instance == USART2)
    {  
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        __HAL_RCC_GPIOA_CLK_ENABLE();


        /* Enable USARTx clock */
        __HAL_RCC_USART2_CLK_ENABLE(); 

        /*##-2- Configure peripheral GPIO ##########################################*/  
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = GPIO_PIN_2;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

extern "C" void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    GPIO_InitTypeDef          GPIO_InitStruct;

    if(hadc->Instance == ADC1) {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/ 
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

extern "C" void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

    if(tim_baseHandle->Instance==TIM3)
    {
        /* USER CODE BEGIN TIM3_MspInit 0 */

        /* USER CODE END TIM3_MspInit 0 */
        /* TIM3 clock enable */
        __HAL_RCC_TIM3_CLK_ENABLE();

        /* TIM3 interrupt Init */
        HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
        /* USER CODE BEGIN TIM3_MspInit 1 */

        /* USER CODE END TIM3_MspInit 1 */
    }
}

extern "C" void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct{};
    if(timHandle->Instance==TIM3)
    {
        /* USER CODE BEGIN TIM3_MspPostInit 0 */

        /* USER CODE END TIM3_MspPostInit 0 */

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM3 GPIO Configuration    
        PA1     ------> TIM3_CH2 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3 ;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM3_MspPostInit 1 */

        /* USER CODE END TIM3_MspPostInit 1 */
    }
}

extern "C" void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 64;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 2;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while(1);
    }
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        while(1);
    }
    /**Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        while(1);
    }
    /**Configure the Systick interrupt time
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    /**Configure the Systick
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

board::board() {
    SystemClock_Config();
    SystemCoreClockUpdate();
    // HAL_EnableCompensationCell();

    // HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    // HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
    //Initialize ST's HAL. This will also initialize the system tick.
    HAL_Init();

    SystemCoreClockUpdate();

    __enable_irq();
}

// static UART_HandleTypeDef _fault_uart_handle;

// void fault_output_init(void);
// void fault_output_write(char *string, uint32_t length);
// void fault_output_printf(const char *format, ...);

// void fault_output_init(void) {
//     _fault_uart_handle.Instance        = USART6;

// 	_fault_uart_handle.Init.BaudRate   = 1000000;
// 	_fault_uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
// 	_fault_uart_handle.Init.StopBits   = UART_STOPBITS_1;
// 	_fault_uart_handle.Init.Parity     = UART_PARITY_NONE;
// 	_fault_uart_handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
// 	_fault_uart_handle.Init.Mode       = UART_MODE_TX_RX;
// 	if(HAL_UART_DeInit(&_fault_uart_handle) != HAL_OK) return;
// 	if(HAL_UART_Init(&_fault_uart_handle) != HAL_OK) return;
// }

// void fault_output_write(char *string, uint32_t length) {
//     HAL_UART_Transmit(&_fault_uart_handle, (uint8_t*)string, (uint16_t)length, 5000);
// }

// __attribute__((format(printf, 1, 2)))
// void fault_output_printf(const char *format, ...) {
//     static char uart_buffer[512];
// 	int length;

// 	va_list arguments;
//     va_start(arguments, format);
//     length = vsniprintf(uart_buffer, 512, format, arguments);
//     va_end(arguments);

//     fault_output_write(uart_buffer, (uint32_t)length);
//     fault_output_write("\r\n", 2);
// }
