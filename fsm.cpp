
#include "fsm.h"
#include "platform.h"
#include "status_led.h"

State FiniteStateMachine::fromInit () {
    // Stay unpowered until a START_REC command is received.
    if (avState == AV_ABORT) return State::ABORT_ON_POWER_ON;
    if (avState == AV_RECORD) return State::TRY_START;
    return currentState;
}
State FiniteStateMachine::fromTryPowerOn () {
    if (avState == AV_ABORT) return State::ABORT_ON_POWER_ON;
    if (tryNumber > MaxNumberRetries) return State::ABORT_ON_POWER_ON;
    return State::WAIT_FOR_ON;
}
State FiniteStateMachine::fromWaitForOn () {
    if (avState == AV_ABORT) return State::ABORT_ON_POWER_ON;
    if (timeSinceLastChangeMs() >= TimerRetryPowerOnMs) return State::TRY_POWER_ON;
    if (platform::camera::isPowerOn()) return State::UART_INIT;
    return currentState;
}
State FiniteStateMachine::fromUartInit () {
    if (avState == AV_ABORT) return State::ABORT_ON_POWER_ON;
    if (timeSinceLastChangeMs() >= TimerPostUartInitMs) return State::POWER_ON;
    return currentState;
}
State FiniteStateMachine::fromPowerOn () {
    if (avState == AV_ABORT) return State::ABORT_ON_POWER_ON;
    if (avState == AV_RECORD) return State::TRY_START;
    return currentState;
}
State FiniteStateMachine::fromPowerOnManual () {
    if (avState == AV_ABORT) return State::ABORT_ON_POWER_ON;
    if (timeSinceLastChangeMs() >= TimerManualStartRecordingMs) return State::TRY_START;
    return currentState;
}
State FiniteStateMachine::fromTryStart () {
    if (avState == AV_ABORT) return State::ABORT_ON_START;
    if (avState == AV_STOPPED) return State::TRY_STOP;
    if (tryNumber > MaxNumberRetries) return State::ABORT_ON_START;
    return State::WAIT_FOR_START;
}
State FiniteStateMachine::fromWaitForStart () {
    if (avState == AV_ABORT) return State::ABORT_ON_START;
    if (avState == AV_STOPPED) return State::TRY_STOP;
    // Power detection replaces UART/current-delta recording verification.
    if (platform::camera::isPowerOn()) return State::RECORDING;
    if (timeSinceLastChangeMs() >= TimerRetryStartRecordingMs) return State::TRY_START;
    return currentState;
}
State FiniteStateMachine::fromRecording () {
    if (avState == AV_ABORT) return State::ABORT_ON_START;
    if (avState == AV_STOPPED) return State::TRY_STOP;

    // No automatic recording toggles or power cycles while recording.
    return currentState;
}
State FiniteStateMachine::fromRecordingManual () {
    if (avState == AV_ABORT) return State::ABORT_ON_START;
    if (timeSinceLastChangeMs() >= TimerManualStopRecordingMs) return State::TRY_STOP;
    return currentState;
}
State FiniteStateMachine::fromTryStop () {
    if (avState == AV_ABORT) return State::ABORT_ON_STOP;
    if (tryNumber > MaxNumberRetries) return State::ABORT_ON_STOP;
    return State::WAIT_FOR_STOP;
}
State FiniteStateMachine::fromWaitForStop () {
    if (avState == AV_ABORT) return State::ABORT_ON_STOP;
    if (!platform::camera::isPowerOn()) return State::ENDED;
    if (timeSinceLastChangeMs() >= TimerRetryStopRecordingMs) return State::TRY_STOP;
    return currentState;
}
State FiniteStateMachine::fromEnded () {
    if (avState == AV_ABORT) return State::ABORT_ON_STOP;
    if (avState == AV_RECORD) return State::TRY_START;
    if (avState == AV_INIT) return State::INIT;
    return currentState;
}
State FiniteStateMachine::fromAbortOnPowerOn () {
    if (avState == AV_ABORT) return currentState;
    if (timeSinceLastChangeMs() >= TimerAbortCooldownMs) return State::INIT;
    return currentState;
}
State FiniteStateMachine::fromAbortOnStart () {
    if (avState == AV_ABORT) return currentState;
    if (timeSinceLastChangeMs() >= TimerAbortCooldownMs) return State::INIT;
    return currentState;
}
State FiniteStateMachine::fromAbortOnStop () {
    if (avState == AV_INIT) return State::INIT;
    return currentState;
}

