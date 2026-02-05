#include "uart.h"
#include <furi_hal.h>

static FuriHalSerialHandle* uart_handle = NULL;

void uart_init(void) {
    uart_handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    furi_hal_serial_init(uart_handle, 115200);
}

void uart_deinit(void) {
    if (uart_handle) {
        furi_hal_serial_deinit(uart_handle);
        furi_hal_serial_control_release(uart_handle);
        uart_handle = NULL;
    }
}

bool uart_write(const uint8_t* data, size_t len) {
    if (!uart_handle) return false;
    furi_hal_serial_tx(uart_handle, data, len);
    return true;
}

bool uart_read(uint8_t* out, size_t len, uint32_t timeout_ms) {
    if (!uart_handle) return false;
    // Note: This is a simplified implementation
    // For production use, you'd want proper async reading with callbacks
    (void)timeout_ms;
    (void)out;
    (void)len;
    return false;
}
