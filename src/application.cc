
#include "application.hh"



using namespace utl::literals;

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    //this may need a call to   HAL_SYSTICK_IRQHandler();
}

static application::leds_t* s_leds = nullptr;
static application::spi_t::rx_dma_channel_t* s_spi_rx_dma = nullptr;
static application::spi_t::tx_dma_channel_t* s_spi_tx_dma = nullptr;
static application::spi_t* s_spi = nullptr;


extern "C" void DMA1_Channel1_IRQHandler(void)
{
    s_leds->service_dma();
}

extern "C" void DMA1_Channel2_IRQHandler(void)
{
    s_spi_rx_dma->service();
}

extern "C" void DMA1_Channel3_IRQHandler(void)
{
    s_spi_tx_dma->service();
}

extern "C" void SPI2_IRQHandler(void)
{
    s_spi->service();
}

#include "usb/usb_device.h"
#include "usbd_hid.h"

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
    m_leds{utl::try_t{m_led_pwm_source}, utl::try_t{m_led_pwm}, utl::try_t{m_led_dma}},
    m_spi_rx_dma{DMA1_Channel2, hw::dma::request::spi2_rx},
    m_spi_tx_dma{DMA1_Channel3, hw::dma::request::spi2_tx},
    m_spi{SPI2, utl::imprecise{300_KHz, 500_KHz}, hw::spi::direction::both, 
        hw::spi::polarity::POL1, hw::spi::phase::PHA0,
        hw::spi::data_size::bits_8, false, false, false, 
        utl::try_t{m_spi_rx_dma}, utl::try_t{m_spi_tx_dma}}
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
    if(m_spi_rx_dma) s_spi_rx_dma = &m_spi_rx_dma.value();
    if(m_spi_tx_dma) s_spi_tx_dma = &m_spi_tx_dma.value();  
    if(m_spi) s_spi = &m_spi.value();  
}

extern USBD_HandleTypeDef hUsbDeviceFS;

void application::start(void)
{
    utl::log("application started");
    utl::log("UCPD reports a %dmA advertisement from DFP", m_ucpd.get_current_advertisement_ma());    

    m_power_switch.set_state(true);
    if(!m_adc) utl::log("ADC initialization failed: %s", m_adc.error().message().data());
    if(!m_leds) utl::log("leds failed to initialize.");
    NVIC_SetPriority(SysTick_IRQn, 1);

    HAL_Delay(100);
    MX_USB_Device_Init();



    // HAL_Delay(100);

    if ((reinterpret_cast<USBD_HandleTypeDef*>(hpcd_USB_FS.pData)->dev_remote_wakeup == 1) &&
        (reinterpret_cast<USBD_HandleTypeDef*>(hpcd_USB_FS.pData)->dev_state ==
        USBD_STATE_SUSPENDED))
    {
        utl::log("remote wakeup appears to be enabled.");
    }
    else if (reinterpret_cast<USBD_HandleTypeDef*>(hpcd_USB_FS.pData)->dev_state ==
        USBD_STATE_CONFIGURED)
    {        
        uint8_t HID_Buffer[4] = {0,0,0,0};
        USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 4);
        utl::log("sent HID report");
    }
    else
    {
        utl::log("usb is neither suspended nor configured.");
    }


    utl::log("USB device state is %d", reinterpret_cast<USBD_HandleTypeDef*>(hpcd_USB_FS.pData)->dev_state);

    if(!m_spi) utl::log("spi initialization failed!");    
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
            // utl::log("Load current: %dmA", current);
        });
    });

    m_spi.visit([&](auto& spi) {
        const uint8_t matrix_columns = 19;
        uint8_t send[matrix_columns] = {};
        uint8_t recv[matrix_columns];

        auto res = spi.transact(send, recv, matrix_columns);
        if(res) {
            spi.wait();
            utl::log("got columns:");
            for(uint8_t i=0; i<matrix_columns; i++) {
                utl::log("\t%x", recv[i]);
            }
        } else {
            utl::log("spi transaction failed with %s", res.error().message().data());
        }
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
