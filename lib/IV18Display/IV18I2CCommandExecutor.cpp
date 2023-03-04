#include "IV18I2CCommandExecutor.h"

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
}
