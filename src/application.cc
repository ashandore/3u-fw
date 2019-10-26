
#include "application.hh"



using namespace utl::literals;

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    //this may need a call to   HAL_SYSTICK_IRQHandler();
}

extern "C" void TIM3_IRQHandler(void) {

    // __HAL_TIM_CLEAR_IT(&m_handle, TIM_IT_UPDATE);
}

application::application() :
    m_uart{USART2, 3000000u},
    m_application_output{utl::try_t{m_uart}},
    m_logger_setup{m_application_output.has_value() ? &m_application_output.value() : nullptr},
    m_ucpd{},
    m_power_switch{GPIOA, GPIO_PIN_1, utl::driver::pin::active_level::high, true, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM},
    m_adc{ADC1, 1000u, 3.3f},
    m_current_sense{utl::try_t{m_adc}, ADC_CHANNEL_1},
    m_led_pwm_source{TIM3, 5000u},
    m_led_pwm{utl::try_t{m_led_pwm_source}, hw::pwm::channel_id::CHANNEL_1}
{

    //dimensions along which construct/try could have policies:
    // - whether to try unboxing by default or not (need to have a forward equivalent to try...)
    // - what to do when unboxing fails - where should the error come from?
    //the ability to specify a fallback value would be useful.
    //  it'sagraph
    //the ability to specify an error code in case of unboxing failure would be useful.
    //what if it was actually a dependency graph?
    //make a try function.

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

    m_current_sense.visit([&](auto& sense) {
        auto calculate_current = [](const uint16_t conversion) {
            return 161u*static_cast<uint32_t>(conversion)/100u;
        };

        sense.conversion().visit([&](uint16_t& conv) {
            auto current = calculate_current(conv);
            utl::log("Load current: %dmA", current);
        });
    });
}
