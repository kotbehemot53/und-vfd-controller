#ifndef PIUCNTVFD1_I2CCOMMS_H
#define PIUCNTVFD1_I2CCOMMS_H

#include <Arduino.h>

// the whole thing must be static, because receiveEvent must be static and must use the other properties...
class I2CComms
{
public:
    static const short COMMAND_BUFFER_LENGTH = 128;

    static void init(byte addr);
    static byte getCommandFromReadBuffer();
    static short getReadBufferRemainingCommandCount();
    static void resetBuffers();

    // it's public for debug purposes
    inline static void addCommandToWriteBuffer(byte command) {
        // add command to write buffer and increase the command count appropriately
        commandBuffers[currentWriteBuffer][commandCountsInBuffer[currentWriteBuffer]++] = command;

        // TODO: throw exception or prevent overflowing the buffer?
    };
private:
    // 2 buffers necessary: 1 for writing, when an interrupt comes, another for reading when necessary, even when new interrupts come
    static byte* commandBuffers[2];
    static short commandCountsInBuffer[2];
    static short readBufferRemainingCommandCount;
    static short currentWriteBuffer;
    static short currentReadBuffer;

    static void receiveEvent(int howMany);
};

#endif //PIUCNTVFD1_I2CCOMMS_H
