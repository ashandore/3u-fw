#ifndef CCKB_KEEB_HH_
#define CCKB_KEEB_HH_

#include <utl/utl.hh>
#include <utl/array.hh>
#include <utl/matrix.hh>
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

//FIXME: DiscardFirst is ugly.
template <size_t Columns, typename Row_t, bool DiscardFirst = false>
struct matrix {
    using row_t = Row_t;
    static constexpr size_t columns = Columns;

    utl::array<row_t,columns + (DiscardFirst ? 1 : 0)> data;

    constexpr row_t* buffer() const volatile {
        return &data[0];
    }

    constexpr bool get(const size_t column, const row_t row) const volatile {
        if constexpr(DiscardFirst) {
            return data[column + 1] & (1 << (sizeof(row_t)*8 - row - 1));
        } else {
            return data[column] & (1 << (sizeof(row_t)*8 - row - 1));
        }
    }

    static constexpr size_t size() {
        return sizeof(row_t)*columns;
    }
};

namespace input {

class key {
    uint32_t m_press_started;
    uint32_t m_release_started;
    bool m_state;
    bool m_pressed;
public:
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

} //namespace input


namespace action {

//A keymap needs to implement all of the methods that an input's
//going to call on it. How do I... not do that?
//Probably only for a specific type, like an empty keymap.
//I could make that the input type's responsibility...
template <typename Input_t>
struct empty { using input_t = Input_t; };

template <>
struct empty<input::key> {
    using input_t = input::key;
    constexpr void released() {}
    constexpr void pressed() {}
};


using keycode_t = utl::hal::usb::hid::scancode::code;
using keycode_buffer_t = utl::hal::usb::hid::keyboard_report;

class keycode {
    keycode_t const       m_code;
    keycode_buffer_t&     m_buffer;
public:
    keycode(keycode_t code, keycode_buffer_t& buff)
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

template <typename Input_t, typename Action_t>
struct element {
    using input_t = Input_t;
    using action_t = Action_t;

    input_t& input;
    action_t& action;
    size_t column;
    size_t row;

    template <typename Matrix_t>
    void update(Matrix_t&& matrix) {
        auto new_state = matrix.get(column, row);
        input.update(new_state, action);
    }
};

template <typename Input_t, typename Action_t>
element(Input_t,Action_t,size_t,size_t) -> element<Input_t,Action_t>;


template <typename Matrix_t, typename Input_t, typename Action_t, size_t Rows>
struct keyboard {
    using matrix_t = Matrix_t;
    using input_t = Input_t;
    using action_t = Action_t;
    using input_matrix_t = utl::matrix<input_t,matrix_t::columns,Rows>;
    using action_matrix_t = utl::matrix<action_t,matrix_t::columns,Rows>;

    matrix_t& matrix;
    input_matrix_t inputs;
    action_matrix_t actions;

    void update() {
        for(size_t col = 0; col < matrix_t::columns; col++) {
            for(size_t row = 0; row < Rows; row++) {
                auto new_state = matrix.get(col,static_cast<typename matrix_t::row_t>(row));
                auto& action = actions[{col,row}];
                inputs[{col,row}].update(new_state, action);
            }
        }
    }
};

template <typename Matrix_t, typename Input_t, typename Action_t, size_t Rows>
keyboard(Matrix_t, utl::matrix<Input_t,Matrix_t::columns,Rows>, utl::matrix<Action_t,Matrix_t::columns,Rows>) -> keyboard<Matrix_t, Input_t, Action_t, Rows>;

} //namespace keeb

#endif //CCKB_KEEB_HH_
