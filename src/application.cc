
#include "application.hh"



using namespace utl::literals;

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    //this may need a call to   HAL_SYSTICK_IRQHandler();
}

application::application() :
    m_uart{USART2, 3000000u},
    m_application_output{m_uart},
    m_logger_setup{&m_application_output},
    m_ucpd{},
    m_power_switch{GPIOA, GPIO_PIN_1, utl::driver::pin::active_level::high, true, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM},
    m_adc{ADC1, 1000u, 3.3f},
    m_current_sense{m_adc ? &m_adc.value() : nullptr, ADC_CHANNEL_1},
    m_led_pwm_source{TIM3, 5000u},
    m_led_pwm{utl::dependent_t{}, utl::try_t{m_led_pwm_source}, hw::pwm::channel_id::CHANNEL_1}
{
    if(!m_uart) while(1);
}

void application::start(void)
{
    utl::log("application started");
    utl::log("PD: %d mA", m_ucpd.get_current_advertisement_ma());    

    m_power_switch.set_state(true);
    if(!m_adc) utl::log("ADC initialization failed: %s", m_adc.error().message().data());
}

void application::loop(void)
{
    HAL_Delay(1000);
    if(m_adc) {
        auto calculate_current = [](const uint16_t conversion) {
            return 161u*static_cast<uint32_t>(conversion)/100u;
        };

        auto conv = m_current_sense.conversion();
        if(conv) {
            auto current = calculate_current(conv.value());
            utl::log("Load current: %dmA", current);
        } else {
            utl::log("conversion failed.");
        }
    }
}
