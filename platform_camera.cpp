
#include <Arduino.h>
#include "platform.h"
#include "const.h"

namespace {
    float base_power_on_current;
    bool uart_is_on = false;
};


bool platform::camera::isPowerOn () {
    return measureCurrent(NUMBER_REPETITIONS) >= PWR_TRESHOLD;
}
void platform::camera::powerOn (bool isFirst) {
    if (!isFirst) {
        digitalWrite(PWR_CTRL, LOW);
        delay(100);
    }

    digitalWrite(PWR_CTRL, HIGH);
}
void platform::camera::setupPowerOnBase () {
    base_power_on_current = measureCurrent(NUMBER_REPETITIONS);
}
void platform::camera::powerOff () {
    if (uart_is_on) {
        Serial1.end();
        pinMode(UART_RX, INPUT);
        pinMode(UART_TX, INPUT);
        uart_is_on = false;
    }
    
    digitalWrite(PWR_CTRL, LOW);
}

bool platform::camera::isRecording () {
    return (
        measureCurrent(NUMBER_REPETITIONS) - base_power_on_current
    ) >= REC_TRESHOLD;
}


uint8_t crc_high_first(uint8_t* ptr, uint8_t len) {
  uint8_t i;
  uint8_t crc = 0x00;
  while (len--) {
    crc ^= *ptr++;
    for (i = 8; i > 0; --i) {
      if (crc & 0x80)
        crc = (crc << 1) ^ 0x31;
      else
        crc = (crc << 1);
    }
  }
  return (crc);
}
uint8_t runcam_crc(uint8_t header, uint8_t cmd, uint8_t toggle) {
  uint8_t buffer[5];

  buffer[0] = RUNCAM_HEADER;
  buffer[1] = RUNCAM_CMD_CAM_CTRL;
  buffer[2] = RUNCAM_TOGGLE;
  buffer[3] = RUNCAM_TAIL;

  uint8_t crc = crc_high_first(buffer, 4);

  return crc;
}
void toggleRecording() {
  uint8_t pkt[5];
  pkt[0] = RUNCAM_HEADER;
  pkt[1] = RUNCAM_CMD_CAM_CTRL;
  pkt[2] = RUNCAM_TOGGLE;
  pkt[3] = runcam_crc(pkt[0], pkt[1], pkt[2]);
  pkt[4] = RUNCAM_TAIL;

  platform::camera::sendUart(pkt, 5);
}

void platform::camera::startRecording (bool isFirst) {
    toggleRecording();
}
void platform::camera::stopRecording (bool isFirst) {
    toggleRecording();
}

void platform::camera::initUart () {
    if (!uart_is_on) {
        Serial1.begin(115200, SERIAL_8N1, UART_RX, UART_TX);
        uart_is_on = true;
    }
}
void platform::camera::sendUart (const uint8_t *data, size_t len) {
    Serial1.write(data, len);
}
