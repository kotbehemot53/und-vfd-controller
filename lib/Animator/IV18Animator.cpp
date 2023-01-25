#include "IV18Animator.h"

#include <Arduino.h>

IV18Animator::IV18Animator(IV18Display display)
{
    currentDisplay = display;

    auto steps = new DeviceAnimatorStep[2]{
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOn), 500000),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOff), 500000)
    };

    auto threads = new DeviceAnimatorThread[1]{
        DeviceAnimatorThread(steps, 2)
    };

    animator.setThreads(threads, 1);
}

void IV18Animator::doFrame()
{
    // TODO: how to do self-modification of steps here? some callback from animator to IV18Animator??
    animator.doFrame();
}
