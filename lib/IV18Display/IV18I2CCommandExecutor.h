#ifndef PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H
#define PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H

#include "Arduino.h"
#include "IV18Animator.h"

class IV18Animator;

// TODO: amend nixie commands appropriately (the way they work + names)
class IV18I2CCommandExecutor
{
private:
    static const short BUNCHED_COMMANDS_BUFFER_LENGTH = 128;

    // instant commands
    static const byte CMD_OFF = 0x10; //turn the lamps off
    static const byte CMD_ON = 0x11; //turn the lamps on

    // TODO: define the current default animation from main.cpp as intro?
    static const byte CMD_INTRO_ON = 0x12; //turn the intro on

    // bunched commands
    static const byte CMD_MULTI_FIN = 0x20; //loads number, commas and dimming values from buffers to output (renders new frame)
    // TODO: maybe set duty cycle by separate byte for more precision?
    //       DECISION: yes
    static const byte CMD_MULTI_DIGIT_DIMMER = 0x60; // the FOLLOWING byte sets the duty cycle; last 4 bits determine which digit
    // TODO: do we want to set only chosen digits and retain old ones? or always set all 9? taking dimmer into account,
    //       maybe always set all or reset unset digits to space
    //       DECISION: reset unset ones to space (on FIN)
    static const byte CMD_MULTI_DIGIT_CHAR = 0x80; // the FOLLOWING byte sets char; last 4 bits determine which digit
    static const byte CMD_MULTI_DIGIT_BYTE = 0x90; // the FOLLOWING byte sets raw segments byte; last 4 bits determine which digit
    // TODO: do we want to reset commas every frame? or retain old commas and define a separate command to turn them off?
    //       DECISION: reset unset ones to no comma (on FIN)
    static const byte CMD_MULTI_DIGIT_POINT_R = 0xF0; // last 4 bits determine which right comma should be on
    static const byte CMD_MULTI_DIGIT_FADE_IN = 0xA0; // the FOLLOWING byte sets target duty cycle; last 4 bits determine which digit
    static const byte CMD_MULTI_DIGIT_FADE_OUT = 0xB0; // the FOLLOWING byte sets target duty cycle; last 4 bits determine which digit

    static byte bunchedCommandsBuffer[BUNCHED_COMMANDS_BUFFER_LENGTH];

public:
    // TODO: check if it's actually sufficient (no undertimes in typical scenarios, no led of course)
    static const short MAX_EXECUTION_TIME = 500;

    static void executeBufferedCommands(IV18Animator* animator, int sequenceNumber);
};

#endif //PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H
