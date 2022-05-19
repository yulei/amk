#!/usr/bin/env python3

import sys
import argparse
import json

KBD_UNIT = 4
KBD_SPACE = 0
KBD_OFFSET_X = 6
KBD_OFFSET_Y = 30

def print_key(key):
    #print("x:{},y:{},w:{},h:{},r:{},c:{}".format(key["x"], key["y"], key["w"], key["h"], key["pos"]["r"], key["pos"]["c"]))
    print("{{ {:>3d}, {:>3d}, {:>3d}, {:>3d}, {{ {:>2d}, {:>2d} }} }},".format(key["x"], key["y"], key["w"], key["h"], key["pos"]["c"], key["pos"]["r"]))

def dump_to_file(keys, file):
    with open(file, "w") as f:
        f.write("const keyboard_key_t keyboard_keys[KEYBOARD_KEY_COUNT] = {\n\t// x, y, w, h, pos.col, pos.row\n")
        for key in keys:
            ks = "\t{{ {:>3d}, {:>3d}, {:>3d}, {:>3d}, {{ {:>2d}, {:>2d} }} }},\n".format(key["x"], key["y"], key["w"], key["h"], key["pos"]["c"], key["pos"]["r"])
            f.write(ks)
        f.write("};\n")


def convert_to_kbd(data, file):
    x = 0
    y = 0
    keys = []
    w = KBD_UNIT

    for row in data:
        x = 0
        for item in row:
            if isinstance(item,dict):
                for k,v in item.items():
                    if k == "w":
                        w = int(KBD_UNIT*v)
                    if k == "x":
                        x += int(KBD_UNIT*v)
                    if k == "y":
                        y += int(KBD_UNIT*v)
            else:
                key = {}
                key["x"] = x + KBD_OFFSET_X
                key["y"] = y + KBD_OFFSET_Y
                key["w"] = w
                key["h"] = KBD_UNIT
                x += w + KBD_SPACE
                w = KBD_UNIT    #reset w
                #print(item)
                pos_x, pos_y = item.split(',')
                #print(pos_x, pos_y)
                key["pos"] = {"r":int(pos_x), "c":int(pos_y)}
                keys.append(key)
                print_key(key)

        y += KBD_UNIT
    dump_to_file(keys, file)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", help="intput kle json file")
    parser.add_argument("-f", "--format", default="kbd", help="output file format")
    parser.add_argument("-o", "--output", help="output file")
    args = parser.parse_args()

    with open(args.input) as f:
        data_in = json.load(f)
        if (args.format == "kbd"):
            convert_to_kbd(data_in, args.output)


if __name__ == "__main__":
    sys.exit(main())
