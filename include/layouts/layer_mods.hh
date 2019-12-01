#ifndef POKEY_LAYOUTS_HH_
#define POKEY_LAYOUTS_HH_

#include "macro.hh"

namespace sc = utl::hal::usb::hid::scancode;

template <typename Config_t>
struct mod_layout {
    using input_t = keeb::input_t<Config_t>;
    using layout_t = keeb::layout<
        Config_t, 1,
        keeb::slot<Config_t,keeb::macro::push_layout>,
        keeb::slot<Config_t,keeb::macro::pop_layout>
    >;
    
    keeb::layout_stack_t<Config_t>&     stack;

    layout_t layout{
        {{6,4}, 
            {input_t::event::pressed, keeb::macro::push_layout{stack, layout}},
            {input_t::event::released, keeb::macro::pop_layout{stack}}
        }
    };
};


#endif //POKEY_LAYOUTS_HH_
