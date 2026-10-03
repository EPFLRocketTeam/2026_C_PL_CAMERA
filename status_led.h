
#pragma once

#include <stdint.h>
#include <stddef.h>

struct status_led {
    uint32_t nb_blink;
    
    uint32_t value;
    uint8_t  value_length;
};

enum LEDStateMachine {
    LED_INIT,  // 1000 ms on
    LED_BLINK, // 50ms (on - off) N times
    LED_WAIT,  // 500ms off

    LED_VALUE_NEXT, // 100ms off
    LED_VALUE, // if value is true then 900 ms on
           // if value is true then 400 ms on => 500 ms off

    LED_END // 500ms off, go back to BLINK
};

void set_status (status_led xStatus);
void led_tick ();
