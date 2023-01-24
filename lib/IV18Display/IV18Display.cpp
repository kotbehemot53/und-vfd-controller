#include "IV18Display.h"

IV18Display Display;

void IV18Display::setByte(byte val)
{
    //theoretically not necessary (if there's no error anywhere)
//    int arrSize = sizeof(SEGMENT_DICT)/sizeof(SEGMENT_DICT[0]);
//    if (idx >= 0 && idx < arrSize) {
    for (int i = 0; i < SEGMENT_CNT; i++) {
        digitalWrite(SEGMENTS[i], val & (0b10000000 >> i));
    }
//    }
}

void IV18Display::setChar(char val)
{
    int idx = 0; //default index - TODO: there should probably be a special "character" for it

    if (val >= '-' && val <= '9') {
        idx = val - '-' + 1;
    } else if (val >= '_' && val <= 'z') {
        idx = val - '_' + 14;
    }

    setByte(SEGMENT_DICT[idx]);
}

void IV18Display::setComma(bool val)
{
    digitalWrite(SEGMENTS[7], val & (0b00000001));
}

void IV18Display::clearChar()
{
    for (int i: SEGMENTS) {
        digitalWrite(i, LOW);
    }
}

void IV18Display::resetMultiplexingPulse()
{
    digitalWrite(MUX_MR, LOW);
    delayMicroseconds(SERIAL_REG_CLK_PULSE_LEN_US);
    digitalWrite(MUX_MR, HIGH);
}

void IV18Display::initMultiplexingPulse(byte subframeNumber)
{
    //send a single "1" to the shift register in the beginning of each cycle
    if ((subframeNumber % (SUBFRAMES_IN_CYCLE) == 0)) {
        digitalWrite(MUX_DA, HIGH);
    } else {
        digitalWrite(MUX_DA, LOW);
    }
}

void IV18Display::stepMultiplexingPulse(byte subframeNumber)
{
    //1 clock pulse to the shift register
    digitalWrite(MUX_CL, HIGH);
    delayMicroseconds(SERIAL_REG_CLK_PULSE_LEN_US);
    digitalWrite(MUX_CL, LOW);

}

void IV18Display::multiplexViaShiftRegister(byte subframeNumber)
{
    initMultiplexingPulse(subframeNumber);
    stepMultiplexingPulse(subframeNumber);
}

void IV18Display::initSubframe()
{
    currentSubframeStartUs = micros();
    grid9Begin(currentSubframeNumber);
}

void IV18Display::doSubframe(byte subframeNumber)
{
    byte idx = SUBFRAMES_IN_CYCLE - (subframeNumber + 2);

    // account for 1 "empty frame"
    if (idx >= 0) {
        if (currentMode == MODE_CHARS) {
            setChar(currentString[idx]);
        } else {
            setByte(currentBytes[idx]);
        }
        setComma(currentCommas[idx]);
    }

    // TODO: status led heartbeat?
}

void IV18Display::finishSubframe()
{
    unsigned int delayLength;

    currentSubframeNumber++;

    if (currentSubframeNumber >= SUBFRAMES_IN_CYCLE) {
        currentSubframeNumber = 0;
    }

    unsigned long currentSubframeEndUs = micros();
    //DEBUG MS instead of US
//    unsigned long frameEndMs = millis();

    //the frame is set up, now wait for the reminder of time.

    if ((currentSubframeNumber % SUBFRAMES_IN_CYCLE) ==
        SUBFRAMES_IN_CYCLE - 1) { //last empty frame for dot/minus afterglow
        delayLength = AFTER_GLOW_DELAY_US - (currentSubframeEndUs - currentSubframeStartUs);
    } else {
        delayLength = subframeDurationUs - (currentSubframeEndUs - currentSubframeStartUs);
    }
    //DEBUG MS instead of US
//    unsigned int delayLength = FRAME_DURATION_US - (frameEndMs - frameStartMs);

    //delay debug
//    if (delayLength < 100) {
//        Serial.print("Delay length ");
//        Serial.println(delayLength);
//    }

    delayMicroseconds(delayLength > 0 ? delayLength : 1);
    //DEBUG MS instead of US
//    delay(delayLength > 0 ? delayLength : 1);

    //here a new frame really begins - we clear the character
    clearChar();

    multiplexViaShiftRegister(currentSubframeNumber);

    grid9End(currentSubframeNumber);
}

void IV18Display::grid9Begin(byte subframeNumber)
{
    if ((subframeNumber % (SUBFRAMES_IN_CYCLE) == 9)) {
        digitalWrite(GRID9, LOW);
    }
}

void IV18Display::grid9End(byte subframeNumber)
{
    //set GRID9 if we reached the last frames (they are cycled right to left)
    if ((subframeNumber % (SUBFRAMES_IN_CYCLE) == 8)) {
        digitalWrite(GRID9, HIGH);
    }
}

void IV18Display::init(unsigned long frameDurationUs)
{
    //TODO: calc subframeDurationUs from frameDurationUs
    subframeDurationUs = 1000;

    pinMode(MUX_DA, OUTPUT);
    digitalWrite(MUX_DA, LOW);
    pinMode(MUX_CL, OUTPUT);
    digitalWrite(MUX_CL, LOW);
    pinMode(MUX_MR, OUTPUT);
    digitalWrite(MUX_MR, LOW);
    pinMode(HV_ENABLE, OUTPUT);
    digitalWrite(HV_ENABLE, LOW);
    pinMode(STATUS, OUTPUT);
    digitalWrite(STATUS, LOW);

    for (int i = 0; i < SEGMENT_CNT; i++) {
        pinMode(SEGMENTS[i], OUTPUT);
        digitalWrite(SEGMENTS[i], HIGH);
    }

    pinMode(GRID9, OUTPUT);
    digitalWrite(GRID9, LOW);

    resetMultiplexingPulse();

    // TODO: make a nice heartbeat or sth
    digitalWrite(STATUS, HIGH);
}

void IV18Display::doFrame()
{
    for (byte i = 0; i < SUBFRAMES_IN_CYCLE; i++) {
        initSubframe();

        doSubframe(currentSubframeNumber);

        finishSubframe();
    }
}

void IV18Display::on()
{
    digitalWrite(HV_ENABLE, HIGH);
}

void IV18Display::off()
{
    digitalWrite(HV_ENABLE, LOW);
}
