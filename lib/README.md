Contents
========

Here are the main libraries for the Universal Nixie Display Board B (VFD controller).

AnimatorFailureListener
-----------------------

Classes that can handle animation timing failures (can be used for debugging).

DeviceAnimator
--------------

Abstract, device-agnostic animation planner/executor, allowing for multithreaded animation of various
device elements (like status LED, VFD digits etc.).

DutyCycleGenerator
------------------

Utility classes for non-linear changing duty cycle generation for animating the device's elements (like
LED or digit fade-in or fade-out cycles).

I2CComms
--------

A class for receiving and buffering commands sent over I2C and for initializing and maintaining the communication.

IV18Display
-----------

Concrete, device-specific classes to govern and animate the display, execute concrete I2C commands.