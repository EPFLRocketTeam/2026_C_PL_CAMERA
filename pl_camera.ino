
#include "platform.h"
#include "status_led.h"
#include "fsm.h"

FiniteStateMachine fsm;

void setup () {
    platform::initPlatform();

    // Apply init actions
    fsm.applyActions();
}

uint32_t last_health_packet = 0;

void loop () {
    if ((platform::currentTime() - last_health_packet) >= TimerHealthPacketMs) {
        last_health_packet = platform::currentTime();
        platform::can::send_health();
    }

    fsm.tick();
    platform::can::try_receive();
    led_tick();
}
