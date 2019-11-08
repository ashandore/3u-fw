
#include "application.hh"



using namespace utl::literals;

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    //this may need a call to   HAL_SYSTICK_IRQHandler();
}

static application::leds_t* s_leds = nullptr;
extern "C" void DMA1_Channel1_IRQHandler(void)
{
  s_leds->service_dma();
}

#include "usb/usb_device.h"

extern PCD_HandleTypeDef hpcd_USB_FS;
extern "C" void USB_LP_IRQHandler(void)
{
  /* USER CODE BEGIN USB_LP_IRQn 0 */

  /* USER CODE END USB_LP_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_LP_IRQn 1 */

  /* USER CODE END USB_LP_IRQn 1 */
}

application::application() :
    m_uart{USART2, 3000000u},
    m_application_output{utl::try_t{m_uart}},
    m_logger_setup{m_application_output.has_value() ? &m_application_output.value() : nullptr},
    m_ucpd{},
    m_power_switch{GPIOA, GPIO_PIN_1, utl::driver::pin::active_level::high, true, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM},
    m_adc{ADC1, 1000u, 3.3f},
    m_current_sense{utl::try_t{m_adc}, ADC_CHANNEL_1},
    m_led_dma{DMA1_Channel1, hw::dma::request::tim3_ch1},
    m_led_pwm_source{TIM3, 1250_ns},
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
    utl::log("UCPD reports a %dmA advertisement from DFP", m_ucpd.get_current_advertisement_ma());    

    m_power_switch.set_state(true);
    if(!m_adc) utl::log("ADC initialization failed: %s", m_adc.error().message().data());
    if(!m_leds) utl::log("leds failed to initialize.");
    NVIC_SetPriority(SysTick_IRQn, 1);

    HAL_Delay(500);
    MX_USB_Device_Init();
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
            if((idx + m_march_count) % 3 == 0) leds[idx] = hw::red;
            if((idx + m_march_count) % 3 == 1) leds[idx] = hw::green;
            if((idx + m_march_count) % 3 == 2) leds[idx] = hw::blue;
        }
        if(leds.write()) {
            m_march_count++;
        } else {
            utl::log("couldn't write led data");
        }
    });
}
