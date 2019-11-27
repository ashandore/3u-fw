#ifndef POKEY_LAYOUTS_HH_
#define POKEY_LAYOUTS_HH_

#include "macro.hh"

namespace sc = utl::hal::usb::hid::scancode;

template <typename Config_t>
struct default_layout {
    using input_t = typename Keyboard::input_t;
    using layout_t = keeb::layout<
        input_t, 1,
        typename Keyboard::template slot_t<keeb::macro::push_layout>,
        typename Keyboard::template slot_t<keeb::macro::pop_layout>
    >;

    utl::hal::usb::hid::keyboard_report& report;

    layout_t layout{
        {{6,4}, 
            {input_t::event::pressed, keeb::macro::push_layout{m_mod_layout,stack}},
            {input_t::event::released, keeb::macro::pop_layout{}}
        }
    };
};


#endif //POKEY_LAYOUTS_HH_
