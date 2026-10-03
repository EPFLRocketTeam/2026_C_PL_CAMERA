
#include <Arduino.h>
#include "const.h"
#include "platform.h"
#include "status_led.h"

namespace {
    /**
     * Blink nb_blink times and then display value_length bits stored in value in order
     */
    status_led gLEDStatusDisplayed = {
        .nb_blink     = 0,
        .value        = 0,
        .value_length = 0
    };

    LEDStateMachine ledState;

    uint8_t offset;
    uint8_t nbBlinks;

    uint32_t last_change_of_state = 0;
    
    void reset_led () {
        ledState = LED_INIT;

        nbBlinks = 0;
        offset   = 0;

        last_change_of_state = platform::currentTime();
    }
};

void set_status (status_led xStatus) {
    if (xStatus.nb_blink     == gLEDStatusDisplayed.nb_blink
     && xStatus.value        == gLEDStatusDisplayed.value
     && xStatus.value_length == gLEDStatusDisplayed.value_length) {
        return ;
    }

    gLEDStatusDisplayed = xStatus;
    reset_led();
    led_tick();
}
void led_tick () {
    uint32_t maxDeltaTime = 0;
    LEDStateMachine nextState = LEDStateMachine::LED_INIT;
    switch (ledState) {
        case LEDStateMachine::LED_INIT:
            maxDeltaTime = TimerLEDInitMs;
            nextState = LEDStateMachine::LED_BLINK;
            break ;
        case LEDStateMachine::LED_BLINK:
            maxDeltaTime = TimerLEDBlinkOnMs + TimerLEDBlinkOffMs;
            nextState = nbBlinks == gLEDStatusDisplayed.nb_blink ? LEDStateMachine::LED_VALUE_NEXT : LEDStateMachine::LED_BLINK;
            break ;
        case LEDStateMachine::LED_VALUE_NEXT:
            maxDeltaTime = TimerLEDValueNextMs;
            nextState = LEDStateMachine::LED_VALUE;
            break ;
        case LEDStateMachine::LED_VALUE:
            maxDeltaTime = TimerLEDValueDurationMs;
            nextState = offset >= gLEDStatusDisplayed.value_length ? LEDStateMachine::LED_END : LEDStateMachine::LED_VALUE_NEXT;
            break ;
        case LEDStateMachine::LED_END:
            maxDeltaTime = TimerLEDEndDurationMs;
            nextState = LEDStateMachine::LED_INIT;
            break ;
    }

    uint32_t deltaTime = (platform::currentTime() - last_change_of_state);
    if (deltaTime > maxDeltaTime) {
        last_change_of_state = platform::currentTime();
        ledState = nextState;

        switch (ledState) {
        case LEDStateMachine::LED_INIT:
            reset_led();
            break ;
        case LEDStateMachine::LED_BLINK:
            nbBlinks ++;
            break ;
        case LEDStateMachine::LED_VALUE_NEXT:
            offset ++;
            break ;
        };
    }

    switch (ledState) {
        case LEDStateMachine::LED_INIT:
            platform::setLED(false);
            break ;
        case LEDStateMachine::LED_BLINK:
            platform::setLED(deltaTime <= TimerLEDBlinkOnMs);
            break ;
        case LEDStateMachine::LED_VALUE_NEXT:
            platform::setLED(false);
            break ;
        case LEDStateMachine::LED_VALUE: 
            {
                int value = ((gLEDStatusDisplayed.value >> (offset - 1)) & 1);
                uint32_t duration = value ? TimerLEDValueOnMs : TimerLEDValueOffMs;
                platform::setLED(deltaTime <= duration);
            }
            break ;
        case LEDStateMachine::LED_END:
            platform::setLED(false);
            break ;
    }
}
