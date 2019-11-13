#ifndef CCKB_KEEB_HH_
#define CCKB_KEEB_HH_

#include <utl/utl.hh>
#include <utl/hal/usb.hh>

namespace keeb {

//something to think about:
//if this is intended to be used by others, it might
//make sense to be relatively strict by default.
//Heavily constrain the semantics of all of these types
//and provide useful error messages when those constraints
//are violated. Then, all of these types can accept
//policies for each constraint to relax it if the user
//wishes. Provide useful compile time messages
//wherever possible.


template <size_t Columns, typename Row_t>
struct matrix {
    using row_t = Row_t;
    static constexpr size_t columns = Columns;

    row_t data[columns];

    constexpr row_t* buffer() const volatile {
        return reinterpret_cast<row_t*>(data);
    }

    constexpr bool get(const size_t column, const row_t row) const volatile {
        return data[column] & (1 << (sizeof(row_t)*8 - row - 1));
    }

    static constexpr size_t size() {
        return sizeof(data);
    }
};


template <size_t Column, size_t Row>
class key {
    uint32_t m_press_started;
    uint32_t m_release_started;
    bool m_state;
    bool m_pressed;
public:
    static constexpr size_t column = Column;
    static constexpr size_t row = Row;

    template<typename Keymap_t>
    void update(bool new_state, Keymap_t keymap) {        
        if(!m_state && new_state) {
            m_press_started = HAL_GetTick();
        } else if (m_state && new_state) {
            if(!m_pressed && HAL_GetTick() - m_press_started > 20) {
                m_pressed = true;
                keymap.pressed();
            }
        } else if(m_state && !new_state) {
            m_release_started = HAL_GetTick();
        } else if(!m_state && !new_state) {
            if(m_pressed && HAL_GetTick() - m_release_started > 20) {
                m_pressed = false;
                keymap.released();
            }
        }
            
        m_state = new_state;
    }

    bool pressed() const {
        return m_pressed;
    }
};



namespace action {

//A keymap needs to implement all of the methods that an input's
//going to call on it. How do I... not do that?
//Probably only for a specific type, like an empty keymap.
//I could make that the input type's responsibility...
template <typename Input_t>
struct empty {};

template <size_t Column, size_t Row>
struct empty<key<Column,Row>> {
    using key_t = key<Column,Row>;
    constexpr void released(key_t k) { utl::maybe_unused(k); }
    constexpr void pressed(key_t k) { utl::maybe_unused(k); }
};


using keycode_t = utl::hal::usb::hid::keycode;
using keycode_buffer_t = utl::hal::usb::hid::keycode_buffer;

class keycode {
    keycode_t const       m_code;
    keycode_buffer_t&     m_buffer;
public:
    keycode(uint8_t code, keycode_buffer_t& buff)
        : m_code{code}, m_buffer{buff}
    {}

    constexpr void pressed() {
        m_buffer.add(m_code);
    }

    constexpr void released() {
        m_buffer.remove(m_code);
    }
};

} //namespace action



template <typename Matrix_t, typename Input_t, typename Action_t>
void update(Matrix_t&& matrix, Input_t&& input, Action_t&& action) {
    auto new_state = matrix.get(input.column,input.row);
    input.update(new_state, std::forward<Action_t>(action));
}

//the keyboard is just responsible for holding an Input_t matrix.
// template <typename... Input_t>
// struct keyboard {

// };

//then, a layer is what holds an Action_t matrix.
// template <typename... Actions>
// struct layer {
//     //need a tuple...


// };



} //namespace keeb

#endif //CCKB_KEEB_HH_
