#include "uart.h"
#include "../common/protocol.h"
#include <furi_hal.h>
#include <furi.h>
#include <string.h>

static FuriHalSerialHandle* uart_handle = NULL;
static UartRxCallback rx_callback = NULL;
static void* rx_callback_context = NULL;
static FuriThread* rx_thread = NULL;
static volatile bool rx_thread_running = false;

// UART receive buffer
static uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static size_t rx_buffer_pos = 0;
static FuriMutex* rx_mutex = NULL;

// UART RX interrupt callback
static void uart_on_irq_cb(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    (void)context;
    
    if (event == FuriHalSerialRxEventData) {
        uint8_t data;
        while (furi_hal_serial_rx(handle, &data, 1) > 0) {
            if (rx_mutex && furi_mutex_acquire(rx_mutex, 0) == FuriStatusOk) {
                if (rx_buffer_pos < UART_RX_BUFFER_SIZE) {
                    rx_buffer[rx_buffer_pos++] = data;
                }
                furi_mutex_release(rx_mutex);
            }
        }
    }
}

// Worker thread that processes received data
static int32_t uart_rx_worker(void* context) {
    (void)context;
    
    while (rx_thread_running) {
        size_t available = 0;
        
        if (rx_mutex && furi_mutex_acquire(rx_mutex, FuriWaitForever) == FuriStatusOk) {
            available = rx_buffer_pos;
            
            // Process received data if callback is set
            if (available > 0 && rx_callback) {
                rx_callback(rx_buffer, available, rx_callback_context);
                // Clear buffer after processing
                rx_buffer_pos = 0;
            }
            
            furi_mutex_release(rx_mutex);
        }
        
        furi_delay_ms(100);
    }
    
    return 0;
}

void uart_init(void) {
    if (uart_handle) return; // Already initialized
    
    uart_handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    if (!uart_handle) return;
    
    furi_hal_serial_init(uart_handle, 115200);
    
    // Create mutex for RX buffer
    rx_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    
    // Set up RX callback
    furi_hal_serial_async_rx_start(uart_handle, uart_on_irq_cb, NULL, false);
    
    // Start RX worker thread
    rx_thread_running = true;
    rx_thread = furi_thread_alloc_ex("UartRxWorker", 2048, uart_rx_worker, NULL);
    furi_thread_start(rx_thread);
}

void uart_deinit(void) {
    if (!uart_handle) return;
    
    // Stop RX worker thread
    if (rx_thread) {
        rx_thread_running = false;
        furi_thread_join(rx_thread);
        furi_thread_free(rx_thread);
        rx_thread = NULL;
    }
    
    // Stop async RX
    furi_hal_serial_async_rx_stop(uart_handle);
    
    // Free mutex
    if (rx_mutex) {
        furi_mutex_free(rx_mutex);
        rx_mutex = NULL;
    }
    
    furi_hal_serial_deinit(uart_handle);
    furi_hal_serial_control_release(uart_handle);
    uart_handle = NULL;
    
    rx_callback = NULL;
    rx_callback_context = NULL;
    rx_buffer_pos = 0;
}

bool uart_write(const uint8_t* data, size_t len) {
    if (!uart_handle || !data || len == 0) return false;
    furi_hal_serial_tx(uart_handle, data, len);
    return true;
}

bool uart_write_str(const char* str) {
    if (!str) return false;
    return uart_write((const uint8_t*)str, strlen(str));
}

bool uart_read(uint8_t* out, size_t len, uint32_t timeout_ms) {
    if (!uart_handle || !out || len == 0) return false;
    
    uint32_t start_time = furi_get_tick();
    size_t read_count = 0;
    
    while (read_count < len) {
        if (furi_get_tick() - start_time > furi_ms_to_ticks(timeout_ms)) {
            return false; // Timeout
        }
        
        if (rx_mutex && furi_mutex_acquire(rx_mutex, 10) == FuriStatusOk) {
            while (rx_buffer_pos > 0 && read_count < len) {
                out[read_count++] = rx_buffer[0];
                // Shift buffer
                rx_buffer_pos--;
                if (rx_buffer_pos > 0) {
                    memmove(rx_buffer, rx_buffer + 1, rx_buffer_pos);
                }
            }
            furi_mutex_release(rx_mutex);
        }
        
        if (read_count < len) {
            furi_delay_ms(10);
        }
    }
    
    return true;
}

void uart_set_rx_callback(UartRxCallback callback, void* context) {
    rx_callback = callback;
    rx_callback_context = context;
}

bool uart_is_connected(void) {
    if (!uart_handle) return false;
    
    // Clear any stale data in buffer first
    if (rx_mutex && furi_mutex_acquire(rx_mutex, FuriWaitForever) == FuriStatusOk) {
        rx_buffer_pos = 0;
        furi_mutex_release(rx_mutex);
    }
    
    // Send "help" command to verify Marauder firmware is responding
    // Marauder should respond with command list
    const char* test_cmd = MARAUDER_CMD_HELP;
    uart_write_str(test_cmd);
    
    // Wait for response
    furi_delay_ms(UART_CONNECTION_TEST_TIMEOUT_MS);
    
    // Check if we got any response
    bool has_data = false;
    if (rx_mutex && furi_mutex_acquire(rx_mutex, FuriWaitForever) == FuriStatusOk) {
        has_data = (rx_buffer_pos > 0);
        // Clear buffer after test
        rx_buffer_pos = 0;
        furi_mutex_release(rx_mutex);
    }
    
    return has_data;
}
