#pragma once
#include <stdint.h>

enum {
    CMD_START        = 0x01,
    CMD_STOP         = 0x02,
    CMD_SET_CHANNEL  = 0x03,
    CMD_GET_RSSI     = 0x04,
    CMD_GET_CHANNEL  = 0x05,
    CMD_TOGGLE_SCAN  = 0x06,
};
