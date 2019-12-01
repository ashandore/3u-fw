#ifndef POKEY_LAYOUTS_HH_
#define POKEY_LAYOUTS_HH_

#include "macro.hh"

namespace sc = utl::hal::usb::hid::scancode;

template <typename Config_t>
using default_layout_t = keeb::layout<
        Config_t,
        keeb::get_keycount<Config_t>(),
        keeb::slot<Config_t,keeb::macro::set_scancode>,
        keeb::slot<Config_t,keeb::macro::unset_scancode>>;

template <typename Config_t>
struct default_layout : default_layout_t<Config_t>
{
    using event_t = keeb::event_t<Config_t>;

    default_layout(utl::hal::usb::hid::keyboard_report& report) 
    : default_layout_t<Config_t>{ 
        {{0,0},  
            {event_t::pressed, keeb::macro::set_scancode{sc::grave, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::grave, report}}
        },
        {{0,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::tab, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::tab, report}}
        },     
        {{0,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lctrl, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lctrl, report}}
        },   
        {{0,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lshift, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lshift, report}}
        },  
        {{0,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lctrl, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lctrl, report}}
        },
        {{1,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num1, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num1, report}}
        },      
        {{1,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::q, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::q, report}}
        },      
        {{1,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::a, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::a, report}}
        },
        {{1,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::f13, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::f13, report}}
        },
        {{2,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num2, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num2, report}}
        },      
        {{2,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::w, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::w, report}}
        },      
        {{2,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::s, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::s, report}}
        },      
        {{2,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::z, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::z, report}}
        },      
        {{2,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lmeta, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lmeta, report}}
        },
        {{3,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num3, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num3, report}}
        },      
        {{3,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::e, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::e, report}}
        },      
        {{3,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::d, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::d, report}}
        },      
        {{3,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::x, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::x, report}}
        },      
        {{3,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lalt, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lalt, report}}
        },
        {{4,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num4, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num4, report}}
        },      
        {{4,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::r, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::r, report}}
        },      
        {{4,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::f, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::f, report}}
        },      
        {{4,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::c, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::c, report}}
        },      
        {{4,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::space, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::space, report}}
        },
        {{5,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num5, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num5, report}}
        },      
        {{5,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::t, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::t, report}}
        },      
        {{5,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::g, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::g, report}}
        },      
        {{5,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::v, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::v, report}}
        },      
        {{5,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lshift, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lshift, report}}
        },
        {{6,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num6, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num6, report}}
        },      
        {{6,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::y, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::y, report}}
        },      
        {{6,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::h, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::h, report}}
        },      
        {{6,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::b, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::b, report}}
        },      
        {{6,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lctrl, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lctrl, report}}
        },
        {{7,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num7, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num7, report}}
        },      
        {{7,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::u, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::u, report}}
        },      
        {{7,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::j, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::j, report}}
        },      
        {{7,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::n, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::n, report}}
        },
        {{8,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num8, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num8, report}}
        },      
        {{8,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::i, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::i, report}}
        },      
        {{8,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::k, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::k, report}}
        },      
        {{8,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::m, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::m, report}}
        },      
        {{8,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::space, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::space, report}}
        },
        {{9,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num9, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num9, report}}
        },      
        {{9,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::o, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::o, report}}
        },      
        {{9,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::l, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::l, report}}
        },      
        {{9,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::comma, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::comma, report}}
        },  
        {{9,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::ralt, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::ralt, report}}
        },
        {{10,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num0, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num0, report}}
        },      
        {{10,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::p, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::p, report}}
        },      
        {{10,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::semicolon, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::semicolon, report}}
        },
        {{10,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::dot, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::dot, report}}
        },  
        {{10,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::esc, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::esc, report}}
        },
        {{11,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::minus, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::minus, report}}
        },     
        {{11,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lbrace, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lbrace, report}}
        }, 
        {{11,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::apostrophe, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::apostrophe, report}}
        },
        {{11,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::slash, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::slash, report}}
        },
        {{11,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::rctrl, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::rctrl, report}}
        },
        {{12,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::equal, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::equal, report}}
        },     
        {{12,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::rbrace, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::rbrace, report}}
        },
        {{12,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::rshift, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::rshift, report}}
        }, 
        {{12,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::left, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::left, report}}
        },
        {{13,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::backspace, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::backspace, report}}
        }, 
        {{13,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::lslash, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::lslash, report}}
        }, 
        {{13,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::enter, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::enter, report}}
        },   
        {{13,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::up, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::up, report}}
        },     
        {{13,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::down, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::down, report}}
        },
        {{14,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::del, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::del, report}}
        },       
        {{14,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num7, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num7, report}}
        },   
        {{14,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num4, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num4, report}}
        },   
        {{14,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num1, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num1, report}}
        },   
        {{14,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::right, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::right, report}}
        },
        {{15,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::slash, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::slash, report}}
        },     
        {{15,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num8, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num8, report}}
        },   
        {{15,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num5, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num5, report}}
        },   
        {{15,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num2, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num2, report}}
        },   
        {{15,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num0, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num0, report}}
        },
        {{16,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::kpasterisk, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::kpasterisk, report}}
        },
        {{16,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num9, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num9, report}}
        },   
        {{16,2}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num6, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num6, report}}
        },  
        {{16,3}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::num3, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::num3, report}}
        },   
        {{16,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::dot, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::dot, report}}
        },
        {{17,0}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::minus, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::minus, report}}
        },     
        {{17,1}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::kpplus, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::kpplus, report}}
        },
        {{17,4}, 
            {event_t::pressed, keeb::macro::set_scancode{sc::none, report}},
            {event_t::released, keeb::macro::unset_scancode{sc::none, report}}
        }
    }
    {
        
    }
};


#endif //POKEY_LAYOUTS_HH_
