#ifndef PIUCNTVFD1_I2CCOMMANDEXECUTORINTERFACE_H
#define PIUCNTVFD1_I2CCOMMANDEXECUTORINTERFACE_H

#include <Arduino.h>

class I2CCommandExecutorInterface
{
public:
//    virtual bool bufferCommand(byte command);
    virtual void handleBufferedInput();
};


#endif //PIUCNTVFD1_I2CCOMMANDEXECUTORINTERFACE_H
