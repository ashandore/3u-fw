#ifndef CCKB_KEEB_HH_
#define CCKB_KEEB_HH_

#include <utl/utl.hh>
#include <utl/array.hh>
#include <utl/matrix.hh>
#include <utl/tuple.hh>
#include <utl/variant.hh>
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



template <typename Config_t>
using input_t = typename Config_t::input_t;

template <typename Config_t>
using event_t = typename Config_t::input_t::event;

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


template <typename Config_t, typename... Macros>
struct slot {
    using config_t = Config_t;
    using event_t = event_t<Config_t>;
    using input_t = input_t<Config_t>;

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

//connects a set of slots to a position
template <typename... Slots>
struct signal {

    //FIXME: require that all Slots have the same config type
    //as us.

    const pos_t pos;
    utl::tuple<Slots...> slots;

    constexpr signal(pos_t&& pos_, Slots&&... slots_) : pos{pos_}, slots{slots_...} {}
};

template <typename... Slots>
signal(Slots...) -> signal<Slots...>;

namespace detail {

template <typename Config_t, size_t N, class = utl::make_index_sequence<N>, typename... Slots>
struct layout;

template <typename Config_t, size_t N, size_t... Ns, typename... Slots>
struct layout<Config_t,N,utl::index_sequence<Ns...>,Slots...> {
    using config_t = Config_t;
    using signal_t = signal<Slots...>;
    using array_t = utl::array<signal_t,N>;
    
    array_t signals;

    template <size_t>
    using _signal_t = signal_t;
    
    layout(_signal_t<Ns>&&... signals_) : signals{std::forward<signal_t>(signals_)...} 
    {}
};


} //namespace detail

template <typename Config_t, size_t N, typename... Slots>
struct layout : public detail::layout<Config_t,N,utl::make_index_sequence<N>,Slots...> {
    using detail::layout<Config_t,N,utl::make_index_sequence<N>,Slots...>::layout;
};

template <typename Config_t>
class keyboard : public Config_t {
public:
    using config_t = Config_t;
    using scan_matrix_t = typename config_t::scan_matrix_t;
    using input_matrix_t = utl::matrix<input_t<config_t>,config_t::columns,config_t::rows>;

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



//FIXME: should have some kind of context object that gets passed around.
//it's what macros can use to... do stuff


template <typename Keyboard_t, typename Config_t, typename... Macros>
void emit(Keyboard_t& keyboard, slot<Config_t,Macros...>& s, input_t<Config_t>& input) {
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

template <typename Keyboard_t, typename Config_t, size_t N, typename... Slots>
void emit(Keyboard_t& keyboard, layout<Config_t,N,Slots...>& layout, pos_t pos, input_t<Config_t>& input, event_t<Config_t>& event) {
    for(auto& signal : layout.signals) {
        if(signal.pos == pos) {
            emit(keyboard, signal, input, event);
        }
    }
}

template <typename Keyboard_t, typename Visit_t>
void update(Keyboard_t& keyboard, Visit_t& layout_collection) {
    for(size_t col = 0; col < keyboard.width(); col++) {
        for(size_t row = 0; row < keyboard.height(); row++) {
            pos_t pos{col,row};
            auto new_state = keyboard.scan_matrix.get(pos);
            auto& input = keyboard.inputs[pos];
            input.update(new_state, pos, 
                [&](event_t<Keyboard_t> event) {
                    layout_collection.accept([&](auto* layout) {
                        emit(keyboard,*layout,pos,input,event);
                    });
                }
            );
        }
    }
}

template <typename Config_t>
constexpr size_t get_width() {
    return keyboard<Config_t>::width();
}

template <typename Config_t>
constexpr size_t get_height() {
    return keyboard<Config_t>::height();
}

template <typename Config_t>
constexpr size_t get_keycount() {
    return Config_t::keycount;
}

template <size_t N, typename... Ts>
struct layout_stack {
    struct empty {};

    utl::array<utl::variant<empty,Ts*...>,sizeof...(Ts)> m_stack;
    size_t m_index;
public:
    template <typename T>
    constexpr layout_stack(T& layout) : m_stack{&layout}, m_index{0}
    {}

    template<typename T>
    void push(T& layout) {
        if(m_index == N) return;
        m_stack[m_index] = &layout;
        m_index++;
    }

    void pop() {
        if(m_index == 0) return;
        m_index--;
        m_stack[m_index] = empty{};
    }

    template <typename F>
    void accept(F&& visitor) {
        m_stack[m_index-1].accept(visitor);
    }
};

} //namespace keeb

#endif //CCKB_KEEB_HH_
