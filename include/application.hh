
#include <utl.hh>
#include <driver/uart.hh>
#include <driver/pin/push-pull.hh>

class application {
    stm32g4::driver::uart<false>        m_uart;
    stm32g4::driver::pin::push_pull     m_power_switch;
public:
    application();
    void start(void);
    void loop(void);
};

