from gc9106 import Gc9106_t
from mcu_interface import Mcu_t
from display import Display_t
from utils import *

mcu = Mcu_t(d=[0, 1, 2, 3, 4, 5, 6, 7], rd=8, wr=9, dc=10, rst=11, cs=12)
controller = Gc9106_t(mcu)
display = Display_t(controller)

blue = display.color(red = 0xFF)

display.blank()
display.image("nubix.bmp")
display.rect(10, 10, 10, 10, blue, True)
display.hline(30, 30, 20, blue)
display.text("nubix", 0, 0, blue)
display.ellipse(160//2, 120//2, 10, 20, blue)
display.line(100, 100, 110, 110, blue)

import apple
display.image(apple, 144, 80)
display.show()


# if __name__ == '__main__':
    # gc9106_test()
    # display_test()

