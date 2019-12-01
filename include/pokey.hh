#ifndef POKEY_HH_
#define POKEY_HH_

#include <utl/utl.hh>
#include <keeb.hh>
#include <key.hh>
#include <macro.hh>
#include <utl/tuple.hh>
#include <utl/hal/usb.hh>

#include "layouts/default.hh"
// #include "layouts/layer_mods.hh"

struct config {
    static constexpr size_t columns = 18;
    static constexpr size_t rows = 5;
    static constexpr size_t keycount = 85;
    using scan_matrix_t = volatile const keeb::matrix<columns,uint8_t,true>;
    using input_t = keeb::input::key;
};

struct pokey : keeb::keyboard<config> {
    using kb = keeb::keyboard<config>;

    default_layout<config> m_default_layout;
    // mod_layout<config> m_mod_layout;

    using layout_stack_t = keeb::layout_stack<3,default_layout<config>>;
    layout_stack_t m_layout_stack;
    
    pokey(scan_matrix_t& scan_matrix_, utl::hal::usb::hid::keyboard_report& report_)
      : kb{scan_matrix_, {}}, m_default_layout{report_}, 
        m_layout_stack{m_default_layout}
    {}

    void update() {
        keeb::update(*this, m_layout_stack);
    }
};



#endif //POKEY_HH_
