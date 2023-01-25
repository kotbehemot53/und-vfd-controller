#ifndef PIUCNTVFD1_IV18DISPLAY_H
#define PIUCNTVFD1_IV18DISPLAY_H

#include <Arduino.h>

class IV18Display
{
public:
    static const int GRID_STEPS_COUNT = 10; //main grids + dot/minus
    static const byte MODE_CHARS = 0;
    static const byte MODE_BYTES = 1;

private:
    // grids/subframes count
    static const int SEGMENT_CNT = 8;

    // output/input pins
    static const int MUX_DA = 2;
    static const int MUX_CL = 3;
    static const int MUX_MR = 4;
    static constexpr int SEGMENTS[SEGMENT_CNT] = {5, 6, 7, 8, 9, 10, 11, 12};
    static const byte GRID9 = 13;
    static const byte HV_ENABLE = A0;
    static const byte STATUS = A3;

    // timing
    static const unsigned short SERIAL_REG_CLK_PULSE_LEN_US = 50;

    // char-to-segments dict
    //TODO 0 is actually ' '!
    //each bit is one segment in order: abcdefg.
    static constexpr byte SEGMENT_DICT[] = {
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
    //minus works for idx 0
    //+1 because the string must contain the /0 ending
    char currentString[GRID_STEPS_COUNT] = "-asscock7";
    //dot at idx 0 is the "big one"
    bool currentCommas[GRID_STEPS_COUNT - 1] = {false, false, false, false, false, false, false, false, false};
    //bytes for custom mode
    byte currentBytes[GRID_STEPS_COUNT - 1] = {0b00000000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000,
                                               0b00010000, 0b00010000, 0b00010000};

    byte currentMode = MODE_CHARS;

    static void setByte(byte val);
    static void setChar(char val);
    static void setComma(bool val);

    static void resetMultiplexingPulse(); //reset multiplexer

public:
    IV18Display();

    static void on();
    static void off();
    static void statusOn();
    static void statusOff();

    void prepareGridSegments(int sequenceNumber);
    static void clearChar();
    static void multiplexViaShiftRegister(bool isFirst = false);
    static void multiplexGrid9(bool isGrid9 = false);

    // TODO:
//    // TODO: remember to append first empty value to those passed or introduce the empty frame otherwise
//    void setBytes(byte* bytes); // TODO: how to ensure length of 8? enable exceptions?
//    void setChars(char* chars); // TODO: how to ensure length of 8? enable exceptions?
//    void setCommas(bool* commas); // TODO: how to ensure length of 8? enable exceptions?
//    void setMode(byte mode);
};

extern IV18Display Display;

#endif //PIUCNTVFD1_IV18DISPLAY_H
