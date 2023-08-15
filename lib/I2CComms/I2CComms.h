#ifndef PIUCNTVFD1_I2CCOMMS_H
#define PIUCNTVFD1_I2CCOMMS_H

#include <Arduino.h>

// the whole thing must be static, because receiveEvent must be static and must use the other properties...
/**
 * Main buffer for the incomming I2C commands.
 */
class I2CComms
{
public:
    static const short COMMAND_BUFFER_LENGTH = 191;

    /**
     * Initialize I2C communication on a given address.
     *
     * @param addr
     */
    static void init(byte addr);

    /**
     * Get next buffered command from the read buffer.
     *
     * @return
     */
    static byte getCommandFromReadBuffer();

    /**
     * Get the number of commands remaining in the read buffer.
     * @return
     */
    static short getReadBufferRemainingCommandCount();

    /**
     * Swap buffers before reading (previous read buffer is cleared and becomes a write buffer,
     * previous write buffer becomes a read buffer). Should be run before reading previously written commands to execute them.
     */
    static void resetBuffers();

    /**
     * Receiving events will be paused - receiveEvent will do nothing (keeping received bytes in Wire buffer for later
     * reading). Useful before resetting buffers and reading stuff.
     */
    static void disableReceiving();

    /**
     * Receiving events will be enabled again. Calls receiveEvent to process bytes received when it was disabled.
     * Useful after resetting buffers and reading stuff.
     */
    static void enableReceiving();

    // it's public for debug purposes
    /**
     * Adds a command to the current write buffer. Should be run when a new command is received via I2C.
     *
     * @param command
     */
    inline static void addCommandToWriteBuffer(byte command) {
        if (commandCountsInBuffer[currentWriteBuffer] == COMMAND_BUFFER_LENGTH) {
            // TODO: throw exception or prevent overflowing the buffer?
            //       plain return for now
            return;
        }
        // add command to write buffer and increase the command count appropriately
        commandBuffers[currentWriteBuffer][commandCountsInBuffer[currentWriteBuffer]++] = command;
    };
private:
    // 2 buffers necessary: 1 for writing, when an interrupt comes, another for reading when necessary, even when new interrupts come
    static byte* commandBuffers[2];
    static short commandCountsInBuffer[2];
    static short readBufferRemainingCommandCount;
    static short currentWriteBuffer;
    static short currentReadBuffer;
    static bool receivingEnabled;

    static void receiveEvent(int howMany);
};

#endif //PIUCNTVFD1_I2CCOMMS_H
