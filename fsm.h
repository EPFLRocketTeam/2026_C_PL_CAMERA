#pragma once
#include <stdint.h>
#include <stddef.h>
#include "const.h"

enum State : uint8_t {
    // Init and setup power.
    INIT,
    TRY_POWER_ON,
    WAIT_FOR_ON,
    UART_INIT,

    // Power is on.
    POWER_ON,
    POWER_ON_MANUAL,

    // Start Recording.
    TRY_START,
    WAIT_FOR_START,

    // Recording.
    RECORDING,
    RECORDING_MANUAL,

    // Stop Recording.
    TRY_STOP,
    WAIT_FOR_STOP,

    // Ended (Power off)
    ENDED,

    // Aborts
    ABORT_ON_POWER_ON,
    ABORT_ON_START,
    ABORT_ON_STOP
};

enum AvionicsStateMachine : uint8_t {
    AV_INIT,
    AV_RECORD,
    AV_STOPPED,
    AV_ABORT
};

struct FiniteStateMachine {
private:
    AvionicsStateMachine avState;

    State currentState;
    
    State fromInit ();
    State fromTryPowerOn ();
    State fromWaitForOn ();
    State fromUartInit ();
    State fromPowerOn ();
    State fromPowerOnManual ();
    State fromTryStart ();
    State fromWaitForStart ();
    State fromRecording ();
    State fromRecordingManual ();
    State fromTryStop ();
    State fromWaitForStop ();
    State fromEnded ();
    State fromAbortOnPowerOn ();
    State fromAbortOnStart ();
    State fromAbortOnStop ();

    State fromCurrentState ();

    uint32_t lastChange = 0;
    uint32_t timeSinceLastChangeMs ();

    uint32_t tryNumber = 0;
public:
    void onAvionicsStart   ();
    void onAvionicsStop    ();
    void onAvionicsAbort   ();
    void onAvionicsRecover ();

    AvionicsStateMachine getAvionicsState ();
    State getState ();
    void  tick ();

    void applyActions ();
};

extern FiniteStateMachine fsm;
