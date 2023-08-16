#include "I2CComms.h"

#include <Wire.h>

byte* I2CComms::commandBuffers[2] = {new byte[I2CComms::COMMAND_BUFFER_LENGTH], new byte[I2CComms::COMMAND_BUFFER_LENGTH]};
short I2CComms::commandCountsInBuffer[2] = {0,0};
short I2CComms::readBufferRemainingCommandCount = 0;
short I2CComms::currentWriteBuffer = 0;
short I2CComms::currentReadBuffer = 1;
bool I2CComms::receivingEnabled = true;

//I2CComms Comms;
void I2CComms::init(byte addr)
{
    Wire.begin(addr);

    Wire.onReceive(receiveEvent); // register event
}

void I2CComms::receiveEvent(int howMany)
{
    if (!receivingEnabled) {
        return;
    }

    // buffers all commands
    while(Wire.available()) // loop through all
    {
        byte c = Wire.read(); // receive byte
        addCommandToWriteBuffer(c);
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
    commandCountsInBuffer[currentWriteBuffer] = 0;
    readBufferRemainingCommandCount = commandCountsInBuffer[currentReadBuffer];
}

void I2CComms::disableReceiving()
{
    receivingEnabled = false;
}

void I2CComms::enableReceiving()
{
    receivingEnabled = true;
    I2CComms::receiveEvent(0); // TODO: what about the howMany argument? unused for now
}
