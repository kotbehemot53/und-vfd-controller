#include "IV18Animator.h"

// TODO: move this class to IV18Display directory maybe

IV18Animator::IV18Animator(IV18Display display)
{
    currentDisplay = display;

    // TODO: turn it into a proper heartbeat thread and change the waitUs proportions in every frame accordingly
    auto steps = new DeviceAnimatorStep[2]{
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOn), 500000),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOff), 1000000)
    };

    // TODO: remove this debug thread
    auto steps2 = new DeviceAnimatorStep[3]{
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::noOp), 700000),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOn), 100000),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*)>(&IV18Display::statusOff), 100000)
    };

    // TODO: add a proper display thread (with steps multiplexing digits, maybe with dimming too, later on)

    auto threads = new DeviceAnimatorThread[2]{
        DeviceAnimatorThread(steps, 2),
        DeviceAnimatorThread(steps2, 3)
    };

    animator.setThreads(threads, 2);
}

void IV18Animator::doFrame()
{
    // TODO: how to do self-modification of steps here? some callback from animator to IV18Animator??
    //       this seems like an overkill
    //       instead maybe track frame numbers and arbitrarily modify steps every frame? animator operates on pointers
    //       so it should work?
    animator.doFrame();
}
