
# Micropython image workflow

Acceptable image formats for now are:

- BMP with 24 colors
- python code: this offloads BMP conversion processing from the pico

1. change the `.svg` image using Inkscape
2. export it to png to the same folder:
    `inkscape --export-type=png <image>.svg`
3. export it to code, eg.:
    `./image_to_code.py -W <width> -H <height> <image>.png`
  - `image_to_code.py` is located at `utils/`
4. symbolic link to the targets
