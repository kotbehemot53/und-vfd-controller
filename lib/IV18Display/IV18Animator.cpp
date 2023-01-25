#include "IV18Animator.h"

// TODO: move this class to IV18Display directory maybe

IV18Animator::IV18Animator(IV18Display display)
{
    currentDisplay = display;

    // TODO: turn it into a proper heartbeat thread and change the waitUs proportions in every frame accordingly
    heartbeatSteps = new DeviceAnimatorStep[2]{
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOn), 5000),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOff), 5000)
    };

    // TODO: remove this debug thread
//    auto steps2 = new DeviceAnimatorStep[3]{
//        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::noOp), 700000),
//        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOn), 100000),
//        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOff), 100000)
//    };

    // TODO: add a proper display thread (with heartbeatSteps multiplexing digits, maybe with dimming too, later on)

    threads = new DeviceAnimatorThread[1]{
        DeviceAnimatorThread(heartbeatSteps, 2),
//        DeviceAnimatorThread(steps2, 3)
    };

    animator.setThreads(threads, 1);
}

void IV18Animator::doFrame()
{
    // TODO: how to do self-modification of steps here? some callback from animator to IV18Animator??
    //       this seems like an overkill
    //       instead maybe track frame numbers and arbitrarily modify steps every frame? animator operates on pointers
    //       so it should work?
    animator.doFrame();

    ++currentFrame;
}
