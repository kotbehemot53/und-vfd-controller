#include "IV18Animator.h"

// TODO: move this class to IV18Display directory maybe

IV18Animator::IV18Animator(IV18Display display)
{
    currentDisplay = display;

    heartbeatSteps = new DeviceAnimatorStep[2]{
        // waitUs values will be overriden on every frame by animateHeartbeat
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

void IV18Animator::animateHeartbeat()
{
    // formula for a nice sinusoidal bajinga
    //((FRAME_LENGTH_US - DIMMEST_LENGTH_US)/2)* (1 - cos(x/(0.0506599 * FRAMES_PER_CYCLE * π))) + DIMMEST_LENGTH_US

    short onTime;
    short offTime;
    double multiplier = 1 - cos(currentFrame/(0.0506599 * FRAMES_PER_CYCLE * PI));
    onTime = multiplier <= 0 ? DIMMEST_LENGTH_US : ((FRAME_LENGTH_US - DIMMEST_LENGTH_US) / 2) * multiplier + DIMMEST_LENGTH_US;
    offTime = FRAME_LENGTH_US < onTime ? 0 : FRAME_LENGTH_US - onTime;

    // TODO: maybe make a waitUs setter, only getters for the rest and make them private?
    heartbeatSteps[0].waitUs = onTime;
    heartbeatSteps[1].waitUs = offTime;
}

void IV18Animator::doFrame()
{
    // multiframe animation of the heartbeat
    animateHeartbeat();

    animator.doFrame();

    ++currentFrame;

    // for now, we're doing uniform cycles per all threads (applies only to heartbeat atm)
    if (currentFrame >= FRAMES_PER_CYCLE) {
        currentFrame = 0;
    }
}
