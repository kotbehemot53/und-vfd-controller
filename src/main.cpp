#include <Arduino.h>

//TODO: PWM brightness
//TODO: I2C (?) + COMMANDS
//TODO: ability to send custom array of bytes representing raw segments, not ASCII characters
//TODO: support small dots (set last bit to 1 based on a separate array & the output of of regular SEGMENT_DICT?)
//TODO: support big dot/minus (frames + 1 in frameUp, multiplexViaShiftRegister, ...?)
//TODO: fix SEGMENT_CNT (number of SMALL segments) != FRAMES_IN_CYCLE (number of grids)!!

const int SEGMENT_CNT = 8;
const unsigned int FRAME_DURATION_US = 1000;
//const unsigned int AFTER_GLOW_DELAY_US = 10; //TODO: seems unnecessary
const int FRAMES_IN_CYCLE = SEGMENT_CNT;
const int SERIAL_REG_CLK_PULSE_LEN_US = 100;   //clock/reset pulse length for the shift registers

const int MUX_DA = 2;
const int MUX_CL = 3;
const int MUX_MR = 4;
const int SEGMENTS[SEGMENT_CNT] = {5, 6, 7, 8, 9, 10, 11, 12};
const byte HV_ENABLE = A0;
const byte STATUS = A3;

//each bit is one segment in order: abcdefg.
const byte SEGMENT_DICT[] = { //TODO
    0b00000000, //0
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
char currentString[SEGMENT_CNT+1] = "asscock8";
bool currentCommas[SEGMENT_CNT+1] = {true, true, true, false, false, true, true, true};

int frame = 0;
unsigned long frameStartUs = 0;
unsigned long frameStartMs = 0;


void setChar(char val) {
    int idx = 0; //default index - TODO: there should probably be a special "character" for it
    if (val >= '0' && val <= '9') {
        idx = val - '0';
    }
    if (val >= '_' && val <= 'z') {
        idx = val - '_' + 10;
    }
    //theoretically not necessary (if there's no error anywhere)
//    int arrSize = sizeof(SEGMENT_DICT)/sizeof(SEGMENT_DICT[0]);
//    if (idx >= 0 && idx < arrSize) {
        for (int i = 0; i < SEGMENT_CNT; i++) {
            digitalWrite(SEGMENTS[i], SEGMENT_DICT[idx] & (0b10000000 >> i));
        }
//    }
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
    pinMode(HV_ENABLE, OUTPUT);
    digitalWrite(HV_ENABLE, LOW);
    pinMode(STATUS, OUTPUT);
    digitalWrite(STATUS, LOW);

    for (int i = 0; i < SEGMENT_CNT; i++) {
        pinMode(SEGMENTS[i], OUTPUT);
        digitalWrite(SEGMENTS[i], HIGH);
    }

    resetMultiplexingPulse();

    delay(100);
    digitalWrite(HV_ENABLE, HIGH);
    digitalWrite(STATUS, HIGH);

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

    //TODO: this is debug
//    setChar('8');
    setChar(currentString[FRAMES_IN_CYCLE - (frame + 1)]);//intToChar(frame + 1));
    setComma(currentCommas[FRAMES_IN_CYCLE - (frame + 1)]);

    frameUp();
}