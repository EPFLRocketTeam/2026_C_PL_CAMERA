
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>

static constexpr uint8_t  ADDR_FLIGHT_COMP   = 0b11;
static constexpr uint32_t NUMBER_REPETITIONS = 10;

static constexpr uint16_t VALID_START_REC = 0x42A5;
static constexpr uint16_t VALID_STOP_REC  = 0xA542;

static constexpr gpio_num_t LED      = GPIO_NUM_2;
static constexpr gpio_num_t PWR_CTRL = GPIO_NUM_3;
static constexpr gpio_num_t TRIG     = GPIO_NUM_10;

static constexpr gpio_num_t CAN_RX = GPIO_NUM_4;
static constexpr gpio_num_t CAN_TX = GPIO_NUM_5;

static constexpr gpio_num_t UART_RX = GPIO_NUM_20;
static constexpr gpio_num_t UART_TX = GPIO_NUM_21;

static constexpr uint16_t PWR_TRESHOLD = 200.0f;
static constexpr uint8_t  REC_TRESHOLD = 20.0f;

static constexpr uint8_t RUNCAM_HEADER = 0x55;
static constexpr uint8_t RUNCAM_TAIL = 0xaa;
static constexpr uint8_t RUNCAM_CMD_CAM_CTRL = 0x01;
static constexpr uint8_t RUNCAM_TOGGLE = 0x02;

inline constexpr uint32_t MaxNumberRetries = 5;

inline constexpr uint32_t TimerRetryPowerOnMs = 2000;
inline constexpr uint32_t TimerPostUartInitMs = 2000;

inline constexpr uint32_t TimerManualStartRecordingMs = 10'000;
inline constexpr uint32_t TimerRetryStartRecordingMs  = 3'000;

inline constexpr uint32_t TimerManualStopRecordingMs = 10'000;
inline constexpr uint32_t TimerRetryStopRecordingMs  = 3'000;

#define CAM_AERO_TOP 0b01 // blue spacers
#define CAM_SEPMECH  0b10 // yellow spacers
#define CAM_AERO_BOT 0b00 // red spacers

// Update on flash
static constexpr uint8_t WHO_AM_I = CAM_SEPMECH;

static constexpr uint32_t TimerLEDInitMs          = 1000;
static constexpr uint32_t TimerLEDBlinkOnMs       = 100;
static constexpr uint32_t TimerLEDBlinkOffMs      = 100;
static constexpr uint32_t TimerLEDValueNextMs     = 250;
static constexpr uint32_t TimerLEDValueDurationMs = 750;
static constexpr uint32_t TimerLEDValueOnMs       = 500;
static constexpr uint32_t TimerLEDValueOffMs      = 200;
static constexpr uint32_t TimerLEDValueOffMs      = 200;
static constexpr uint32_t TimerLEDEndDurationMs   = 500;

static constexpr uint32_t TimerHealthPacketMs = 1000;

enum class MessageType : uint16_t {
    START_REC = 0b111000000,
    STOP_REC  = 0b000111000,
    HEALTH    = 0b000000111,
    NONE      = 0b000000000
};
