# I7540dSerialAdapter #

Проект динамической библиотеки для работы с последовательными интерфейсам через адаптер ICP DAS I7540D.
ICP DAS I7540D позволяет передавать информацию через Ethernet в CAN,RS232,RS485.

### Использование ###

Для включения в свой проект понадобятся заголовочные фалы: i7540dserialadapter.hpp и i7540dserialadapter_global.hpp.
Интерфейс и использование аналогичны QSerialPort из Qt5, в качестве имени порта надо использовать IP адрес устройства.

### Пример 
```
#include "i7540dserialadapter.hpp"

int main(void) {
    I7540dSerialAdapter serial("192.168.0.1", I7540dSerialAdapter::RS485);
    //используем serial также как объект класса QSerialPort
    return 0;
}
```
