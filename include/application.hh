
#include <utl/utl.hh>
#include <utl/logger.hh>
#include <utl/construct.hh>
#include <driver/uart.hh>
#include <driver/pin/push-pull.hh>
#include <driver/adc.hh>
#include <driver/ucpd.hh>
#include <driver/pwm.hh>
#include <driver/dma.hh>
#include <driver/ws2812.hh>
#include <driver/spi.hh>
#include <driver/usb.hh>

namespace hw = stm32g4::driver;
namespace hid = utl::hal::usb::hid;

template <size_t Columns, typename Row_t>
struct matrix {
    using row_t = Row_t;
    static constexpr size_t columns = Columns;

    row_t data[columns];

    constexpr row_t* buffer() const volatile {
        return reinterpret_cast<row_t*>(data);
        // return data;
    }

    constexpr bool get(const size_t column, const row_t row) const volatile {
        return data[column] & (1 << row);
    }

    static constexpr size_t size() {
        return sizeof(data);
    }
};

template <typename Buffer_t, typename T>
volatile Buffer_t* dma_cast(volatile T& object) {
    return reinterpret_cast<volatile Buffer_t*>(const_cast<volatile std::remove_const_t<T>*>(&object));
}

template <typename T>
size_t dma_sizeof(T&& object) {
    return sizeof(object);
}

template <typename Matrix_t, typename Input_t>
void update(Matrix_t matrix, Input_t input, Keymap_t keymap) {
    auto new_state = matrix.get(input.column,input.row);
    input.update(new_state, keymap);
}


template <size_t Column, size_t Row>
class key {
    static constexpr size_t column = Column;
    static constexpr size_t row = Row;
    bool m_state;
    bool m_pressed;
public:
    template<typename Keymap_t>
    void update(bool new_state, Keymap_t keymap) {        
        if(!m_state && new_state) {
            press_started = utl::chrono::now();
        } else if (m_state && new_state) {
            if(!m_pressed && utl::chrono::now() - press_started > 20_ms) {
                m_pressed = true;
                keymap.pressed(*this);
            }
        } else if(m_state && !new_state) {
            release_started = utl::chrono::now();
        } else if(!m_state && !new_state) {
            if(m_pressed && utl::chrono::now() - release_started > 20_ms) {
                m_pressed = false;
                keymap.released(*this);
            }
        }
            
        m_state = new_state;
    }

    bool pressed() const {
        return m_pressed;
    }
};


//okay. how do I attach behaviour to a key.

namespace cckb::keymap {

//A keymap needs to implement all of the methods that an input's
//going to call on it. How do I... not do that?
//Probably only for a specific type, like an empty keymap.
//I could make that the input type's responsibility...
template <typename Input_t>
struct empty {};

template <size_t Column, size_t Row>
struct empty<key<Column,Row>> {
    constexpr void released(key k) { utl::maybe_unused(k); }
    constexpr void pressed(key k) { utl::maybe_unused(k); }
};

template <typename Input_t, size_t Column, size_t Row>
keymap(Input_t) -> keymap<Input_t,Row,Column>;


} //namespace cckb::keymap

//FIXME: maybe matrix is indexable with keys?
//FIXME: key should take a position in the row rather than a mask
//FIXME: need to be able to handle a column/row offset in the matrix

//NB: I think that the code instantiating a keyboard layout should be able to look like the physical keyboard.
//not like the matrix shape.
//this means I need to map the matrix to keys
//then keys to lambdas. ... or other things, like an object to collect keycodes.
//to perform updates, I would have to iterate over the keys or lambdas, check if the key is pressed, and then do the thing.
//then, there's a question of _when_ I do the thing. on press? on release? after a duration?
// make keymaps composable. make it easy to plug new behaviour into them.gggg



//could go the algorithm approach and have templated functions for
//what to do on press, release, etc. could probably even handle durations.
//template arguments would uniquely specify the key.
//these would have to take the hid device (perhaps abstracted) as an argument.
//or, maybe, they're a template method on some type that holds its dependencies.
//probably on a type.

