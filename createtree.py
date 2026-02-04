#!/usr/bin/env python3
"""
createtree.py

Creates the complete flipagotcha repository layout and populates every
source/header file with the exact boiler‑plate content we designed.

Features:
* Safe file creation – skips existing files.
* Detailed console output (what’s created, what’s skipped, any errors).
* Optional dry‑run mode (--dry-run) to preview actions.
* Works on macOS, Linux, and Windows (Python 3.6+).

Run:
    chmod +x createtree.py
    ./createtree.py            # normal run
    ./createtree.py --dry-run  # preview only
"""

import argparse
import pathlib
import sys
from textwrap import dedent

# ----------------------------------------------------------------------
# Helper: write a file only if it does not already exist
# ----------------------------------------------------------------------
def write_if_missing(target: pathlib.Path, content: str, dry_run: bool) -> None:
    """Write *content* to *target* only when the file does not already exist."""
    if target.exists():
        print(f"⚠️  Skipping existing: {target}")
        return

    if dry_run:
        print(f"[DRY‑RUN] Would create: {target}")
        return

    try:
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(content, encoding="utf-8")
        print(f"✅ Created: {target}")
    except Exception as exc:  # pragma: no cover – defensive
        print(f"❌ Failed to create {target}: {exc}", file=sys.stderr)


