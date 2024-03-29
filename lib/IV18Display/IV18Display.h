#ifndef PIUCNTVFD1_IV18DISPLAY_H
#define PIUCNTVFD1_IV18DISPLAY_H

#include <Arduino.h>

/**
 * Low level control of the display.
 */
class IV18Display
{
public:
    static const int DIGIT_STEPS_COUNT = 9; // main grids + dot/minus
    static const byte MODE_CHARS = 0; // display chars from their buffer
    static const byte MODE_BYTES = 1; // display raw bytes from their buffer
    static const int GRID9_COOLDOWN_US = 100; // how long cooldown grid9 needs to avoid ghosting (it's separately multiplexed)

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

    // TODO: add capital letters?
    // char-to-segments dict
    // TODO 0 is actually ' '!
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
            0b00101110, //h
            0b00100000, //i
            0b01111000, //j
            0b00001110, //k //ugly
            0b00011100, //l
            0b10101010, //m //ugly
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
            0b01101110, //x //looks like h, ugly!
            0b01110110, //y
            0b11011010, //z //same as 2!
    };

    //TODO: these initial values are debug - will be set by command
    //minus works for idx 0
    //+1 because the string must contain the /0 ending
    char currentString[DIGIT_STEPS_COUNT + 1] = "-_-dong-_";
    //dot at idx 0 is the "big one"
    bool currentCommas[DIGIT_STEPS_COUNT] = {true, false, false, false, false, false, false, false, false};
    //bytes for custom mode
    byte currentBytes[DIGIT_STEPS_COUNT] = {0b00000000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000,
                                            0b00010000, 0b00010000, 0b00010000};

    byte currentMode = MODE_CHARS;

    static void setByte(byte val);
    static void setChar(char val);
    static void setComma(bool val);

    static void resetMultiplexingPulse(); //reset multiplexer

public:
    IV18Display();

    /**
     * Turns the display on
     */
    static void on();

    /**
     * Turns the display off
     */
    static void off();

    /**
     * Checks if display is on
     * @return bool
     */
    static bool isOn();

    /**
     * Sets the status LED to on
     */
    static void statusOn();

    /**
     * Sets the status LED to off
     */
    static void statusOff();

    /**
     * Lights up segments for a given digit, depending on current display mode (char or byte). Also turns on commas.
     *
     * @param sequenceNumber Digit number (counting from the right)
     */
    void prepareDigitSegments(int sequenceNumber);

    /**
     * Clears all segments
     */
    static void clearChar();

    /**
     * Lights up the next digit grid in the multiplexing cycle.
     *
     * @param isFirst Whether it's the first cycle (sends a new "1" to the shift register).
     */
    static void multiplexViaShiftRegister(bool isFirst = false);

    /**
     * Turns grid 9 off (it must be multiplexed separately)
     */
    static void Grid9Off();

    /**
     * Turns grid 9 on (it must be multiplexed separately)
     *
     * @param isGrid9
     */
    static void Grid9OnWhenNeeded(bool isGrid9 = false);

    /**
     * Sets current bytes (for byte display mode). It copies values from given source to the device's own buffer.
     *
     * @param bytesBuffer Source buffer (values are copied from there)
     */
    void setBytes(byte* bytesBuffer);

    /**
     * Sets current chars (for char display mode). It copies values from given source to the device's own buffer.
     *
     * @param charsBuffer Source buffer (values are copied from there)
     */
    void setChars(char const *charsBuffer);

    /**
     * Sets current commas. It copies values from given source to the device's own buffer.
     *
     * @param commasBuffer Source buffer (values are copied from there)
     */
    void setCommas(bool* commasBuffer);

    /**
     * Display mode (use MODE_BYTES or MODE_CHARS).
     *
     * @param mode
     */
    void setMode(byte mode);
};

extern IV18Display Display;

#endif //PIUCNTVFD1_IV18DISPLAY_H
