#!/usr/bin/env python3
import sys

def main():
    if len(sys.argv) != 3:
        print("Usage: font2c.py font.amft font.c")
        return 1

    with open(sys.argv[1], "rb") as in_f:
        data = in_f.read()

    with open(sys.argv[2], "w") as out_f:
        out_f.write("#include <stdint.h>\n")
        out_f.write("#pragma once\n")
        out_f.write("static const uint8_t default_font[] = {")
        arr = ["0x{:02X}".format(b) for b in data]
        out_f.write(", ".join(arr))
        out_f.write("};\n")

    return 0

if __name__ == "__main__":
    sys.exit(main())