# ----------------------------------------------------------------------
# Repository layout definition
# Each tuple = (relative_path, file_contents or None)
# If file_contents is None the entry is a directory only.
# ----------------------------------------------------------------------
repo_structure = [
    # -------------------- top‑level files --------------------
    (".gitignore", dedent("""\
        # Build artefacts
        /dist/
        /build/
        /obj/
        *.dfu
        *.elf
        *.map
        *.bin
        *.hex

        # CMake generated files
        CMakeCache.txt
        CMakeFiles/
        cmake_install.cmake
        Makefile

        # Python cache
        __pycache__/
        *.pyc

        # macOS / Windows junk
        .DS_Store
        Thumbs.db
        """)),

    ("LICENSE", dedent("""\
        MIT License

        Copyright (c) 2026 1090mb

        Permission is hereby granted, free of charge, to any person obtaining a copy
        of this software and associated documentation files (the "Software"), to deal
        in the Software without restriction, including without limitation the rights
        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
        furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included in
        all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
        FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
        DEALINGS IN THE SOFTWARE.
        """)),

    ("README.md", dedent("""\
        # flipagotcha – a tiny “Pwnagotchi‑style” Flipper Zero app

        **What it does**

        * Shows a minimalist animated face (blinking eyes, smile/frown) on the Flipper’s 128 × 64 monochrome screen.  
        * Uses UART to talk to an external ESP‑32 (optional) for Wi‑Fi sniffing.  
        * Buttons:  
          * **OK** – toggles the Wi‑Fi scanner (`CMD_TOGGLE_SCAN`).  
          * **← / →** – close/open eyes manually.  
          * **↑ / ↓** – switch between smile and frown.  

        **Why no PNG assets?**  
        The face is drawn directly with the Flipper Canvas API, so the binary stays tiny and you never need to generate or ship image files.

        ## 📂 Repository layout
        (tree omitted for brevity – see the script source for the full list)

        ## 🛠️ Building the app
        1. Clone this repo into your Flipper firmware tree under `applications_user/flipagotcha`.  
        2. Run `./fbt COMPACT=1 APPSRC=applications_user/flipagotcha`.  
        3. Flash the resulting `dist/flipagotcha.dfu` with qFlipper or `dfu-util`.

        ## 📡 Optional ESP‑32 Wi‑Fi bridge
        (see design notes for the UART command set)

        ## 🎨 Adding more faces / expressions
        Edit `src/ui/draw.c` – all drawing is done with Canvas primitives.

        ## 📜 License
        MIT – see `LICENSE` file.
        """)),

    ("CMakeLists.txt", dedent("""\
        # Optional top‑level CMake file – useful if you open the repo in an IDE.
        cmake_minimum_required(VERSION 3.13)
        project(flipagotcha LANGUAGES C)

        # The real build definition lives in
        # applications_user/flipagotcha/CMakeLists.txt.
        """)),

    # -------------------- tools (reference only) --------------------
    ("tools/gen_assets.py", dedent("""\
        #!/usr/bin/env python3
        \"\"\"
        gen_assets.py – converts 8×8 1‑bit PNG icons into a C header.
        Kept for reference; the current app draws faces programmatically.
        \"\"\"
        import pathlib, sys
        try:
            from PIL import Image
        except ImportError:
            sys.stderr.write("Pillow not installed. Run: pip install pillow\\n")
            sys.exit(1)

        ICON_DIR = pathlib.Path(__file__).parent.parent / "applications_user" / "flipagotcha" / "resources" / "icons"
        OUT_HDR = pathlib.Path(__file__).parent.parent / "applications_user" / "flipagotcha" / "src" / "ui" / "assets.h"

        def png_to_bytes(png_path):
            img = Image.open(png_path).convert("1")
            if img.size != (8, 8):
                raise ValueError(f"{png_path} is not 8×8")
            rows = []
            for y in range(8):
                byte = 0
                for x in range(8):
                    if img.getpixel((x, y)) == 0:  # black pixel
                        byte |= 1 << (7 - x)
                rows.append(byte)
            return rows

        def main():
            pngs = sorted(p for p in ICON_DIR.iterdir() if p.suffix.lower() == ".png")
            sheet = []
            enum = []
            for idx, p in enumerate(pngs):
                sheet.extend(png_to_bytes(p))
                enum.append(f"    {p.stem.upper()} = {idx * 8},")
            with open(OUT_HDR, "w") as f:
                f.write("// AUTO‑GENERATED – DO NOT EDIT MANUALLY\\n")
                f.write("#pragma once\\n#include <stdint.h>\\n\\n")
                f.write("static const uint8_t FACE_SHEET[] = {\\n")
                for i, b in enumerate(sheet):
                    f.write(f"    0x{b:02X},")
                    if (i + 1) % 8 == 0:
                        f.write("\\n")
                f.write("};\\n\\nenum {\\n")
                f.write("\\n".join(enum))
                f.write("\\n};\\n")
            print(f"Generated {OUT_HDR}")

        if __name__ == "__main__":
            main()
        """)),

    # -------------------- applications_user/flipagotcha --------------------
    ("applications_user/flipagotcha/CMakeLists.txt", dedent("""\
        # Build definition for the flipagotcha app
        add_application(flipagotcha src/main.c)

        target_sources(flipagotcha PRIVATE
            src/ui/view.c
            src/ui/draw.c
            src/hw/uart.c
        )

        target_link_libraries(flipagotcha PRIVATE
            furi
            gui
            storage
            uart
        )

        target_include_directories(flipagotcha PRIVATE src/ui)
        """)),

    ("applications_user/flipagotcha/src/main.c", dedent("""\
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

            /* Main loop – wait for UI thread to finish */
            while (!furi_thread_is_stopped(ui_thr)) {
                furi_delay_ms(200);
            }

            /* Clean‑up */
            furi_thread_join(ui_thr);
            furi_thread_free(ui_thr);
            uart_deinit();

            return 0;
        }
        """)),

    ("applications_user/flipagotcha/src/ui/view.h", dedent("""\
        #pragma once
        #include <gui/view_port.h>
        #include <gui/gui.h>
        #include <stdbool.h>

        typedef struct {
            ViewPort* vp;
            Gui* gui;
            bool eyes_closed;
            bool mouth_frown;
            bool scanning;
            uint8_t current_channel;
            FuriTimer* anim_timer;
        } UiState;

        void ui_thread_entry(void* args);
        """)),

    ("applications_user/flipagotcha/src/ui/view.c", dedent("""\
        #include "view.h"
        #include "draw.h"
        #include "../hw/uart.h"
        #include "../common/protocol.h"
        #include <furi.h>
        #include <input/input.h>

        #define ANIM_TICK_MS 100

        /* ------------------------------------------------------------------ */
        static void draw_callback(Canvas* canvas, void* ctx) {
            UiState* st = ctx;

            /* Clear background (optional) */
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_box(canvas, 0, 0, 128, 64, ColorBlack);
            canvas_set_color(canvas, ColorWhite);

            /* Draw the face */
            draw_face(canvas,
                      40,               // x offset
                      20,               // y offset
                      st->eyes_closed,
                      st->mouth_frown);
        }

        /* ------------------------------------------------------------------ */
        static void input_callback(InputEvent* ev, void* ctx) {
            UiState* st = ctx;
            if (ev->type != InputTypeShort) return;

            switch (ev->key) {
                case InputKeyOk:
                    {
                        uint8_t cmd = CMD_TOGGLE_SCAN;
                        uart_write(&cmd, 1);
                        st->scanning = !st->scanning;
                    }
                    break;
                case InputKeyLeft:
                    st->eyes_closed = true;
                    break;
                case InputKeyRight:
                    st->eyes_closed = false;
                    break;
                case InputKeyUp:
                    st->mouth_frown = false;   // smile
                    break;
                case InputKeyDown:
                    st->mouth_frown = true;    // frown
                    break;
                case InputKeyBack:
                    furi_thread_exit(furi_thread_get_current());
                    break;
                default:
                    break;
            }
            view_port_update(st->vp);
        }

        /* ------------------------------------------------------------------ */
        static void anim_tick(void* ctx) {
            UiState* st = ctx;
            static uint8_t blink_ctr = 0;
            blink_ctr++;
                            if (blink_ctr >= 4) {          # toggle every 4 ticks ≈400 ms
                    st->eyes_closed = !st->eyes_closed;
                    blink_ctr = 0;
                }
                view_port_update(st->vp);
            }

            /* ------------------------------------------------------------------ */
            void ui_thread_entry(void* args) {
                (void)args;

                UiState* st = malloc(sizeof(UiState));
                memset(st, 0, sizeof(UiState));

                /* ViewPort creation */
                st->vp = view_port_alloc();
                view_port_draw_callback_set(st->vp, draw_callback, st);
                view_port_input_callback_set(st->vp, input_callback, st);

                /* Attach to global GUI */
                st->gui = furi_record_open("gui");
                gui_add_view_port(st->gui, st->vp, GuiLayerFullscreen);

                /* Animation timer */
                st->anim_timer = furi_timer_alloc(anim_tick, FuriTimerTypePeriodic, st);
                furi_timer_start(st->anim_timer, ANIM_TICK_MS);

                /* Keep thread alive until the user exits */
                while (!furi_thread_is_stopped(furi_thread_get_current())) {
                    furi_delay_ms(200);
                }

                /* Clean‑up */
                furi_timer_stop(st->anim_timer);
                furi_timer_free(st->anim_timer);
                gui_remove_view_port(st->gui, st->vp);
                view_port_free(st->vp);
                furi_record_close(st->gui);
                free(st);
            }
            """)),

    ("applications_user/flipagotcha/src/ui/draw.h", dedent("""\
        #pragma once
        #include <gui/canvas.h>
        #include <stdbool.h>

        void draw_face(Canvas* canvas,
                       int x,
                       int y,
                       bool eyes_closed,
                       bool mouth_frown);
        """)),

    ("applications_user/flipagotcha/src/ui/draw.c", dedent("""\
        #include "draw.h"

        static inline void set_px(Canvas* canvas, int x, int y) {
            canvas_draw_point(canvas, x, y, ColorBlack);
        }

        /* ------------------------------------------------------------------ */
        static void draw_eyes_open(Canvas* canvas, int x, int y) {
            set_px(canvas, x + 2, y + 2);
            set_px(canvas, x + 5, y + 2);
        }

        static void draw_eyes_closed(Canvas* canvas, int x, int y) {
            canvas_draw_line(canvas, x + 2, y + 4, x + 3, y + 4, ColorBlack);
            canvas_draw_line(canvas, x + 5, y + 4, x + 6, y + 4, ColorBlack);
        }

        /* ------------------------------------------------------------------ */
        static void draw_mouth_smile(Canvas* canvas, int x, int y) {
            canvas_draw_line(canvas, x + 2, y + 5, x + 3, y + 6, ColorBlack);
            canvas_draw_line(canvas, x + 3, y + 6, x + 4, y + 6, ColorBlack);
            canvas_draw_line(canvas, x + 4, y + 6, x + 5, y + 5, ColorBlack);
        }

        static void draw_mouth_frown(Canvas* canvas, int x, int y) {
            canvas_draw_line(canvas, x + 2, y + 5, x + 3, y + 4, ColorBlack);
            canvas_draw_line(canvas, x + 3, y + 4, x + 4, y + 4, ColorBlack);
            canvas_draw_line(canvas, x + 4, y + 4, x + 5, y + 5, ColorBlack);
        }

        /* ------------------------------------------------------------------ */
        void draw_face(Canvas* canvas,
                       int x,
                       int y,
                       bool eyes_closed,
                       bool mouth_frown) {

            /* Optional clear of the area (helps when switching expressions) */
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_box(canvas, x, y, 12, 16, ColorWhite);
            canvas_set_color(canvas, ColorBlack);

            if (eyes_closed) {
                draw_eyes_closed(canvas, x, y);
            } else {
                draw_eyes_open(canvas, x, y);
            }

            if (mouth_frown) {
                draw_mouth_frown(canvas, x, y);
            } else {
                draw_mouth_smile(canvas, x, y);
            }
        }
        """)),

    ("applications_user/flipagotcha/src/hw/uart.h", dedent("""\
        #pragma once
        #include <stdbool.h>
        #include <stddef.h>
        #include <stdint.h>

        void uart_init(void);
        void uart_deinit(void);
        bool uart_write(const uint8_t* data, size_t len);
        bool uart_read(uint8_t* out, size_t len, uint32_t timeout_ms);
        """)),

    ("applications_user/flipagotcha/src/hw/uart.c", dedent("""\
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
            return furi_uart_write(uart, data, len) == len;
        }

        bool uart_read(uint8_t* out, size_t len, uint32_t timeout_ms) {
            return furi_uart_read(uart, out, len, timeout_ms) == len;
        }
        """)),

    ("applications_user/flipagotcha/src/common/protocol.h", dedent("""\
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
        """)),

    # Empty resources folder (no PNGs needed for programmatic drawing)
    ("applications_user/flipagotcha/resources/", None),
]

# ----------------------------------------------------------------------
# Main driver – parse arguments, then create everything
# ----------------------------------------------------------------------
def main() -> None:
    parser = argparse.ArgumentParser(
        description="Create the flipagotcha repository layout."
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Show what would be created without writing any files.",
    )
    args = parser.parse_args()

    root = pathlib.Path.cwd()
    for rel_path, content in repo_structure:
        target = root / rel_path
        if content is None:
            # Directory entry – ensure it exists (or dry‑run)
            if args.dry_run:
                print(f"[DRY‑RUN] Would ensure directory exists: {target}")
            else:
                try:
                    target.mkdir(parents=True, exist_ok=True)
                    print(f"📁 Created folder (if missing): {target}")
                except Exception as exc:  # pragma: no cover – defensive
                    print(f"❌ Failed to create folder {target}: {exc}", file=sys.stderr)
        else:
            write_if_missing(target, content, args.dry_run)


if __name__ == "__main__":
    # Quick sanity check – ensure we're running on Python 3.6+
    if sys.version_info < (3, 6):
        sys.stderr.write("Error: Python 3.6 or newer is required.\n")
        sys.exit(1)

    main()
            