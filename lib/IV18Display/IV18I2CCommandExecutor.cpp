#include "IV18I2CCommandExecutor.h"

#include "../I2CComms/I2CComms.h"
#include "IV18IntroSequencer.h"
#include "IV18Display.h"

short IV18I2CCommandExecutor::bunchedCommandsCount = 0;
byte IV18I2CCommandExecutor::bunchedCommandsBuffer[IV18I2CCommandExecutor::BUNCHED_COMMANDS_BUFFER_LENGTH];

void IV18I2CCommandExecutor::executeBufferedCommands(IV18Animator *animator, int sequenceNumber)
{
    bool bufferFollowerCommand = false;

    I2CComms::resetBuffers();

    byte currentCommand;
    while (I2CComms::getReadBufferRemainingCommandCount()) {
        currentCommand = I2CComms::getCommandFromReadBuffer();

        if (bufferFollowerCommand) {
            addCommandToBunchedBuffer(currentCommand);
            bufferFollowerCommand = false;
            continue;
        }

        if (isCommandBunchable(currentCommand)) {
            addCommandToBunchedBuffer(currentCommand);
            if (isCommandFollowed(currentCommand)) {
                bufferFollowerCommand = true;
            }
        } else {
            executeCommand(animator, currentCommand);
        }
    }
}

void IV18I2CCommandExecutor::executeBunchedCommands(IV18Animator *animator)
{
    short lampDigitNumber;
    byte commandByte;

    bool setBytes = false;
    bool setChars = false;
    bool setCommas = false;

    byte bytesBuffer[IV18Display::DIGIT_STEPS_COUNT] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    char charsBuffer[IV18Display::DIGIT_STEPS_COUNT + 1] = "         ";
    bool commasBuffer[IV18Display::DIGIT_STEPS_COUNT] = {false,false,false,false,false,false,false,false,false};

    // TODO: throw exception/handle improper commands count (above buffer or not including supplementary commands with values?)
    for (short i = 0; i < bunchedCommandsCount; ++i) {
        commandByte = bunchedCommandsBuffer[i];
        switch (commandByte & 0xF0) { //mask out the last 4 bits
            case CMD_MULTI_DIGIT_CHAR:
                setChars = true;
                charsBuffer[commandByte & 0x0F] = (char) bunchedCommandsBuffer[++i];
                break;
            case CMD_MULTI_DIGIT_BYTE:
                setBytes = true;
                bytesBuffer[commandByte & 0x0F] = bunchedCommandsBuffer[++i];
                break;
            case CMD_MULTI_DIGIT_POINT_R:
                setCommas = true;
                commasBuffer[commandByte & 0x0F] = true;
                break;
            case CMD_MULTI_DIGIT_DIMMER:
                animator->setCurrentLampDigitDutyValue(
                    commandByte & 0x0F,
                    IV18Animator::convertDutyCycle(bunchedCommandsBuffer[++i])
                );
                break;
            case CMD_MULTI_DIGIT_FADE_IN:
                lampDigitNumber = commandByte & 0x0F;
                animator->setLampDigitAction(
                    lampDigitNumber,
                    IV18Animator::LAMP_DIGIT_IN,
                    IV18Animator::convertDutyCycle(bunchedCommandsBuffer[++i]),
                    animator->getLampDigitPreviousOnDutyUs(lampDigitNumber)
                );
                break;
            case CMD_MULTI_DIGIT_FADE_OUT:
                lampDigitNumber = commandByte & 0x0F;
                animator->setLampDigitAction(
                    lampDigitNumber,
                    IV18Animator::LAMP_DIGIT_OUT,
                    animator->getLampDigitPreviousOnDutyUs(lampDigitNumber),
                    IV18Animator::convertDutyCycle(bunchedCommandsBuffer[++i])
                );
                break;
            case CMD_MULTI_DIGIT_FADE_TIME:
                animator->setLampDigitFramesPerAction(commandByte & 0x0F, bunchedCommandsBuffer[++i]);
                break;
            default:
                // TODO: throw exception for unsupported commands?
                break;
        }
    }

    IV18Display* display = animator->getDisplay();
    if (setBytes) {
        display->setBytes(bytesBuffer);
        display->setMode(IV18Display::MODE_BYTES);
    }
    if (setChars) {
        display->setChars(charsBuffer);
        display->setMode(IV18Display::MODE_CHARS);
    }
    if (setCommas) {
        display->setCommas(commasBuffer);
    }

    bunchedCommandsCount = 0;
}

void IV18I2CCommandExecutor::executeCommand(IV18Animator* animator, byte command)
{
    IV18Display* display = animator->getDisplay();

    switch (command) {
        case CMD_OFF:
            display->off();
            break;
        case CMD_ON:
            display->on();
            break;
        case CMD_INTRO_ON:
            animator->setSequencingCallback(IV18IntroSequencer::doIntroSequencing);
            break;
        case CMD_INTRO_OFF:
            animator->disableSequencing();
            break;
        case CMD_MULTI_FINISH:
            IV18I2CCommandExecutor::executeBunchedCommands(animator);
            break;
        default:
            // TODO: throw exception for unsupported commands?
            break;
    }
}

