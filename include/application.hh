
#include <utl.hh>
#include <logger.hh>
#include <construct.hh>
#include <driver/uart.hh>
#include <driver/pin/push-pull.hh>
#include <driver/adc.hh>
#include <driver/ucpd.hh>
#include <driver/pwm.hh>
#include <driver/dma.hh>
#include <driver/ws2812.hh>

namespace hw = stm32g4::driver;

class application {
public:
    using uart_t = hw::uart<false>;
    using log_out_t = utl::logger::output<uart_t>;
    using leds_t = ws2812::pwm<85, hw::pwm::source, hw::dma>;
private:
    utl::construct<uart_t>                      m_uart;
    utl::construct<log_out_t>                   m_application_output;
    utl::logger::push_output                    m_logger_setup;
    hw::ucpd                                    m_ucpd;
    hw::pin::push_pull                          m_power_switch;
    utl::construct<hw::adc>                     m_adc;
    utl::construct<hw::adc::adc_channel_t>      m_current_sense;
    utl::construct<hw::dma>                     m_led_dma;
    utl::construct<hw::pwm::source>             m_led_pwm_source; 
    utl::construct<hw::pwm::dma_channel_t>      m_led_pwm;
    utl::construct<leds_t>                      m_leds;
    uint32_t                                    m_march_count = 0;
public:
    application();
    void start(void);
    void loop(void);
};

