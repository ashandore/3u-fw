
#include <utl.hh>
#include <logger.hh>
#include <construct.hh>
#include <driver/uart.hh>
#include <driver/pin/push-pull.hh>
#include <driver/adc.hh>
#include <driver/ucpd.hh>
#include <driver/pwm.hh>

namespace hw = stm32g4::driver;

class application {
    using uart_t = hw::uart<false>;

    utl::construct<uart_t>                      m_uart;
    utl::logger::output<uart_t>                 m_application_output;
    utl::logger::push_output                    m_logger_setup;
    hw::ucpd                                    m_ucpd;
    hw::pin::push_pull                          m_power_switch;
    utl::construct<hw::adc>                     m_adc;
    hw::adc::adc_channel_t                      m_current_sense;
    utl::construct<hw::pwm::source>             m_led_pwm_source;
    utl::construct<hw::pwm::channel_t>          m_led_pwm;
public:
    application();
    void start(void);
    void loop(void);
};

