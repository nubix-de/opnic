'''!
@file
@author Vinícius Gabriel Linden
@date 2022-11-10
@copyright nubix Software-Design GmbH
@brief High level function for drawing graphics on TFT display.
@note Based on https://github.com/peterhinch/micropython-nano-gui
'''

import framebuf

class Display_t(framebuf.FrameBuffer):
    def __init__(self, controller):
        self.controller = controller
        self.controller.init()
        self.controller.direction_set("x-y exchange x-invert")
        self.height = self.controller.HEIGHT
        self.width = self.controller.WIDTH
        super().__init__(
            bytearray(self.height * self.width * 2),
            self.height,
            self.width,
            framebuf.RGB565)

    def color(self,
              red : int = 0,
              green : int = 0,
              blue : int = 0):
        '''
        Get desired color
        '''
        return self.controller.color_get(blue, red, green)

    def color_random(self):
        '''
        Get a random color
        '''
        return self.controller.color_random()

    def image(self,
              image,
              x : int = 0,
              y : int = 0):
        '''
        Add an image
        image: either a path to a BMP image or the translated python code
        x: x position [px]
        y: y position [px]
        '''
        if type(image) is str:
            file = open(image, 'rb')
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

            rowsize = (width * 3 + 3) & ~3

            buffer = bytes()
            for row in range(height):
                pos = offset + (height - 1 - row) * rowsize
                if file.tell() != pos:
                    _ = file.seek(pos)
                for _ in range(width):
                    read = file.read(3)
                    color = self.color(read[0], read[1], read[2])
                    buffer += color.to_bytes(2, "big")

            self.blit(
                    framebuf.FrameBuffer(bytearray(buffer),
                                         width,
                                         height,
                                         framebuf.RGB565),
                    x,
                    y)
        else:
            self.blit(
                    framebuf.FrameBuffer(image.CONTENT,
                                         image.WIDTH,
                                         image.HEIGHT,
                                         framebuf.RGB565),
                    x,
                    y)
        return self

    def blank(self):
        '''
        Reset screen to black color
        '''
        self.fill(self.color())
        return self

    def show(self):
        '''
        Display buffered frame
        '''
        self.controller(bytearray(self))
        return self

