Universal Nixie Display
=======================

VFD microcontroller
-------------------

Hardware: Atmega328p

Controls the Board B of the Universal Nixie Display (the VFD module).
Receives commands via I²C.

Available commands
------------------

Defined in `IV18I2CCommandExecutor`.
See the public constants there. The "instant commands" will be run within the same frame during which they are received.
The "bunchable commands" will be run when `CMD_MULTI_FINISH` instant command is received, all at once, in the order
in which they were sent.

TODO
----

- add CMakeLists.txt to git?
- make a fresh platformio deployment instruction