#include <Arduino.h>

const int SEGMENT_CNT = 8;
const unsigned int FRAME_DURATION_US = 1000;
const int FRAMES_IN_CYCLE = SEGMENT_CNT;
const int SERIAL_REG_CLK_PULSE_LEN_US = 100;   //clock/reset pulse length for the shift registers

const int MUX_DA = 2;
const int MUX_CL = 3;
const int MUX_MR = 4;
const int SEGMENTS[SEGMENT_CNT] = {5, 6, 7, 8, 9, 10, 11, 12};

int frame = 0;
unsigned long frameStartUs = 0;
unsigned long frameStartMs = 0;

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

void setup() {

    pinMode(MUX_DA, OUTPUT);
    digitalWrite(MUX_DA, LOW);
    pinMode(MUX_CL, OUTPUT);
    digitalWrite(MUX_CL, LOW);
    pinMode(MUX_MR, OUTPUT);
    digitalWrite(MUX_MR, LOW);

    for (int i = 0; i < SEGMENT_CNT; i++) {
        pinMode(SEGMENTS[i], OUTPUT);
        digitalWrite(SEGMENTS[i], LOW);
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

    multiplexViaShiftRegister(frame);

    frameUp();
}