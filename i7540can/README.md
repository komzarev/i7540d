# QCanBus Plugin for ICP DAS I7540D

This project provides a QCanBus plugin for the ICP DAS I7540D adapter. The ICP DAS I7540D allows information to be transmitted via Ethernet to CAN.

## Usage

To include in your project, you will need the header files: `i7540can.hpp` and `i7540can_global.hpp`. The interface and usage are similar to QCanBus from Qt5.

## Example

```cpp
#include "i7540can.hpp"

int main(void) {
    I7540Can can("192.168.0.1", I7540Can::CAN);
    // Use the `can` object similarly to a QCanBusDevice
    return 0;
}
```
