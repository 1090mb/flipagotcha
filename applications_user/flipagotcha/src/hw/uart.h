#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Callback type for receiving UART data
typedef void (*UartRxCallback)(const uint8_t* data, size_t len, void* context);

void uart_init(void);
void uart_deinit(void);
bool uart_write(const uint8_t* data, size_t len);
bool uart_write_str(const char* str);
bool uart_read(uint8_t* out, size_t len, uint32_t timeout_ms);

// Set callback for receiving data
void uart_set_rx_callback(UartRxCallback callback, void* context);

// Check if UART is connected and responding
bool uart_is_connected(void);
