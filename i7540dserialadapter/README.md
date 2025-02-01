# I7540dSerialAdapter #

A dynamic library project for working with serial interfaces through the ICP DAS I7540D adapter.
The ICP DAS I7540D allows information to be transmitted via Ethernet to CAN, RS232, RS485.

### Usage ###

To include in your project, you will need the header files: i7540dserialadapter.hpp and i7540dserialadapter_global.hpp.
The interface and usage are similar to QSerialPort from Qt5, and the port name should be the IP address of the device.

### Example
```cpp
#include "i7540dserialadapter.hpp"

int main(void) {
    I7540dSerialAdapter serial("192.168.0.1", I7540dSerialAdapter::RS485);
    //используем serial также как объект класса QSerialPort
    return 0;
}
```
