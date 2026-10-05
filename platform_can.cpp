
#include <Arduino.h>
#include "driver/twai.h"
#include "platform.h"
#include "const.h"
#include "fsm.h"

void platform::can::init_can () {
    if (isManualMode()) {
        return ;
    }

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX, CAN_RX, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();  // 500 kbps
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        while (1) {
            digitalWrite(LED, !digitalRead(LED));
            delay(100);
        }
    }

    if (twai_start() != ESP_OK) {
        while (1) {
            digitalWrite(LED, !digitalRead(LED));
            delay(500);
        }
    }
}

void platform::can::try_receive () {
    for (int idx = 0; idx < 8; idx ++) {
        twai_message_t rx_msg;

        bool did_receive = (twai_receive(&rx_msg, pdMS_TO_TICKS(10)) == ESP_OK);
        if (!did_receive) {
            return ;
        }

        uint16_t msg_type    = did_receive ? ((rx_msg.identifier >> 2) & 0x1FF) : 0;
        uint8_t  sender_addr = did_receive ? (rx_msg.identifier & 0x03) : 0;
        uint16_t validation  = did_receive ? ((uint16_t(rx_msg.data[0]) << 8) | rx_msg.data[1]) : 0;

        if (sender_addr != ADDR_FLIGHT_COMP) {
            return ;
        }

        if (msg_type == static_cast<uint16_t>(MessageType::START_REC) && validation == VALID_START_REC) {
            fsm.onAvionicsStart();
            continue ;
        }
        if (msg_type == static_cast<uint16_t>(MessageType::STOP_REC) && validation == VALID_STOP_REC) {
            fsm.onAvionicsStop();
            continue ;
        }

        if (msg_type == static_cast<uint16_t>(MessageType::ABORT) && validation == VALID_ABORT) {
            fsm.onAvionicsAbort();
            continue ;
        }

        if (msg_type == static_cast<uint16_t>(MessageType::RECOVER) && validation == VALID_RECOVER) {
            fsm.onAvionicsRecover();
            continue ;
        }

        // Unknown message
        return ;
    }
}

void platform::can::send_health () {
    twai_message_t tx_msg = { 0 };

    tx_msg.identifier = (static_cast<uint16_t>(MessageType::HEALTH) << 2) | WHO_AM_I;
    tx_msg.extd = 0;
    tx_msg.rtr = 0;
    tx_msg.data_length_code = 3;
    tx_msg.data[0] = fsm.getState();
    tx_msg.data[1] = fsm.getAvionicsState();
    tx_msg.data[2] 
     = (platform::camera::isRecording() ? 1 : 0)
     | (platform::camera::isPowerOn()   ? 2 : 0);

    twai_transmit(&tx_msg, pdMS_TO_TICKS(50));
}
