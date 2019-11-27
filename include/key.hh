#ifndef CCKB_KEY_HH_
#define CCKB_KEY_HH_

#include <utl/utl.hh>
#include <keeb.hh>

namespace keeb::input {

class key {
    uint32_t m_time;
    bool m_state:4;
    bool m_pressed:4;
public:
    enum class event : uint8_t {
        pressed,
        released,
        held
    };

    template <typename S>
    bool update(bool new_state, keeb::pos_t pos, S signal) {  
        utl::maybe_unused(pos);      
        if(!m_state && new_state) { //maybe a press
            m_time = HAL_GetTick();
        } else if (m_state && new_state) { //being held
            if(!m_pressed && HAL_GetTick() - m_time > 20) {
                m_pressed = true;
                utl::log("key pressed!");
                signal(event::pressed);
            }
        } else if(m_state && !new_state) { //maybe a release
            m_time = HAL_GetTick();
        } else if(!m_state && !new_state) { //not being held
            if(m_pressed && HAL_GetTick() - m_time > 20) {
                m_pressed = false;
                signal(event::released);
            }
        }
            
        m_state = new_state;
        return true;
    }    
};

} //namespace keeb::input

#endif //CCKB_KEY_HH_
