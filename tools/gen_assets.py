#!/usr/bin/env python3
"""
gen_assets.py – converts 8×8 1‑bit PNG icons into a C header.
Kept for reference; the current app draws faces programmatically.
"""
import pathlib, sys
try:
    from PIL import Image
except ImportError:
    sys.stderr.write("Pillow not installed. Run: pip install pillow\n")
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
        f.write("// AUTO‑GENERATED – DO NOT EDIT MANUALLY\n")
        f.write("#pragma once\n#include <stdint.h>\n\n")
        f.write("static const uint8_t FACE_SHEET[] = {\n")
        for i, b in enumerate(sheet):
            f.write(f"    0x{b:02X},")
            if (i + 1) % 8 == 0:
                f.write("\n")
        f.write("};\n\nenum {\n")
        f.write("\n".join(enum))
        f.write("\n};\n")
    print(f"Generated {OUT_HDR}")

if __name__ == "__main__":
    main()
