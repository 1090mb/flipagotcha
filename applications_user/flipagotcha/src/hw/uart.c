#include "uart.h"
#include <furi.h>
#include <uart/uart.h>

static FuriUart* uart = NULL;

void uart_init(void) {
    uart = furi_uart_alloc();
    furi_uart_set_path(uart, "/dev/uart1");
    furi_uart_set_baudrate(uart, 115200);
    furi_uart_open(uart);
}

void uart_deinit(void) {
    if (uart) {
        furi_uart_close(uart);
        furi_uart_free(uart);
        uart = NULL;
    }
}

bool uart_write(const uint8_t* data, size_t len) {
    if (!uart) return false;
    return furi_uart_write(uart, data, len) == len;
}

bool uart_read(uint8_t* out, size_t len, uint32_t timeout_ms) {
    if (!uart) return false;
    return furi_uart_read(uart, out, len, timeout_ms) == len;
}
