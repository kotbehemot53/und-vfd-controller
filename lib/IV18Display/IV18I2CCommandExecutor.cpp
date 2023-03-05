#include "IV18I2CCommandExecutor.h"

#include "../I2CComms/I2CComms.h"

short IV18I2CCommandExecutor::bunchedCommandsCount = 0;
byte IV18I2CCommandExecutor::bunchedCommandsBuffer[IV18I2CCommandExecutor::BUNCHED_COMMANDS_BUFFER_LENGTH];

void IV18I2CCommandExecutor::executeBufferedCommands(IV18Animator *animator, int sequenceNumber)
{
    // TODO
    // TODO: buffer all commands that require a "FIN" to be executed (like setting chars, dimming, etc)
    //       IN THE HIGH LEVEL self::bunchedCommandsBuffer NOT LOW LEVEL I2CComms buffer!
    // TODO:
    //       pseudocode:
    //          1. I2CComms::resetBuffers()
    //          2. read all buffered commands & react:
    //              while (I2CComms::getReadBufferRemainingCommandCount()) {
    //                  I2CComms::getCommandFromReadBuffer() ...
    //              }
    //          2.1. if command read is instantaneous (incl. FIN) - just do what needs to be done
    //          2.3. if command read is bunchable - add it to bunched commands buffer

    I2CComms::resetBuffers();

    byte currentCommand;
    while (I2CComms::getReadBufferRemainingCommandCount()) {
        currentCommand = I2CComms::getCommandFromReadBuffer();

        if (isCommandBunchable(currentCommand)) {
            addCommandToBunchedBuffer(currentCommand);
        } else {
            executeCommand(animator, currentCommand);
        }
    }
}

void IV18I2CCommandExecutor::executeCommand(IV18Animator* animator, byte command)
{
    IV18Display* display = animator->getDisplay();

//    if (command == CMD_OFF) {
//        display->off();
//    }

    switch (command) {
        case CMD_OFF:
            display->off();
            break;
        case CMD_ON:
            display->on();
            break;

        // TODO...
    }
}