//that means that keys would need to be templated on their position? or does it not?
//it means the keys need to be constexpr. which is totally doable right now.

//okay. the matrix & switch types are an electrical specification: how large is the grid, and where
//are the switches. these should be constexpr.
//this also provides a nice hook point for things like encoders, or other types of switches.
//which means that the grid of switches is heterogenous in type. so there are some trait
//requirements on switch types. great.

//MATRIX: array with a get(position) method
//POSITIONS: constexpr array of positions.
//SWITCH: templated on position. performs updates, stores switch state. update method that accepts a layout? tuple of switches.
//UPDATE: template function that accepts a matrix, a tuple of switches and a layout.
//  iterates over the switches and calls layout functions appropriately.
//LAYOUT: 

//So I have a constexpr collection of switches with positions.
//How do I map that to behaviour?
//If every switch's type is distinct, I can rely on specializations or overloads.
//If switch types aren't distinct, maybe something with lambdas.
//The bigger question is: how does the developer uniquely identify a switch?
//Position seems appropriate.
//And I like the idea of using overloads, because it more cleanly separates switches.
//And methods over functions because it encapsulates.

//I need a behavioural hook here. I guess I can use press, release, and some kind of duration function
//to provide that? Yeah. Probably. It gets called every scan tick, and the default implementation is
//empty so it can be optimized away.

//So I have classes that encapsulate behaviour for a switch layout.
//How do I switch between them? They certainly have different types.
//Virtuals could be appropriate here; I could see an interface that's only
//invoked once per scan. What are these things? Layers?

//What's the highest level of abstraction I want the user to be dealing with
//here? Layers & various collections of layers? A layer state machine?
//Interestingly enough, this could allow you to have multiple active layers. Which'd be interesting.
//Anyway, this allows layer composition in a pretty flexible way. Can I do that without the
//layers themselves having to know about it?

//One of the actions a key is going to need to be able to take is to change layers. Maybe to
//a specific one, or popping off a stack, or whatever. So those actions can't be generic.
//But... if they had access to the type, the user could specialize or if constexpr to
//handle those cases. Okay.

//then, we have 

class application {
public:
    using uart_t = hw::uart<false>;
    using log_out_t = utl::logger::output<uart_t>;
    using led_pwm_t = hw::pwm::source<utl::duration::ns>;
    using leds_t = hw::ws2812<85, led_pwm_t, hw::dma::channel>;
    using spi_t = hw::spi::dma_master;
    using usb_t = hw::usb::hid::device<hid::keyboard_report>;
    using matrix_t = volatile const matrix<19, uint8_t>;
    using key_t = key<matrix_t>;
private:
    utl::construct<uart_t>                      m_uart;
    utl::construct<log_out_t>                   m_application_output;
    utl::logger::push_output                    m_logger_setup;
    hw::ucpd                                    m_ucpd;
    hw::pin::push_pull                          m_power_switch;
    utl::construct<hw::adc>                     m_adc;
    utl::construct<hw::adc::adc_channel_t>      m_current_sense;
    utl::construct<leds_t::dma_channel_t>       m_led_dma;
    utl::construct<led_pwm_t>                   m_led_pwm_source; 
    utl::construct<led_pwm_t::dma_channel_t>    m_led_pwm;
    utl::construct<leds_t>                      m_leds;
    utl::construct<spi_t::rx_dma_channel_t>     m_spi_rx_dma;
    utl::construct<spi_t::tx_dma_channel_t>     m_spi_tx_dma;
    utl::construct<spi_t>                       m_spi;
    utl::construct<usb_t>                       m_usb;   
    volatile const matrix_t                     m_matrix;
    uint8_t                                     m_dummy_send[matrix_t::size()];
    const key_t                                 m_test_key;
    uint32_t                                    m_march_count = 0;
public:
    application();
    void start(void);
    void loop(void);
};

