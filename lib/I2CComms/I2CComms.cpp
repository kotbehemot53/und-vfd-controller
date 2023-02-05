#include "I2CComms.h"

#include <Wire.h>

//I2CComms Comms;
void I2CComms::init(byte addr)
{
    commandBuffers[0] = new byte[BUFFER_LENGTH];
    commandBuffers[1] = new byte[BUFFER_LENGTH];
    commandCountsInBuffer[0] = 0;
    commandCountsInBuffer[1] = 0;
    readBufferRemainingCommandCount = 0;
    currentWriteBuffer = 0;
    currentReadBuffer = 1;

    Wire.begin(addr);

    Wire.onReceive(receiveEvent); // register event
}

void I2CComms::receiveEvent(int howMany)
{
    // buffers all commands
    while(Wire.available()) // loop through all
    {
        byte c = Wire.read(); // receive byte
        commandBuffers[currentWriteBuffer][commandCountsInBuffer[currentWriteBuffer]++] = c;
//        ++commandCountsInBuffer[currentWriteBuffer];

        // TODO: throw exception or prevent overflowing the buffer?
    }
}

byte I2CComms::getCommandFromReadBuffer()
{
    // returns next command and decrements the remaining command count
    return commandBuffers[currentReadBuffer][commandCountsInBuffer[currentReadBuffer] - readBufferRemainingCommandCount--];
}

short I2CComms::getReadBufferRemainingCommandCount()
{
    return readBufferRemainingCommandCount;
}

void I2CComms::resetBuffers()
{
    // flip buffers to initialize next command series
    short temp = currentWriteBuffer;
    currentWriteBuffer = currentReadBuffer;
    currentReadBuffer = temp;
    commandCountsInBuffer[0] = 0;
    commandCountsInBuffer[1] = 0;
    readBufferRemainingCommandCount = 0;
}
