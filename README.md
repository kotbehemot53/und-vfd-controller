Universal Nixie Display
=======================

VFD microcontroller
-------------------

Hardware: Atmega328p

*This is a work in progress*

Controls the Board B of the Universal Nixie Display (the VFD module).
Receives commands via I²C.

The current device address is `0x5`.

Available commands
------------------

Defined in `IV18I2CCommandExecutor.h`.
See the public constants and their comments there. 

The "instant commands" will be run within the same frame during which they are received.

The "bunchable commands" will be run when `CMD_MULTI_FINISH` instant command is received, all at once, in the order
in which they were sent.

Some commands require a following byte to be sent, to set a value (see command comments).
Not sending the required following byte will result in an undefined behaviour.

TODO
----

- add CMakeLists.txt to git?
- make a fresh platformio deployment instruction;