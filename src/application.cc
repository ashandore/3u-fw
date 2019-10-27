
#include "application.hh"



using namespace utl::literals;

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    //this may need a call to   HAL_SYSTICK_IRQHandler();
}

static application::leds_t* s_leds = nullptr;

// extern "C" void TIM3_IRQHandler(void) {
//     s_leds->update_pwm();   
// }
// extern "C" void DMA1_Channel1_IRQHandler(void)
// {
//   s_leds->service_dma();
// }


application::application() :
    m_uart{USART2, 3000000u},
    m_application_output{utl::try_t{m_uart}},
    m_logger_setup{m_application_output.has_value() ? &m_application_output.value() : nullptr},
    m_ucpd{},
    m_power_switch{GPIOA, GPIO_PIN_1, utl::driver::pin::active_level::high, true, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM},
    m_adc{ADC1, 1000u, 3.3f},
    m_current_sense{utl::try_t{m_adc}, ADC_CHANNEL_1},
    m_led_dma{DMA1_Channel1, DMA_REQUEST_TIM3_CH1, DMA_MEMORY_TO_PERIPH, DMA_PINC_DISABLE, DMA_MINC_ENABLE,
        DMA_PDATAALIGN_WORD, DMA_MDATAALIGN_WORD, DMA_CIRCULAR, DMA_PRIORITY_VERY_HIGH},
    m_led_pwm_source{TIM3, 1250u},
    m_led_pwm{utl::try_t{m_led_pwm_source}, hw::pwm::channel_id::CHANNEL_1, hw::pwm::polarity::ACTIVE_HIGH},
    m_leds{utl::try_t{m_led_pwm_source}, utl::try_t{m_led_pwm}, utl::try_t{m_led_dma}}
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
    if(m_leds) s_leds = &m_leds.value();
}

void application::start(void)
{
    utl::log("application started");
    utl::log("PD: %d mA", m_ucpd.get_current_advertisement_ma());    

    m_power_switch.set_state(true);
    if(!m_adc) utl::log("ADC initialization failed: %s", m_adc.error().message().data());

    if(!m_leds) utl::log("leds failed to initialize.");
    

    // m_led_pwm_source.visit([] (auto& source) {
    //     source.set_period_ns(1250);
    //     if(source.start()) {
    //         utl::log("started pwm source");
    //     } else {
    //         utl::log("couldn't start pwm source");
    //     }
    // });

    // m_led_pwm.visit([] (auto& pwm) {
    //     pwm.set_width_ns(500);
    //     if(pwm.start()) {
    //         utl::log("started pwm");
    //         utl::log("pulse width is %duS", pwm.width_ns());
    //     } else {
    //         utl::log("couldn't start pwm");
    //     }
    // });
    // NVIC_SetPriority(TIM3_IRQn, 0);
    NVIC_SetPriority(SysTick_IRQn, 1);

    // m_leds.visit([&] (auto& leds) {
    //     for(uint32_t idx=0; idx < leds.count(); idx++) {
    //         if((idx + m_march_count) % 3 == 0) leds[idx] = ws2812::red;
    //         if((idx + m_march_count) % 3 == 1) leds[idx] = ws2812::green;
    //         if((idx + m_march_count) % 3 == 2) leds[idx] = ws2812::blue;
    //     }
    //     if(leds.write()) {
    //         // m_march_count++;
    //     } else {
    //         utl::log("couldn't write led data");
    //     }
    // });
}

void application::loop(void)
{
    HAL_Delay(50);

    m_current_sense.visit([&](auto& sense) {
        auto calculate_current = [](const uint16_t conversion) {
            return 161u*static_cast<uint32_t>(conversion)/100u;
        };

        sense.conversion().visit([&](uint16_t& conv) {
            auto current = calculate_current(conv);
            utl::maybe_unused(current);
            utl::log("Load current: %dmA", current);
        });
    });

    m_leds.visit([&] (auto& leds) {
        for(uint32_t idx=0; idx < leds.count(); idx++) {
            if((idx + m_march_count) % 3 == 0) leds[idx] = ws2812::red;
            if((idx + m_march_count) % 3 == 1) leds[idx] = ws2812::green;
            if((idx + m_march_count) % 3 == 2) leds[idx] = ws2812::blue;
        }
        if(leds.write()) {
            m_march_count++;
        } else {
            utl::log("couldn't write led data");
        }
    });
}
