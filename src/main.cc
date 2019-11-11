
#include "board.h"
#include "string.h"

extern "C" int __aeabi_atexit( 
    void *object, 
    void (*destructor)(void *), 
    void *dso_handle) 
{ 
    static_cast<void>(object); 
    static_cast<void>(destructor); 
    static_cast<void>(dso_handle); 
    return 0; 
}


#include "application.hh"
#include "utl/platform/board.hh"

[[noreturn]]
extern "C" int main(void)
{
	SystemCoreClockUpdate();

	// Board::getInstance();
	// application_t::getInstance().start();
    board b{};
    application app{};
    app.start();
	
	while(1) {
		app.loop();
	}
}
