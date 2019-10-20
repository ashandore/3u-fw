
#include <utl.hh>
#include <construct.hh>
#include <driver/uart.hh>
#include <driver/pin/push-pull.hh>
#include <driver/adc.hh>

class application {
    stm32g4::driver::uart<false>                m_uart;
    stm32g4::driver::pin::push_pull             m_power_switch;
    utl::construct<stm32g4::driver::adc>        m_adc;
    stm32g4::driver::adc::adc_channel_t         m_current_sense;
public:
    application();
    void start(void);
    void loop(void);
};

