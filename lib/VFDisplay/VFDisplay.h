#ifndef PIUCNTVFD1_VFDISPLAY_H
#define PIUCNTVFD1_VFDISPLAY_H

#include <Arduino.h>

class VFDisplay {

public:
    void init();

    void on();
    void off();
    void displayBytes(char*); //TODO: how to ensure length of 8? enable exceptions?
    void displayChars(byte*); //TODO: how to ensure length of 8? enable exceptions?

};

extern VFDisplay Display;

#endif //PIUCNTVFD1_VFDISPLAY_H
