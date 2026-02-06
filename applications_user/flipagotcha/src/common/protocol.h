#pragma once
#include <stdint.h>

enum {
    CMD_START        = 0x01,
    CMD_STOP         = 0x02,
    CMD_SET_CHANNEL  = 0x03,
    CMD_GET_RSSI     = 0x04,
    CMD_GET_CHANNEL  = 0x05,
    CMD_TOGGLE_SCAN  = 0x06,
    CMD_SCAN_NETWORKS = 0x07,
    CMD_START_HANDSHAKE = 0x08,
    CMD_CAPTURE_PACKET = 0x09,
    CMD_SAVE_CAPTURE = 0x0A,
};

#define MAX_SSID_LEN 32
#define MAX_NETWORKS 16
#define MAX_PACKET_SIZE 2048

typedef struct {
    char ssid[MAX_SSID_LEN + 1];
    uint8_t bssid[6];
    int8_t rssi;
    uint8_t channel;
    uint8_t encryption;
} WifiNetwork;

typedef struct {
    uint8_t data[MAX_PACKET_SIZE];
    uint16_t length;
    uint32_t timestamp;
    uint8_t channel;
} CapturedPacket;
