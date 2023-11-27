#!/usr/bin/env python3
'''!
@file
@author Vinícius Gabriel Linden
@company nubix Software-Design GmbH
@date 2022-11-17
@brief Take an image and convert it to raw code
@todo Add different color pallets
'''

import sys, os, getopt
from PIL import Image

def color565(red : int = 0,
             green : int = 0,
             blue : int = 0) -> int:
    '''
    Single RGB565 pixel definition
    Every color should be between 0x00 and 0xFF
    '''
    # colors are also inverted (maybe a "dot inversion" setting)
    # the following is a work around
    red ^= 0xFF
    green ^= 0xFF
    blue ^= 0xFF

    # the following is a workaround
    red, green, blue = blue, red, green

    red &= 0xF8
    green &= 0xFC
    blue &= 0xFF
    return red << 8 | green << 3 | blue >> 3

def main(argv):
    HEIGHT : int = 128
    WIDTH : int = 160
    usage = "USAGE:\n" + \
           f"  ./{os.path.basename(__file__)} [OPTIONS] FILE\n" + \
           f"OPTIONS:\n" + \
           f"  -H, --height: image height [default {HEIGHT}]\n" + \
           f"  -W, --width: image width [default {WIDTH}]"
    try:
        opts, remainder = getopt.getopt(argv, "hH:W:", ["help", "height", "width"])
    except getopt.GetoptError:
        print(usage)
        sys.exit(1)
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            print(usage)
            sys.exit(0)
        elif opt in ('-H', '--height'):
            HEIGHT = int(arg)
            if HEIGHT <= 0:
                raise Exception("Invalid parameter")
        elif opt in ('-W', '--width'):
            WIDTH = int(arg)
            if WIDTH <= 0:
                raise Exception("Invalid parameter")

    path = remainder[0]
    if not os.path.isfile(path):
        raise Exception(f"{os.path.abspath(path)} is not a file")

    # convert to bmp, resize and save with appropriate colors
    new = "/tmp/" + os.path.splitext(os.path.basename(path))[0] + ".bmp"
    Image.open(path).resize((WIDTH, HEIGHT)).convert("RGB", colors=24).save(new)

    buffer = bytes()
    width : int = 0
    height : int = 0

    # Read file and convert it to raw format
    with open(new, 'rb') as file:
        if file.read(2) != b'BM':
            raise Exception("wrong file header")
        _ = file.read(8) # dimension, creator bytes
        offset = int.from_bytes(file.read(4), 'little')
        _ = int.from_bytes(file.read(4), 'little') # header size
        width = int.from_bytes(file.read(4), 'little')
        height = int.from_bytes(file.read(4), 'little')

        if int.from_bytes(file.read(2), 'little') != 1:
            raise Exception("planes must be one")
        depth = int.from_bytes(file.read(2), 'little')
        if depth != 24:
            raise Exception("must be 24 bits color")
        if int.from_bytes(file.read(4), 'little') != 0:
            raise Exception("must be uncompressed")

        print(f"image size: {width}x{height}")
        rowsize = (width * 3 + 3) & ~3

        for row in range(height):
            pos = offset + (height - 1 - row) * rowsize
            if file.tell() != pos:
                _ = file.seek(pos)
            for _ in range(width):
                read = file.read(3)
                color = color565(read[0], read[1], read[2])
                buffer += color.to_bytes(2, "big")

    # save file as python code
    name = os.path.splitext(path)
    with open(name[0] + ".py", 'w') as file:
        # user should use import <name> and then access via <name>.CONTENT
        file.write("CONTENT = bytearray(")
        file.write(str(buffer))
        file.write(")\n")
        file.write(f"WIDTH = {width}\n")
        file.write(f"HEIGHT = {height}\n")

if __name__ == '__main__':
    main(sys.argv[1:])
