#include "IV18Display.h"

IV18Display Display;

IV18Display::IV18Display()
{
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
}

void IV18Display::resetMultiplexingPulse()
{
    digitalWrite(MUX_MR, LOW);
    delayMicroseconds(SERIAL_REG_CLK_PULSE_LEN_US);
    digitalWrite(MUX_MR, HIGH);
}

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
    digitalWrite(SEGMENTS[7], val ? HIGH : LOW);
}

void IV18Display::clearChar()
{
    for (int i: SEGMENTS) {
        digitalWrite(i, LOW);
    }
}

void IV18Display::multiplexViaShiftRegister(bool isFirst)
{
    //send a single "1" to the shift register in the beginning of each cycle
    digitalWrite(MUX_DA, isFirst ? HIGH : LOW);

    //1 clock pulse to the shift register
    digitalWrite(MUX_CL, HIGH);
    delayMicroseconds(SERIAL_REG_CLK_PULSE_LEN_US);
    digitalWrite(MUX_CL, LOW);
}
void IV18Display::Grid9Off()
{
    digitalWrite(GRID9, LOW);
}

// TODO: maybe run it outside as an animation step, just like Grid9Off?
void IV18Display::Grid9OnWhenNeeded(bool isGrid9)
{
    if (isGrid9) {
        digitalWrite(GRID9, HIGH);
    }
}

void IV18Display::prepareDigitSegments(int sequenceNumber)
{
    int idx = DIGIT_STEPS_COUNT - (sequenceNumber + 1);

    if (currentMode == MODE_CHARS) {
        setChar(currentString[idx]);
    } else {
        setByte(currentBytes[idx]);
    }
    setComma(currentCommas[idx]);
}

void IV18Display::on()
{
    digitalWrite(HV_ENABLE, HIGH);
}

void IV18Display::off()
{
    digitalWrite(HV_ENABLE, LOW);
}

bool IV18Display::isOn()
{
    return digitalRead(HV_ENABLE);
}

void IV18Display::statusOn()
{
    digitalWrite(STATUS, HIGH);
}

void IV18Display::statusOff()
{
    digitalWrite(STATUS, LOW);
}

void IV18Display::setBytes(byte *bytesBuffer)
{
    memcpy(this->currentBytes, bytesBuffer, sizeof(this->currentBytes));
}

void IV18Display::setChars(char const *charsBuffer)
{
    memcpy(this->currentString, charsBuffer, sizeof(this->currentString));
}

void IV18Display::setCommas(bool *commasBuffer)
{
    memcpy(this->currentCommas, commasBuffer, sizeof(this->currentCommas));
}

void IV18Display::setMode(byte mode)
{
    this->currentMode = mode;
}
