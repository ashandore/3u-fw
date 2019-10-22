# Board Changes

# Firmware Work
* abstract ST HAL callbacks
* interrupt abstraction
* how to configure callbacks at compile time? typically a driver will set a callback on a dependency, but that's the wrong order to do it at compile time.
* an abstraction like a hardware provider to hold board hardware
* think about what happens when a construct<T> is passed as a constructor argument to another construct<T> in place of a T, T*, or T&. could it handle those cases automatically?
 * unbox or nullptr for T*
 * unbox or return error for T
 * unbox or return error for T&