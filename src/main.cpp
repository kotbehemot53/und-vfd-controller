#include "../lib/Blinker/StatusBlinker.h"

//Blinker* blinker;

void setup() {
//    blinker = new Blinker();
    Blinker.init();
}

void loop() {
//    blinker->blinkFrame();

    Blinker.blinkFrame();
}