State FiniteStateMachine::fromCurrentState () {
    switch (currentState) {
        case INIT: return fromInit();
        case TRY_POWER_ON: return fromTryPowerOn();
        case WAIT_FOR_ON: return fromWaitForOn();
        case UART_INIT: return fromUartInit();
        case POWER_ON: return fromPowerOn();
        case POWER_ON_MANUAL: return fromPowerOnManual();
        case TRY_START: return fromTryStart();
        case WAIT_FOR_START: return fromWaitForStart();
        case RECORDING: return fromRecording();
        case RECORDING_MANUAL: return fromRecordingManual();
        case TRY_STOP: return fromTryStop();
        case WAIT_FOR_STOP: return fromWaitForStop();
        case ENDED: return fromEnded();
        case ABORT_ON_POWER_ON: return fromAbortOnPowerOn();
        case ABORT_ON_START: return fromAbortOnStart();
        case ABORT_ON_STOP: return fromAbortOnStop();
    }

    return INIT;
}
void FiniteStateMachine::onAvionicsStart () {
    if (avState == AV_INIT || avState == AV_STOPPED) {
        avState = AV_RECORD;
    }
}
void FiniteStateMachine::onAvionicsStop () {
    if (avState == AV_RECORD) {
        avState = AV_STOPPED;
    }
}
void FiniteStateMachine::onAvionicsAbort () {
    avState = AV_ABORT;
}
void FiniteStateMachine::onAvionicsRecover () {
    if (avState == AV_ABORT) {
        avState = AV_INIT;
    }
}

AvionicsStateMachine FiniteStateMachine::getAvionicsState () {
    return avState;
}
State FiniteStateMachine::getState () {
    return currentState;
}

void FiniteStateMachine::tick () {
    State oldState = currentState;
    currentState = fromCurrentState();
    if (oldState != currentState) {
        lastChange = platform::currentTime();
        applyActions();
    }
}
uint32_t FiniteStateMachine::timeSinceLastChangeMs () {
    return platform::currentTime() - lastChange;
}

void FiniteStateMachine::applyActions () {
    switch (currentState) {
        case INIT:
            platform::camera::powerOff();
            tryNumber = 0;
            set_status({ .nb_blink = 2, .value = 0, .value_length = 3 });
            break ;
        case TRY_POWER_ON:
            tryNumber ++;
            platform::camera::powerOn(tryNumber == 1);
            
            set_status({ .nb_blink = 2, .value = 1, .value_length = 3 });

            break ;
        case WAIT_FOR_ON:
            break ;
        case UART_INIT:
            // UART state retained, but UART is not initialized.
            break ;
        case POWER_ON:
            set_status({ .nb_blink = 2, .value = 2, .value_length = 3 });
            platform::camera::setupPowerOnBase();
            tryNumber = 0;
            break ;
        case POWER_ON_MANUAL:
            break ;
        case TRY_START:
            set_status({ .nb_blink = 2, .value = 3, .value_length = 3 });
            tryNumber ++;
            // Hold MOSFET HIGH. Retries must not power-cycle the camera.
            platform::camera::powerOn(true);
            break ;
        case WAIT_FOR_START:
            break ;
        case RECORDING:
            set_status({ .nb_blink = 2, .value = 4, .value_length = 3 });
            tryNumber = 0;
            lastCheck = 0;
            missCount = 0;
            break ;
        case RECORDING_MANUAL:
            break ;
        case TRY_STOP:
            set_status({ .nb_blink = 2, .value = 5, .value_length = 3 });
            tryNumber ++;
            platform::camera::powerOff();
            break ;
        case WAIT_FOR_STOP:
            break ;
        case ENDED:
            tryNumber = 0;
            set_status({ .nb_blink = 2, .value = 6, .value_length = 3 });
            platform::camera::powerOff ();
            break ;
        case ABORT_ON_POWER_ON:
        case ABORT_ON_START:
        case ABORT_ON_STOP:
            set_status({ .nb_blink = 5, .value = 0, .value_length = 0 });
            platform::camera::powerOff ();
            break ;
    }
}
