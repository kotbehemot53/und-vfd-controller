#ifndef PIUCNTVFD1_I2CCOMMS_H
#define PIUCNTVFD1_I2CCOMMS_H

#include <Arduino.h>

#include "I2CCommandExecutorInterface.h"

class I2CComms
{
public:
    static const short BUFFER_LENGTH = 1000;

//    I2CComms();
    static void init(byte addr);
    static byte getCommandFromReadBuffer();
    static short getReadBufferRemainingCommandCount();
    static void resetBuffers();
private:
    static byte* commandBuffers[2];
    static short commandCountsInBuffer[2];
    static short readBufferRemainingCommandCount;
    static short currentWriteBuffer;
    static short currentReadBuffer;

    static void receiveEvent(int howMany);
};

//byte* I2CComms::commandBuffers[2] = {new byte[1000], new byte[1000]};
//short I2CComms::commandCountsInBuffer[2] = {0,0};
//short I2CComms::readBufferRemainingCommandCount = 0;
//short I2CComms::currentWriteBuffer = 0;
//short I2CComms::currentReadBuffer = 1;

//extern I2CComms Comms;

#endif //PIUCNTVFD1_I2CCOMMS_H
