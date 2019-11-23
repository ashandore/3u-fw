#ifndef CCKB_KEEB_HH_
#define CCKB_KEEB_HH_

#include <utl/utl.hh>
#include <utl/array.hh>
#include <utl/matrix.hh>
#include <utl/tuple.hh>

namespace keeb {

using pos_t = utl::matrix_index_t;

namespace interface {

template <typename Input_t>
struct emitter {
    using input_t = Input_t;
    using event_t = typename input_t::event;
    virtual void emit(input_t& input, event_t event) = 0;
    virtual ~emitter() = default;
};

template <typename traits>
struct layout {
    using input_t = typename traits::input_t;
    using event_t = typename input_t::event;
    virtual emitter<input_t>* get(pos_t pos) = 0;
    virtual ~layout() = default;
};

} //namespace interface



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
    static constexpr size_t rows = 8*sizeof(row_t);

    utl::array<row_t,columns + (DiscardFirst ? 1 : 0)> data;

    constexpr row_t* buffer() const volatile {
        return &data[0];
    }

    constexpr bool get(const size_t column, const size_t row) const volatile {
        if constexpr(DiscardFirst) {
            return data[column + 1] & static_cast<row_t>(1 << (rows - row - 1));
        } else {
            return data[column] & static_cast<row_t>(1 << (rows - row - 1));
        }
    }

    static constexpr size_t size() {
        return rows*columns;
    }
};


template <typename Input_t, typename... Macros>
struct handler {
    using input_t = Input_t;
    using event_t = typename input_t::event;

    template <typename Callable, typename... Args>
    using is_callable_t = decltype(std::declval<Callable&>()(std::declval<Args>()...));

    template <typename Macro>
    using is_macro_t = is_callable_t<Macro,input_t>;

    // static_assert((is_detected_v<is_macro_t,Macros> && ...), "macro is not callable with the input it is attached to!"
    //     "(a macro must be callable and accept a single argument of the type of input it is attached to)");

    const event_t event;
    utl::tuple<Macros...> macros;
    
    void operator()(input_t& input) {
        utl::for_each(macros, [&](auto& macro) {
            return macro(input);
        });
    }
};

template <typename Input_t, typename... Macros>
handler<Input_t,Macros...> make_handler(typename Input_t::event event, Macros&&... macros) { return {event,{std::forward<Macros>(macros)...}}; }

//connects a set of handlers to a position
template <typename Input_t, typename... Handlers>
struct emitter : public virtual interface::emitter<Input_t> {
    using input_t = Input_t;
    using event_t = typename input_t::event;

    const pos_t pos;
    utl::tuple<Handlers...> handlers;

    constexpr emitter(pos_t p, utl::tuple<Handlers...> h) : pos{p}, handlers{h} {}

    void emit(input_t& input, event_t event) final {
        utl::for_each(handlers, [&](auto& handler) {
            if(handler.event == event) handler(input);
        });
    }
};

template <typename Input_t, typename... Handlers>
emitter<Input_t,Handlers...> make_emitter(pos_t pos, Handlers&&... handlers) { return {pos, utl::make_tuple{std::forward<Handlers>(handlers)...}}; }



template <typename KeyboardPolicy>
class keyboard {
    using config_t = KeyboardPolicy;
    using scan_matrix_t = typename config_t::scan_matrix_t;
    using input_t = typename config_t::input_t;
    
    template <typename T>
    using layout_t = utl::matrix<T,config_t::columns, config_t::rows>;

    template <size_t N, typename... Handlers>
    using layout_t = utl::matrix<emitter<config_t,Handlers...>,config_t::columns, config_t::rows>;
};


template <typename traits>
class keyboard {
    using scan_matrix_t = typename traits::scan_matrix_t;
    using input_t = typename traits::input_t;
    using input_matrix_t = utl::matrix<input_t,scan_matrix_t::columns,traits::rows>;
    using layout_tuple_t = std::result_of_t<decltype(traits::make_layouts)>;

    static_assert(traits::rows <= scan_matrix_t::rows, 
        "keyboard cannot have more rows than its scan matrix.");
    static_assert(traits::columns <= scan_matrix_t::columns,
        "keyboard cannot have more columns than its scan matrix");


    scan_matrix_t&                              m_scan_matrix;
    input_matrix_t                              m_inputs;
    layout_tuple_t                              m_layouts;
    interface::layout<traits>*                  m_active_layout;
public:
    template <typename... Emitters>



    static constexpr size_t width = scan_matrix_t::columns;
    static constexpr size_t height = traits::rows;

    template <typename... Args>
    constexpr keyboard(scan_matrix_t& scan_matrix, input_matrix_t inputs, Args&&... args)
        : m_scan_matrix{scan_matrix}, m_inputs{inputs}, m_layouts{traits::make_layouts(std::forward<Args>(args)...)},
          m_active_layout{&m_layouts.template get<0>()}
    {}

    void update() {
        if(m_active_layout == nullptr) return;
        // for(auto pos : utl::indices(m_inputs)) {
        //     auto new_state = m_scan_matrix.get(pos);
        //     m_inputs[pos].update(new_state, m_active_layout->get(pos));
        // }

        for(size_t col = 0; col < m_inputs.width(); col++) {
            for(size_t row = 0; row < m_inputs.height(); row++) {
                pos_t pos{col,row};
                auto new_state = m_scan_matrix.get(pos);
                m_inputs[pos].update(new_state, m_active_layout->get(pos));
            }
        }
    }

    void set_layout(interface::layout<traits>* layout) {
        m_active_layout = layout;
    }
};

// template <typename Matrix_t, typename Input_t, size_t Rows>
// keyboard(Matrix_t, utl::matrix<Input_t,Matrix_t::columns,Rows>, interface::layout<keyboard<Matrix_t,Input_t,Rows>>*) -> keyboard<Matrix_t, Input_t, Rows>;

} //namespace keeb

#endif //CCKB_KEEB_HH_
