
#include "application.hh"

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    //this may need a call to   HAL_SYSTICK_IRQHandler();
}

application::application() :
    m_uart{USART2, 3000000}
{

}

void application::start(void)
{
    //Mostly working, but I'm getting ~2.6Mbaud instead of 3Mbaud. Probably something weird with clock divisors?
    m_uart.printf("application started");
    m_uart.printf("uart is running at %d", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_USART2));

    UART_ClockSourceTypeDef clocksource;
    UART_GETCLOCKSOURCE(&m_uart.m_handle, clocksource);
    m_uart.printf("uart clock source is %d", clocksource);
    // m_uart.printf("\n");
    // m_uart.write("U",1);
}

void application::loop(void)
{
    HAL_Delay(1000);
    m_uart.printf("HI!");
}
