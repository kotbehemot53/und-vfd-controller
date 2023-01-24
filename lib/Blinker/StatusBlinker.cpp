#include "StatusBlinker.h"

//Blinker::Blinker()
//{
//    pinMode(MUX_DA, OUTPUT);
//    digitalWrite(MUX_DA, LOW);
//    pinMode(MUX_CL, OUTPUT);
//    digitalWrite(MUX_CL, LOW);
//    pinMode(MUX_MR, OUTPUT);
//    digitalWrite(MUX_MR, LOW);
//    pinMode(HV_ENABLE, OUTPUT);
//    digitalWrite(HV_ENABLE, LOW);
//
//    pinMode(BLINK_OUTPUT, OUTPUT);
//    digitalWrite(BLINK_OUTPUT, LOW);
//}

StatusBlinker Blinker;

bool StatusBlinker::isInitialised()
{
    return this->initialised;
}

void StatusBlinker::init()
{
    pinMode(MUX_DA, OUTPUT);
    digitalWrite(MUX_DA, LOW);
    pinMode(MUX_CL, OUTPUT);
    digitalWrite(MUX_CL, LOW);
    pinMode(MUX_MR, OUTPUT);
    digitalWrite(MUX_MR, LOW);
    pinMode(HV_ENABLE, OUTPUT);
    digitalWrite(HV_ENABLE, LOW);

    pinMode(BLINK_OUTPUT, OUTPUT);
    digitalWrite(BLINK_OUTPUT, LOW);

    this->initialised = true;
}

void StatusBlinker::blinkFrame()
{
    digitalWrite(BLINK_OUTPUT, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(BLINK_OUTPUT, LOW);
    delay(BLINK_DELAY);
}