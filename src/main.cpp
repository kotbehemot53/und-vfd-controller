#include <Arduino.h>

// DEBUG:
//#include "avr8-stub.h"
//#include "app_api.h" // only needed with flash breakpoints

//TODO: PWM brightness
//TODO: I2C (?) + COMMANDS

const int SEGMENT_CNT = 8;
const unsigned int FRAME_DURATION_US = 1000; //default 1000
const unsigned int AFTER_GLOW_DELAY_US = 200; //it's only needed because the GRID9 optocoupler goes off with a noticeable delay
const int FRAMES_IN_CYCLE = 10; //main grids + dot/minus
const int SERIAL_REG_CLK_PULSE_LEN_US = 100;   //clock/reset pulse length for the shift registers

const int MUX_DA = 2;
const int MUX_CL = 3;
const int MUX_MR = 4;
const int SEGMENTS[SEGMENT_CNT] = {5, 6, 7, 8, 9, 10, 11, 12};
const byte GRID9 = 13;

const byte HV_ENABLE = A0;
const byte STATUS = A3;

const byte MODE_CHARS = 0;
const byte MODE_BYTES = 1;

//TODO 0 is actually ' '!
//each bit is one segment in order: abcdefg.
const byte SEGMENT_DICT[] = {
    0b00000000, // //empty

    0b00000010, //-

    0b00000000, //placeholder
    0b00000000, //placeholder

    0b11111100, //0
    0b01100000, //1
    0b11011010, //2
    0b11110010, //3
    0b01100110, //4
    0b10110110, //5
    0b10111110, //6
    0b11100000, //7
    0b11111110, //8
    0b11110110, //9

    0b00010000, //_
    0b10000000, //` sort of
    0b11101110, //a
    0b00111110, //b
    0b00011010, //c
    0b01111010, //d
    0b10011110, //e
    0b10001110, //f
    0b10111100, //g
    0b01101110, //h
    0b00100000, //i
    0b01111000, //j
    0b00001110, //k //ugly
    0b00011100, //l
    0b00101010, //m //ugly & same as n!
    0b00101010, //n
    0b00111010, //o
    0b11001110, //p
    0b11100110, //q
    0b00001010, //r
    0b10110110, //s //same as 5!
    0b11100000, //t //ugly
    0b01111100, //u
    0b01111100, //v //same as u, w ugly!
    0b01111100, //w //same as u, v ugly!
    0b01101110, //x //same as h, ugly!
    0b01110110, //y
    0b11011010, //z //same as 2!
};

//TODO: these initial values are debug - will be set by command
//+1 because the string must contain the /0 ending
//first must be empty! (empty frame after dot/minus)
//minus works for idx 1
char currentString[FRAMES_IN_CYCLE+1] = "  asscock6";
//first must be empty! (empty frame after dot/minus)
//dot at idx 1 is the "big one"
bool currentCommas[FRAMES_IN_CYCLE] = {false, true, false, false, false, false, false, false, false, true};
//bytes for custom mode
//first must be empty! (empty frame after dot/minus)
byte currentBytes[FRAMES_IN_CYCLE+1] = {0b00000000, 0b00000000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000};

int frame = 0;
unsigned long frameStartUs = 0;
unsigned long frameStartMs = 0;

byte currentMode = MODE_BYTES;

void setChar(char val) {
    int idx = 0; //default index - TODO: there should probably be a special "character" for it

    if (val >= '-' && val <= '9') {
        idx = val - '-' + 1;
    }
    else if (val >= '_' && val <= 'z') {
        idx = val - '_' + 14;
    }
    //theoretically not necessary (if there's no error anywhere)
//    int arrSize = sizeof(SEGMENT_DICT)/sizeof(SEGMENT_DICT[0]);
//    if (idx >= 0 && idx < arrSize) {
        for (int i = 0; i < SEGMENT_CNT; i++) {
            digitalWrite(SEGMENTS[i], SEGMENT_DICT[idx] & (0b10000000 >> i));
        }
//    }
}

void setByte(byte val) {
    for (int i = 0; i < SEGMENT_CNT; i++) {
        digitalWrite(SEGMENTS[i], val & (0b10000000 >> i));
    }
}

void setComma(bool val) {
    digitalWrite(SEGMENTS[7], val & (0b00000001));
}

void clearChar() {
    for (int i : SEGMENTS) {
        digitalWrite(i, LOW);
    }
}

void resetMultiplexingPulse() {
    digitalWrite(MUX_MR, LOW);
    delayMicroseconds(SERIAL_REG_CLK_PULSE_LEN_US);
    digitalWrite(MUX_MR, HIGH);
}

void stepMultiplexingPulse() {
    digitalWrite(MUX_CL, HIGH);
    delayMicroseconds(SERIAL_REG_CLK_PULSE_LEN_US);
    digitalWrite(MUX_CL, LOW);
}

void multiplexViaShiftRegister(int frameNumber) {
    //send a single "1" to the shift register in the beginning of each cycle
    if ((frameNumber % (FRAMES_IN_CYCLE) == 0)) {
        digitalWrite(MUX_DA, HIGH);
    } else {
        digitalWrite(MUX_DA, LOW);
    }

    //1 clock pulse to the shift register
    stepMultiplexingPulse();
    //set GRID9 if we reached the last frames (they are cycled right to left)
    if ((frameNumber % (FRAMES_IN_CYCLE) == 8)) {
        digitalWrite(GRID9, HIGH);
    }
}

void initFrame() {
    frameStartUs = micros();
    frameStartMs = millis();
}

void frameUp() {
    unsigned int delayLength;

    frame++;

    if (frame >= FRAMES_IN_CYCLE) {
        frame = 0;
    }

    unsigned long frameEndUs = micros();
    //DEBUG MS instead of US
//    unsigned long frameEndMs = millis();

    //the frame is set up, now wait for the reminder of time.

    if ((frame % FRAMES_IN_CYCLE) == FRAMES_IN_CYCLE - 1) { //last empty frame for dot/minus afterglow
        delayLength = AFTER_GLOW_DELAY_US - (frameEndUs - frameStartUs);
    } else {
        delayLength = FRAME_DURATION_US - (frameEndUs - frameStartUs);
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

    multiplexViaShiftRegister(frame);
}


char intToChar (int val) {
    return '0' + val;
}

void setup() {

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

    delay(100);
    digitalWrite(HV_ENABLE, HIGH);
    digitalWrite(STATUS, HIGH);

//    Serial.begin(115200);
//    Serial.println("yo");
}

void loop() {
    initFrame();

    if ((frame % (FRAMES_IN_CYCLE) == 9)) {
        digitalWrite(GRID9, LOW);
    }

    if (currentMode == MODE_CHARS) {
        setChar(currentString[FRAMES_IN_CYCLE - (frame + 1)]);//intToChar(frame + 1));
    } else {
        setByte(currentBytes[FRAMES_IN_CYCLE - (frame + 1)]);
    }
    setComma(currentCommas[FRAMES_IN_CYCLE - (frame + 1)]);

    frameUp();
}