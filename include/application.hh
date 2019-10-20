
#include <utl.hh>
#include <logger.hh>
#include <construct.hh>
#include <driver/uart.hh>
#include <driver/pin/push-pull.hh>
#include <driver/adc.hh>

class application {
    using uart_t = stm32g4::driver::uart<false>;
    using onboard_adc_t = stm32g4::driver::adc;

    utl::construct<uart_t>                      m_uart;
    utl::logger::output<uart_t>                 m_application_output;
    utl::logger::push_output                    m_logger_setup;
    stm32g4::driver::pin::push_pull             m_power_switch;
    utl::construct<onboard_adc_t>               m_adc;
    stm32g4::driver::adc::adc_channel_t         m_current_sense;
public:
    application();
    void start(void);
    void loop(void);
};

