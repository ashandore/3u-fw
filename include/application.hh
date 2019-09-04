
#include <drivers/uart.hh>

class application {
    drivers::stm32g4::uart<false>        m_uart;
public:
    application();
    void start(void);
    void loop(void);
};

