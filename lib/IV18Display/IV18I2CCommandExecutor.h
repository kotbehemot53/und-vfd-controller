#ifndef PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H
#define PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H

#include "Arduino.h"
#include "IV18Animator.h"

class IV18Animator;

/**
 * Main I2C Command Executor. Gets buffered commands from I2CComms and executes them. Meant to be run by IV18Animator
 * in an appropriate section of each frame.
 */
class IV18I2CCommandExecutor
{
public:
    // instant commands
    static const byte CMD_OFF = 0x10; //turn the lamps off
    static const byte CMD_ON = 0x11; //turn the lamps on

    static const byte CMD_INTRO_ON = 0x12; //turn the intro on
    static const byte CMD_INTRO_OFF = 0x13; //turn the intro off

    static const byte CMD_MULTI_FINISH = 0x20; // sets up the next frame with all commands gathered in the bunched buffer

    // bunchable commands
    static const byte CMD_MULTI_DIGIT_CHAR = 0x80; // the FOLLOWING byte sets char; last 4 bits determine which digit; untouched chars will be off
    static const byte CMD_MULTI_DIGIT_BYTE = 0x90; // the FOLLOWING byte sets raw segments byte; last 4 bits determine which digit; untouched digits will be off
    static const byte CMD_MULTI_DIGIT_POINT_R = 0xF0; // last 4 bits determine which right comma should be on; untouched commas will be off
    static const byte CMD_MULTI_DIGIT_DIMMER = 0x60; // the FOLLOWING byte sets the duty cycle; last 4 bits determine which digit; digit > 9 sets all digits
    static const byte CMD_MULTI_DIGIT_FADE_IN = 0xA0; // the FOLLOWING byte sets target duty cycle; last 4 bits determine which digit; digit > 9 sets all digits
    static const byte CMD_MULTI_DIGIT_FADE_OUT = 0xB0; // the FOLLOWING byte sets target duty cycle; last 4 bits determine which digit; digit > 9 sets all digits
    static const byte CMD_MULTI_DIGIT_FADE_TIME = 0xC0; // the FOLLOWING byte sets target duration in frames; last 4 bits determine which digit; digit > 9 sets all digits
    static const byte CMD_MULTI_DIGIT_CLEAR = 0xD0; //clear all chars, bytes & commas, set chars mode

private:
    // TODO: why do strange things happen when buffer is above 511? even though there's still plenty of RAM?
    static const short BUNCHED_COMMANDS_BUFFER_LENGTH = 191;
    static const short BUNCHABLE_COMMANDS_COUNT = 8;
    static const short FOLLOWED_COMMANDS_COUNT = 6;

    // CMD_MULTI_FINISH not included - it acts immediately by dispatching all the bunched commands
    static constexpr byte BUNCHABLE_COMMANDS[BUNCHABLE_COMMANDS_COUNT] = {
        CMD_MULTI_DIGIT_DIMMER,
        CMD_MULTI_DIGIT_CHAR,
        CMD_MULTI_DIGIT_BYTE,
        CMD_MULTI_DIGIT_POINT_R,
        CMD_MULTI_DIGIT_FADE_IN,
        CMD_MULTI_DIGIT_FADE_OUT,
        CMD_MULTI_DIGIT_FADE_TIME,
        CMD_MULTI_DIGIT_CLEAR
    };

    static constexpr byte FOLLOWED_COMMANDS[FOLLOWED_COMMANDS_COUNT] = {
        CMD_MULTI_DIGIT_DIMMER,
        CMD_MULTI_DIGIT_CHAR,
        CMD_MULTI_DIGIT_BYTE,
        CMD_MULTI_DIGIT_FADE_IN,
        CMD_MULTI_DIGIT_FADE_OUT,
        CMD_MULTI_DIGIT_FADE_TIME
    };

    static byte bunchedCommandsBuffer[BUNCHED_COMMANDS_BUFFER_LENGTH];
    static short bunchedCommandsCount;

    inline static void addCommandToBunchedBuffer(byte command)
    {
        if (bunchedCommandsCount == BUNCHED_COMMANDS_BUFFER_LENGTH) {
            // TODO: throw exception or prevent overflowing the buffer?
            //       plain return for now
            return;
        }

        // add command to write buffer and increase the command count appropriately
        bunchedCommandsBuffer[bunchedCommandsCount++] = command;
    };

    inline static bool isCommandBunchable(byte command)
    {
        for (short i = 0; i < BUNCHABLE_COMMANDS_COUNT; ++i) {
            if (IV18I2CCommandExecutor::BUNCHABLE_COMMANDS[i] == (command & 0xF0)) {
                return true;
            }
        }

        return false;
    }
    inline static bool isCommandFollowed(byte command)
    {
        for (short i = 0; i < FOLLOWED_COMMANDS_COUNT; ++i) {
            if (IV18I2CCommandExecutor::FOLLOWED_COMMANDS[i] == (command & 0xF0)) {
                return true;
            }
        }

        return false;
    }

    static void executeCommand(IV18Animator* animator, byte command);

    static void executeBunchedCommands(IV18Animator* animator);

public:
    // TODO: check if it's actually sufficient (no undertimes in typical scenarios, no led of course)
    static const short MAX_EXECUTION_TIME = 500; // max execution time for the commands (used by the animator in the command step).

    /**
     * Execute all commands that have been buffered in I2CComms. Meant to be used as DeviceAnimatorStep callback,
     * as part of the lamp animation.
     *
     * @param animator
     * @param sequenceNumber
     */
    static void executeBufferedCommands(IV18Animator* animator, int sequenceNumber);
};

#endif //PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H
