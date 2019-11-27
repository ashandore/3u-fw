#ifndef CCKB_KEEB_HH_
#define CCKB_KEEB_HH_

#include <utl/utl.hh>
#include <utl/array.hh>
#include <utl/matrix.hh>
#include <utl/tuple.hh>
#include <initializer_list>

namespace keeb {

struct pos_t {
    uint8_t col;
    uint8_t row;
    // constexpr pos_t(uint8_t c, uint8_t r) : col{c}, row{r} {}
    constexpr pos_t(size_t c, size_t r) 
      : col{static_cast<uint8_t>(c)}, row{static_cast<uint8_t>(r)}
    {}
    
    constexpr operator utl::matrix_index_t() {
        return {static_cast<size_t>(col), static_cast<size_t>(row)};
    }

    bool operator==(pos_t const& other) const {
        return col == other.col and row == other.row;
    }
};


namespace input {

template <typename Event_t>
struct get_input_type {
    static_assert("all input types must define a specialization of the "
        "get_input_type trait that accepts their event type and "
        "produces the input type.");
};

template <typename T>
using get_input_t = typename get_input_type<T>::type;

} //namespace input

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
    static constexpr uint8_t columns = Columns;
    static constexpr uint8_t rows = 8*sizeof(row_t);

    utl::array<row_t,columns + (DiscardFirst ? 1 : 0)> data;

    constexpr row_t* buffer() const volatile {
        return &data[0];
    }

    constexpr bool get(const uint8_t column, const uint8_t row) const volatile {
        if constexpr(DiscardFirst) {
            return data[column + 1] & static_cast<row_t>(1 << (rows - row - 1));
        } else {
            return data[column] & static_cast<row_t>(1 << (rows - row - 1));
        }
    }

    constexpr bool get(const pos_t pos) const volatile {
        return get(pos.col,pos.row);
    }

    static constexpr size_t size() {
        return rows*columns;
    }
};


template <typename T, typename... Macros>
struct slot {
    using event_t = T;
    using input_t = input::get_input_t<event_t>;

    template <typename Callable, typename... Args>
    using is_callable_t = decltype(std::declval<Callable&>()(std::declval<Args>()...));

    template <typename Macro>
    using is_macro_t = is_callable_t<Macro,input_t>;

    //Static assertions:
    // - the macros are callable, with the input they're attached to as an input
    // - all macros have the same signature
    // - could I get the input type from the signature of the macro?

    // static_assert((is_detected_v<is_macro_t,Macros> && ...), "macro is not callable with the input it is attached to!"
    //     "(a macro must be callable and accept a single argument of the type of input it is attached to)");

    const event_t event;
    utl::tuple<Macros...> macros;

    constexpr slot(event_t event_, Macros... macros_) : event{event_}, macros{macros_...} {}
};



template <typename T, typename... Ts>
slot(T,Ts...) -> slot<T,Ts...>;

//connects a set of slots to a position
template <typename... Slots>
struct signal {
    using input_t = typename utl::get_t<0,Slots...>::input_t;
    using event_t = typename input_t::event;

    const pos_t pos;
    utl::tuple<Slots...> slots;

    constexpr signal(pos_t&& pos_, Slots&&... slots_) : pos{pos_}, slots{slots_...} {}
};

template <typename... Slots>
signal(pos_t,Slots...) -> signal<Slots...>;

namespace detail {

template <size_t N, class = utl::make_index_sequence<N>, typename... Slots>
class layout;

template <size_t N, size_t... Ns, typename... Slots>
class layout<N,utl::index_sequence<Ns...>,Slots...> {
public:
    using input_t = typename utl::get_t<0,Slots...>::input_t;
    using event_t = typename input_t::event;
    using signal_t = signal<Slots...>;
    using array_t = utl::array<signal_t,N>;
    
    array_t signals;

    template <size_t>
    using _signal_t = signal_t;
public:
    layout(_signal_t<Ns>&&... signals_) : signals{signals_...} {}
};



} //namespace detail

template <size_t N, typename... Slots>
using layout = detail::layout<N,utl::make_index_sequence<N>,Slots...>;

template <typename KeebPolicy>
class keyboard : public KeebPolicy {
public:
    using config_t = KeebPolicy;
    using scan_matrix_t = typename config_t::scan_matrix_t;
    using input_t = typename config_t::input_t;
    using event_t = typename input_t::event;
    using input_matrix_t = utl::matrix<input_t,config_t::columns,config_t::rows>;

    template <size_t N, typename... Slots>
    using layout_t = layout<N,Slots...>;   

    template <typename... Macros>
    using slot_t = slot<event_t,Macros...>;

    static_assert(config_t::rows <= scan_matrix_t::rows, 
        "keyboard cannot have more rows than its scan matrix.");
    static_assert(config_t::columns <= scan_matrix_t::columns,
        "keyboard cannot have more columns than its scan matrix");

    scan_matrix_t&                              scan_matrix;
    input_matrix_t                              inputs;

    constexpr keyboard(scan_matrix_t& scan_matrix_, input_matrix_t inputs_)
        : scan_matrix{scan_matrix_}, inputs{inputs_}
    {}

    constexpr size_t width() { return config_t::columns; }
    constexpr size_t height() { return config_t::rows; }
};

template <typename Keyboard_t>
using input_t = typename Keyboard_t::input_t;

template <typename Keyboard_t>
using event_t = typename Keyboard_t::event_t;

//FIXME: should have some kind of context object that gets passed around.
//it's what macros can use to... do stuff


template <typename Keyboard_t, typename... Macros>
void emit(Keyboard_t& keyboard, slot<event_t<Keyboard_t>,Macros...>& s, input_t<Keyboard_t>& input) {
    utl::maybe_unused(keyboard);
    utl::for_each(s.macros, [&](auto& macro) { 
        macro(input); 
    });
}

template <typename Keyboard_t, typename... Slots>
void emit(Keyboard_t& keyboard, signal<Slots...>& s, input_t<Keyboard_t>& input, event_t<Keyboard_t> event) {
    utl::for_each(s.slots, [&](auto& slot) {
        if(slot.event == event) emit(keyboard, slot, input);
    });
}

template <typename Keyboard_t, size_t N, typename... Slots>
void emit(Keyboard_t& keyboard, layout<N, Slots...>& layout, pos_t pos, input_t<Keyboard_t>& input, event_t<Keyboard_t>& event) {
    for(auto& signal : layout.signals) {
        if(signal.pos == pos) {
            emit(keyboard, signal, input, event);
        }
    }
}

template <typename Keyboard_t, typename Layout_t>
void update(Keyboard_t& keyboard, Layout_t& layout) {

    for(size_t col = 0; col < keyboard.width(); col++) {
        for(size_t row = 0; row < keyboard.height(); row++) {
            pos_t pos{col,row};
            auto new_state = keyboard.scan_matrix.get(pos);
            auto& input = keyboard.inputs[pos];
            input.update(new_state, pos, 
                [&](event_t<Keyboard_t> event) {
                    emit(keyboard,layout,pos,input,event);
                }
            );
        }
    }
}

// template <typename Matrix_t, typename Input_t, size_t Rows>
// keyboard(Matrix_t, utl::matrix<Input_t,Matrix_t::columns,Rows>, interface::layout<keyboard<Matrix_t,Input_t,Rows>>*) -> keyboard<Matrix_t, Input_t, Rows>;

} //namespace keeb

#endif //CCKB_KEEB_HH_
