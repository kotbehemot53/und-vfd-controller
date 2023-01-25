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

void IV18Display::initMultiplexingPulse(bool isFirst)
{
    //send a single "1" to the shift register in the beginning of each cycle
    if (isFirst) {
        digitalWrite(MUX_DA, HIGH);
    } else {
        digitalWrite(MUX_DA, LOW);
    }
}

void IV18Display::stepMultiplexingPulse()
{
    //1 clock pulse to the shift register
    digitalWrite(MUX_CL, HIGH);
    delayMicroseconds(SERIAL_REG_CLK_PULSE_LEN_US);
    digitalWrite(MUX_CL, LOW);

}

void IV18Display::multiplexViaShiftRegister(bool isFirst)
{
    initMultiplexingPulse(isFirst);
    stepMultiplexingPulse();
}

void IV18Display::multiplexGrid9(int sequenceNumber)
{
    digitalWrite(GRID9, sequenceNumber == 8 ? HIGH : LOW);
}



//void IV18Display::initSubframe()
//{
//    currentSubframeStartUs = micros();
//    grid9Begin(currentSubframeNumber);
//}
//
//void IV18Display::doSubframe(byte subframeNumber)
//{
//    byte idx = GRID_STEPS_COUNT - (subframeNumber + 2);
//
//    // account for 1 "empty frame"
//    if (idx >= 0) {
//        if (currentMode == MODE_CHARS) {
//            setChar(currentString[idx]);
//        } else {
//            setByte(currentBytes[idx]);
//        }
//        setComma(currentCommas[idx]);
//    }
//
//    // TODO: status led heartbeat?
//}
//
//void IV18Display::finishSubframe()
//{
//    unsigned int delayLength;
//
//    currentSubframeNumber++;
//
//    if (currentSubframeNumber >= GRID_STEPS_COUNT) {
//        currentSubframeNumber = 0;
//    }
//
//    unsigned long currentSubframeEndUs = micros();
//    //DEBUG MS instead of US
////    unsigned long frameEndMs = millis();
//
//    //the frame is set up, now wait for the reminder of time.
//
//    if ((currentSubframeNumber % GRID_STEPS_COUNT) ==
//        GRID_STEPS_COUNT - 1) { //last empty frame for dot/minus afterglow
//        delayLength = AFTER_GLOW_DELAY_US - (currentSubframeEndUs - currentSubframeStartUs);
//    } else {
//        delayLength = subframeDurationUs - (currentSubframeEndUs - currentSubframeStartUs);
//    }
//    //DEBUG MS instead of US
////    unsigned int delayLength = FRAME_DURATION_US - (frameEndMs - frameStartMs);
//
//    //delay debug
////    if (delayLength < 100) {
////        Serial.print("Delay length ");
////        Serial.println(delayLength);
////    }
//
//    delayMicroseconds(delayLength > 0 ? delayLength : 1);
//    //DEBUG MS instead of US
////    delay(delayLength > 0 ? delayLength : 1);
//
//    //here a new frame really begins - we clear the character
//    clearChar();
//
//    multiplexViaShiftRegister(currentSubframeNumber);
//
//    grid9End(currentSubframeNumber);
//}

//void IV18Display::initGridStep()
//{
//    // TODO: set from animator?
//    currentSubframeStartUs = micros();
//    grid9Begin();
//}

void IV18Display::setGridSegments(int sequenceNumber)
{
    byte idx = GRID_STEPS_COUNT - (sequenceNumber + 2);

    // account for 1 "empty frame"
    if (idx >= 0) {
        if (currentMode == MODE_CHARS) {
            setChar(currentString[idx]);
        } else {
            setByte(currentBytes[idx]);
        }
        setComma(currentCommas[idx]);
    }
}

//void IV18Display::finishGridStep()
//{
//    unsigned int delayLength;
//
//    currentSubframeNumber++;
//
//    if (currentSubframeNumber >= GRID_STEPS_COUNT) {
//        currentSubframeNumber = 0;
//    }
//
//    unsigned long currentSubframeEndUs = micros();
//    //DEBUG MS instead of US
////    unsigned long frameEndMs = millis();
//
//    //the frame is set up, now wait for the reminder of time.
//
//    // TODO: move these to animator?
//    if ((currentSubframeNumber % GRID_STEPS_COUNT) ==
//        GRID_STEPS_COUNT - 1) { //last empty frame for dot/minus afterglow
//        delayLength = AFTER_GLOW_DELAY_US - (currentSubframeEndUs - currentSubframeStartUs);
//    } else {
//        delayLength = subframeDurationUs - (currentSubframeEndUs - currentSubframeStartUs);
//    }
//    //DEBUG MS instead of US
////    unsigned int delayLength = FRAME_DURATION_US - (frameEndMs - frameStartMs);
//
//    //delay debug
////    if (delayLength < 100) {
////        Serial.print("Delay length ");
////        Serial.println(delayLength);
////    }
//
//    delayMicroseconds(delayLength > 0 ? delayLength : 1);
//    //DEBUG MS instead of US
////    delay(delayLength > 0 ? delayLength : 1);
//
//    //here a new frame really begins - we clear the character
//    clearChar();
//
//    multiplexViaShiftRegister();
//
//    grid9End();
//}

//void IV18Display::grid9Begin(int sequenceNumber)
//{
//    if ((sequenceNumber % (GRID_STEPS_COUNT) == 9)) {
//        digitalWrite(GRID9, LOW);
//    }
//}
//
//void IV18Display::grid9End(int sequenceNumber)
//{
//    //set GRID9 if we reached the last frames (they are cycled right to left)
//    if ((sequenceNumber % (GRID_STEPS_COUNT) == 8)) {
//        digitalWrite(GRID9, HIGH);
//    }
//}



void IV18Display::init()
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
//    digitalWrite(STATUS, HIGH);
}

void IV18Display::on()
{
    digitalWrite(HV_ENABLE, HIGH);
}

void IV18Display::off()
{
    digitalWrite(HV_ENABLE, LOW);
}

//void IV18Display::doFrame()
//{
//    for (byte i = 0; i < GRID_STEPS_COUNT; i++) {
//        initSubframe();
//
//        doSubframe(currentSubframeNumber);
//
//        finishSubframe();
//    }
//}

void IV18Display::doGridStep(IV18Display* that, int sequenceNumber)
{
//    if (sequenceNumber == 9) {
//        digitalWrite(GRID9, HIGH);
//    }
    that->setGridSegments(sequenceNumber);
}

void IV18Display::prepareNextGridStep(IV18Display* that, int sequenceNumber)
{
    that->clearChar();
    that->multiplexViaShiftRegister(sequenceNumber == 0);
    that->multiplexGrid9(sequenceNumber);
}

void IV18Display::statusOn(IV18Display* that, int sequenceNumber)
{
    digitalWrite(STATUS, HIGH);
}

void IV18Display::statusOff(IV18Display* that, int sequenceNumber)
{
    digitalWrite(STATUS, LOW);
}

void IV18Display::noOp(IV18Display *that, int sequenceNumber)
{
    // does nothing
}
