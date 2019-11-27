#ifndef CCKB_MACRO_HH_
#define CCKB_MACRO_HH_

#include <utl/utl.hh>
#include <utl/hal/usb.hh>

namespace keeb::macro {

using keycode_t = utl::hal::usb::hid::scancode::code;
using keycode_buffer_t = utl::hal::usb::hid::keyboard_report;

struct set_scancode {
    keycode_t const       m_code;
    keycode_buffer_t&     m_buffer;

    template <typename Input_t>
    constexpr bool operator()(Input_t& input) {
        utl::maybe_unused(input);
        m_buffer.add(m_code);
        return true;
    }
};

struct unset_scancode {
    keycode_t const       m_code;
    keycode_buffer_t&     m_buffer;

    template <typename Input_t>
    constexpr bool operator()(Input_t& input) {
        utl::maybe_unused(input);
        m_buffer.remove(m_code);
        return true;
    }
};

} //namespace keeb::macro

#endif //CCKB_MACRO_HH_
