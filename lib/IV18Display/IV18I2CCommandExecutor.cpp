#include "IV18I2CCommandExecutor.h"

#include "../I2CComms/I2CComms.h"
#include "IV18IntroSequencer.h"
#include "IV18Display.h"

short IV18I2CCommandExecutor::bunchedCommandsCount = 0;
byte IV18I2CCommandExecutor::bunchedCommandsBuffer[IV18I2CCommandExecutor::BUNCHED_COMMANDS_BUFFER_LENGTH];
bool IV18I2CCommandExecutor::awaitingBunchedCommandFollower = false;

void IV18I2CCommandExecutor::executeBufferedCommands(IV18Animator *animator, int sequenceNumber)
{
    I2CComms::disableReceiving();
    I2CComms::resetBuffers(); // TODO: this is wrong? when we're mid-bunch - what then?

    byte currentCommand;
    while (I2CComms::getReadBufferRemainingCommandCount()) {
        currentCommand = I2CComms::getCommandFromReadBuffer();

        if (awaitingBunchedCommandFollower) {
            addCommandToBunchedBuffer(currentCommand);
            awaitingBunchedCommandFollower = false;
            continue;
        }

        if (isCommandBunchable(currentCommand)) {
            addCommandToBunchedBuffer(currentCommand);
            if (isCommandFollowed(currentCommand)) {
                awaitingBunchedCommandFollower = true;
            }
        } else {
            executeCommand(animator, currentCommand);
        }
    }

    I2CComms::enableReceiving();
}

void IV18I2CCommandExecutor::executeBunchedCommands(IV18Animator *animator)
{
    byte commandHalfByte;
    byte indexHalfByte;

    bool setBytes = false;
    bool setChars = false;
    bool setCommas = false;

    byte bytesBuffer[IV18Display::DIGIT_STEPS_COUNT] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    char charsBuffer[IV18Display::DIGIT_STEPS_COUNT + 1] = "---------";
    bool commasBuffer[IV18Display::DIGIT_STEPS_COUNT] = {false,false,false,false,false,false,false,false,false};

    // TODO: throw exception/handle improper commands count (above buffer or not including supplementary commands with values?)
    for (short i = 0; i < bunchedCommandsCount; ++i) {
        commandHalfByte = bunchedCommandsBuffer[i] & 0xF0;
        indexHalfByte = bunchedCommandsBuffer[i] & 0x0F;
        switch (commandHalfByte) { //mask out the last 4 bits
            case CMD_MULTI_DIGIT_CLEAR:
                setBytes = true;
                setChars = true;
                setCommas = true;
                break;
            case CMD_MULTI_DIGIT_CHAR:
                if (indexHalfByte < IV18Display::DIGIT_STEPS_COUNT) {
                    setChars = true;
                    charsBuffer[indexHalfByte] = (char) bunchedCommandsBuffer[++i];
                }
                break;
            case CMD_MULTI_DIGIT_BYTE:
                if (indexHalfByte < IV18Display::DIGIT_STEPS_COUNT) {
                    setBytes = true;
                    bytesBuffer[indexHalfByte] = bunchedCommandsBuffer[++i];
                }
                break;
            case CMD_MULTI_DIGIT_POINT_R:
                if (indexHalfByte < IV18Display::DIGIT_STEPS_COUNT) {
                    setCommas = true;
                    commasBuffer[indexHalfByte] = true;
                }
                break;
            case CMD_MULTI_DIGIT_DIMMER:
                if (indexHalfByte >= IV18Display::DIGIT_STEPS_COUNT) {
                    ++i;
                    for (short j = 0; j < IV18Display::DIGIT_STEPS_COUNT; ++j) {
                        animator->setCurrentLampDigitDutyValue(
                            j,
                            IV18Animator::convertDutyCycle(bunchedCommandsBuffer[i])
                        );
                    }
                } else {
                    animator->setCurrentLampDigitDutyValue(
                        indexHalfByte,
                        IV18Animator::convertDutyCycle(bunchedCommandsBuffer[++i])
                    );
                }
                break;
            case CMD_MULTI_DIGIT_FADE_IN:
                if (indexHalfByte >= IV18Display::DIGIT_STEPS_COUNT) {
                    ++i;
                    for (short j = 0; j < IV18Display::DIGIT_STEPS_COUNT; ++j) {
                        animator->setLampDigitAction(
                            j,
                            IV18Animator::LAMP_MODE_DIGIT_IN,
                            IV18Animator::convertDutyCycle(bunchedCommandsBuffer[i]),
                            animator->getLampDigitPreviousOnDutyUs(j)
                        );
                    }
                } else {
                    animator->setLampDigitAction(
                        indexHalfByte,
                        IV18Animator::LAMP_MODE_DIGIT_IN,
                        IV18Animator::convertDutyCycle(bunchedCommandsBuffer[++i]),
                        animator->getLampDigitPreviousOnDutyUs(indexHalfByte)
                    );
                }
                break;
            case CMD_MULTI_DIGIT_FADE_OUT:
                if (indexHalfByte >= IV18Display::DIGIT_STEPS_COUNT) {
                    ++i;
                    for (short j = 0; j < IV18Display::DIGIT_STEPS_COUNT; ++j) {
                        animator->setLampDigitAction(
                            j,
                            IV18Animator::LAMP_MODE_DIGIT_OUT,
                            animator->getLampDigitPreviousOnDutyUs(j),
                            IV18Animator::convertDutyCycle(bunchedCommandsBuffer[i])
                        );
                    }
                } else {
                    animator->setLampDigitAction(
                        indexHalfByte,
                        IV18Animator::LAMP_MODE_DIGIT_OUT,
                        animator->getLampDigitPreviousOnDutyUs(indexHalfByte),
                        IV18Animator::convertDutyCycle(bunchedCommandsBuffer[++i])
                    );
                }
                break;
            case CMD_MULTI_DIGIT_FADE_TIME:
                if (indexHalfByte >= IV18Display::DIGIT_STEPS_COUNT) {
                    ++i;
                    for (short j = 0; j < IV18Display::DIGIT_STEPS_COUNT; ++j) {
                        animator->setLampDigitFramesPerAction(
                            j,
                            bunchedCommandsBuffer[i]
                        );
                    }
                } else {
                    animator->setLampDigitFramesPerAction(
                        indexHalfByte,
                        bunchedCommandsBuffer[++i]
                    );
                }
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
    short ledMode;

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
        case CMD_STATUS_LED_MODE | IV18Animator::LED_MODE_HEARTBEAT:
        case CMD_STATUS_LED_MODE | IV18Animator::LED_MODE_SQUARE_HEARTBEAT:
        case CMD_STATUS_LED_MODE | IV18Animator::LED_MODE_DIM:
            ledMode = command & 0x0F;
            animator->setLEDMode(ledMode);
            break;
        case CMD_STATUS_LED_MODE | IV18Animator::LED_MODE_WARNING:
            animator->setLEDModeWarning(8);
            break;
        default:
            // TODO: throw exception for unsupported commands?
            break;
    }
}

