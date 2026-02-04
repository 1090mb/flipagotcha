#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void uart_init(void);
void uart_deinit(void);
bool uart_write(const uint8_t* data, size_t len);
bool uart_read(uint8_t* out, size_t len, uint32_t timeout_ms);
