#include <furi.h>
#include <gui/gui.h>
#include "ui/view.h"
#include "hw/uart.h"

int32_t flipagotcha_main(void* p) {
    (void)p;

    /* Initialise UART (ESP‑32 link) */
    uart_init();

    /* Spawn UI thread */
    FuriThread* ui_thr = furi_thread_alloc_ex(
        "FlipUI", 4096, ui_thread_entry, NULL);
    furi_thread_start(ui_thr);

    /* Block until UI thread finishes */
    furi_thread_join(ui_thr);

    /* Clean‑up */
    furi_thread_free(ui_thr);
    uart_deinit();

    return 0;
}
