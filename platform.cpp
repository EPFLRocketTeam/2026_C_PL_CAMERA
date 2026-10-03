
#include <Arduino.h>
#include <Adafruit_INA219.h>
#include "platform.h"
#include "const.h"

namespace {
    bool is_manual_mode = false;
    Adafruit_INA219 ina219;
};

void platform::initPlatform () {
    pinMode(LED, OUTPUT);
    pinMode(PWR_CTRL, OUTPUT);
    pinMode(TRIG, INPUT);
    pinMode(UART_RX, INPUT);
    pinMode(UART_TX, INPUT);

    digitalWrite(PWR_CTRL, LOW);
        
    ina219.begin();
    ina219.setCalibration_32V_1A();
    measureCurrent(NUMBER_REPETITIONS);

    is_manual_mode = digitalRead(TRIG);

    platform::can::init_can();
}
bool platform::isManualMode () {
    return is_manual_mode;
}
uint32_t platform::currentTime () {
    return millis();
}
float platform::measureCurrent(uint8_t rep) {
    float total = 0.0;
    for (uint8_t i = 0; i < rep; i++) {
        total += ina219.getCurrent_mA();
        delay(10);
    }
    return total / rep;
}
void platform::setLED (bool enabled) {
    digitalWrite(LED, enabled ? HIGH : LOW);
}
