#ifndef POKEY_LAYOUTS_HH_
#define POKEY_LAYOUTS_HH_

#include "macro.hh"

namespace sc = utl::hal::usb::hid::scancode;

template <typename Config_t>
struct default_layout {
    using input_t = keeb::input_t<Config_t>;
    using layout_t = keeb::layout<
        Config_t, 1,
        keeb::slot<Config_t,keeb::macro::push_layout>,
        keeb::slot<Config_t,keeb::macro::pop_layout>
    >;
    

    //for a stack, I need a variant. then I have a stack of variant layout pointers.
    //and I'll have to visit it in order to get the active layout...
    //which means the update will need to visit it. or - the update expects
    //a visitable collection of layout pointers. great.

    //NEXT: implement a variant.

    layout_t layout{
        {{6,4}, 
            {input_t::event::pressed, keeb::macro::push_layout{m_mod_layout,stack}},
            {input_t::event::released, keeb::macro::pop_layout{}}
        }
    };
};


#endif //POKEY_LAYOUTS_HH_
