#include <Arduino.h>

const int SEGMENT_CNT = 8;
const unsigned int FRAME_DURATION_US = 1000;
//const unsigned int AFTER_GLOW_DELAY_US = 10; //TODO: seems unnecessary
const int FRAMES_IN_CYCLE = SEGMENT_CNT;
const int SERIAL_REG_CLK_PULSE_LEN_US = 100;   //clock/reset pulse length for the shift registers

const int MUX_DA = 2;
const int MUX_CL = 3;
const int MUX_MR = 4;
const int SEGMENTS[SEGMENT_CNT] = {5, 6, 7, 8, 9, 10, 11, 12};

//const char ALPHABET[] = { //TODO
//    '0',
//    '1',
//    '2',
//    '3',
//    '4',
//    '5',
//    '6',
//    '7',
//    '8',
//    '9'
//};
const bool SEGMENT_DICT[][8] = { //TODO
    {true, true, true, true, true, true, false, false},
    {false, true, true, false, false, false, false, false},
    {true, true, false, true, true, false, true, false},
    {true, true, true, true, false ,false, true, false},
    {false, true, true, false, false, true, true, false},
    {true, false, true, true, false, true, true, false},
    {true, false, true, true, true, true, true, false},
    {true, true, true, false, false, false, false, false},
    {true, true, true, true, true, true, true, false},
    {true, true, true, true, false, true, true, false}
};

int frame = 0;
unsigned long frameStartUs = 0;
unsigned long frameStartMs = 0;

//TODO: nicer implementation with letters!
void setChar(char val) {
    int idx = val - '0';
    int arrSize = sizeof(SEGMENT_DICT)/sizeof(SEGMENT_DICT[0]);

    if (idx >= 0 && idx < arrSize) {
        for (int i = 0; i < SEGMENT_CNT; i++) {
            digitalWrite(SEGMENTS[i], SEGMENT_DICT[idx][i]);
        }
    }
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
//    //stop displaying points for player when all are displayed (clear the shift register)
//    if (frameNumber % (SEGMENT_CNT) >= playerScore) {
//        resetScoreTrackWalkingPulse(playerIdx);
//    }

    //send a single "1" to the shift register in the beginning of each cycle
    if ((frameNumber % (SEGMENT_CNT) == 0)) {
        digitalWrite(MUX_DA, HIGH);
    } else {
        digitalWrite(MUX_DA, LOW);
    }

    //1 clock pulse to the shift register
    stepMultiplexingPulse();
}

void initFrame() {
    frameStartUs = micros();
    frameStartMs = millis();
}

void frameUp() {
    frame++;

    if (frame >= FRAMES_IN_CYCLE) {
        frame = 0;
    }

    unsigned long frameEndUs = micros();
//    unsigned long frameEndMs = millis();

    unsigned int delayLength = FRAME_DURATION_US - (frameEndUs - frameStartUs);
    //debug
//    if (delayLength < 100) {
//        Serial.print("Delay length ");
//        Serial.println(delayLength);
//    }
    delayMicroseconds(delayLength > 0 ? delayLength : 1);

    //DEBUG MS instead of US
//    unsigned int delayLength = FRAME_DURATION_US - (frameEndMs - frameStartMs);
//    //debug
////    if (delayLength < 100) {
////        Serial.print("Delay length ");
////        Serial.println(delayLength);
////    }
//    delay(delayLength > 0 ? delayLength : 1);


    clearChar();
    //TODO: seems unnecessary
//    delayMicroseconds(AFTER_GLOW_DELAY_US);

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

    for (int i = 0; i < SEGMENT_CNT; i++) {
        pinMode(SEGMENTS[i], OUTPUT);
        digitalWrite(SEGMENTS[i], HIGH);
    }

    resetMultiplexingPulse();

//    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    initFrame();
//    digitalWrite(LED_BUILTIN, HIGH);
//    delay(500);
//    digitalWrite(LED_BUILTIN, LOW);
//    delay(500);

//    clearChar();
//    delayMicroseconds(AFTER_GLOW_DELAY_US);
    setChar(intToChar(frame + 1));

    frameUp();
}