# Board Changes

# Firmware Work
* abstract ST HAL callbacks
* interrupt abstraction
* how to configure callbacks at compile time? typically a driver will set a callback on a dependency, but that's the wrong order to do it at compile time.
* an abstraction like a hardware provider to hold board hardware
 * unbox or nullptr for T*
 * unbox or return error for T
 * unbox or return error for T&



## Keyboard Abstraction
* Find a good means of storing global macro state. holding 170 references to the same thing is wasteful.
* where should the HID report live? it's going to be commonly used. it should probably be hidden behind an abstraction.
* whiteboard out the architecture as it stands

