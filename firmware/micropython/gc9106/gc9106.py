'''!
@file
@author Vinícius Gabriel Linden
@date 2022-10-28
@copyright nubix Software-Design GmbH
@brief GC9106 display controller
'''

import micropython
from time import sleep_ms
from os import urandom
import micropython

@micropython.native
def color_565(red : int = 0,
              green : int = 0,
              blue : int = 0) -> int:
    '''
    Single RGB565 pixel definition
    Every color should be between 0x00 and 0xFF
    Note:
    - FrameBuffer does not support RGB666 by default
    - FrameBuffer methods only accepts integers as colors
    '''
    # colors are also inverted (maybe a "dot inversion" setting)
    # the following is a work around
    red ^= 0xFF
    green ^= 0xFF
    blue ^= 0xFF

    red &= 0xF8
    green &= 0xFC
    blue &= 0xFF
    return red << 8 | green << 3 | blue >> 3

class Gc9106_t:
    '''
    GC9106 display controller
    '''
    def __init__(self, interface):
        '''
        interface: interface used
        Currently only available:
        - parallel interface
        - 160x128 pixel display
        - RGB666 color mode
        Creating this object does not initialize the screen
        self.init() to run the initialization sequence
        '''
        self.WIDTH = 128 # px
        self.HEIGHT = 160 # px
        self.interface = interface
        self.internal_command_enabled = False

        # initialization values
        self.mv = False
        self.color_get = self.IFPF["565"][1]
        self.color_size = self.IFPF["565"][2]

    #      scheme:   IFPF, function, size
    IFPF = {"444": [0b001, None, None], # 12 bit, 4k
            "565": [0b101, color_565, 2], # 16 bit, 65k
            "666": [0b110, None, 3]  # 18 bit, 262k
            }

    def color_random(self) -> int:
        '''
        Get a random color
        '''
        return int().from_bytes(urandom(self.color_size), 'little')

    def reset(self,
              hardware : bool = True,
              software : bool = True):
        '''
        Perform a reset
        hardware: perform a hardware reset before software reset
        software: perform a software reset
        '''
        if hardware:
            self.interface.toggle(False)
            sleep_ms(5)
            self.interface.toggle(True)
            sleep_ms(15)
        if software:
            self.interface(0x01) # software reset
            '''
            "It will be necessary to wait 5msec before sending new command following
            software reset"
            '''
            sleep_ms(5)
        return self

    def nop(self, n : int = 1):
        '''
        No operation
        Use this to break commands
        n: repeat this command "n" times
        '''
        while n > 0:
            self.interface(0x2C) # memory write with no parameters
            n -= 1
        return self

    def display(self, on : bool = True):
        '''
        Turn display on or off per software
        on: True for "display on";
            False for "display off"
        '''
        self.interface(0x29 if on else 0x29)
        return self

    def invertcolor(self, invert : bool = False):
        '''
        Invert colors
        invert: True for "display inversion on";
                False for "display inversion off"
        '''
        self.interface(0x21 if invert else 0x20)
        return self

    def sleep(self, sleep : bool = False):
        '''
        Enter or exit sleep modus
        sleep: True for "sleep in";
               False for "sleep out"
        '''
        self.interface(0x10 if sleep else 0x11)
        sleep_ms(120) # datasheet, self-diagnostics
        return self

    def idle(self, on : bool = False):
        '''
        Enter or exit idle mode
        Idle mode is simply a reduced color set, using only the MSB of each
        color
        on: True to enter;
            False to exit
        '''
        self.interface(0x39 if on else 0x38)
        return self

    def normal_display_mode_on(self):
        '''
        Normal display mode on
        Exit this mode via a partial display command
        '''
        self.interface(0x13) # normal mode on
        return self

    def partial_mode_on(self):
        '''
        Enter partial mode
        Exit this mode via the normal display mode command
        '''
        self.interface(0x12) # partial mode on
        return self

    def partial_area(self,
                     sr : int,
                     er : int):
        '''
        Configure partial area selection
        ML also has an effect on partial area
        sr: start row
        er: end row
        If sr = er, partial area will be one deep
        '''
        sr &= 0xFFFF
        er &= 0xFFFF
        parameters = [sr >> 8, sr & 0xFF, er >> 8, er & 0xFF]
        self.interface(0x30, parameters) # partial area
        return self

    def display_inversion_control(
            self,
            inv_ctl : int,
            bgr_comp : bool):
        '''
        Display inversion mode set
        inv_ctl: inversion type
        - 0 = column
        - 1 = 1 dot
        - ...
        - 4 = 4 dot
        bgr_comp: True = fix high value; False = fix 0
        '''
        self.internal_command_enable()
        inv_ctl &= 0b111
        if inv_ctl > 4:
            raise ValueError("Invalid parameter")
        parameter = [int(bgr_comp) << 5 | inv_ctl]
        self.interface(0xB4, parameter) # display inversion control
        return self

    def internal_command_enable(self, force : bool = False):
        '''
        Enable internal commands
        force: send the command a second time
        '''
        if not self.internal_command_enable or force:
            self.interface(0xFE) # internal register command 1
            self.interface(0xEF) # internal register command 2
            self.interface(0xB6, [0b111]) # enabling clocks read
            self.internal_command_enabled = True
        return self

    def pixel_format_set(self, ifpf : str = "RGB565"):
        '''
        Pixel format set
        No effect when in idle off mode
        ifpf: possible values:
        - RGB444
        - RGB565
        - RGB666
        '''
        desired = self.IFPF[ifpf.upper().lstrip("RGB")]
        if desired[1] is None or desired[2] is None:
            raise NotImplementedError("Parameter is unsupported")
        self.color_get = desired[1]
        self.color_size = desired[2]
        self.interface(0x3A, [desired[0]]) # Pixel Format Set
        self.ifpf = ifpf
        return self

    def column_address_set(self,
                           sc : int = 0,
                           ec : int = -1):
        '''
        Define column limitation
        No parameters for maximum area size
        sc: start column, always <= ec
        ec: end column, < 0 for maximum
        Note: range changes with MV and screen size
        '''
        sc &= 0xFFFF
        if ec < 0:
            ec = 0x009F if self.mv else 0x007F # screen size
        ec &= 0xFFFF
        if sc >= ec:
            raise ValueError("Invalid parameter")

        parameters = [sc >> 8, sc & 0xFF, ec >> 8, ec & 0xFF]
        self.interface(0x2A, parameters) # column address set
        return self

    def row_address_set(self,
                        sp : int = 0,
                        ep : int = -1):
        '''
        Define row limitation
        No parameters for maximum area size
        sp: start page, always <= ep
        ep: end page, < 0 for maximum
        Note: range changes with MV and screen size
        '''
        sp &= 0xFFFF
        if ep < 0:
            ep = 0x007F if self.mv else 0x009F # screen size
        ep &= 0xFFFF
        if sp >= ep:
            raise ValueError("Invalid parameter")

        parameters = [sp >> 8, sp & 0xFF, ep >> 8, ep & 0xFF]
        self.interface(0x2B, parameters) # column address set
        return self

    def frame_rate_set(self, framerate : int):
        '''
        Set frame rate
        framerate: raw value to set framerate, consult the datasheet
        '''
        framerate &= 0b1111111
        if framerate > 0x7f:
            raise ValueError("Invalid parameter")
        self.internal_command_enable()
        self.interface(0xA3, [framerate]) # frame rate set
        return self

    def id_get(self):
        '''
        Reads "read display id", "read 'manufactory' programming id", "read ID1"
        to "read ID3"
        '''
        IDS = self.interface(0x00) # read display identification
        # information is repeated in the following lines
        IDM = self.interface(0x04) # read "manufactory" programming identification
        ID1 = self.interface(0xDA)[0] # read ID1
        ID2 = self.interface(0xDB)[0] # read ID2
        ID3 = self.interface(0xDC)[0] # read ID3
        return {"IDS": IDS,
                "IDM": IDM,
                "ID1": ID1,
                "ID2": ID2,
                "ID3": ID3}

    def memory_access_control(self,
                  my : bool = False,
                  mx : bool = False,
                  mv : bool = False,
                  ml : bool = False,
                  bgr : bool = False,
                  mh : bool = False):
        '''
        Read/write scanning definitions for frame memory
        No parameter to reset values
        my: row address order
        mx: column address order
        mv: row / column exchange
        ml: vertical refresh order: origin to end
        bgr: RGB-BGR order (RGB = 0)
        mh: horizontal refresh order: origin to end
        This command has no effect when tearing = ON
        '''
        parameter = int(my) << 7 | int(mx) << 6 | \
                    int(mv) << 5 | int(ml) << 4 | \
                    int(bgr) << 3 | int(mh) << 2
        if bgr:
            raise NotImplementedError("Parameter is unsupported")
        self.interface(0x36, [parameter]) # memory access control
        self.mv = mv
        return self

    def read_display_status(self) -> dict:
        '''
        Get current read display status
        '''
        status = self.interface(0x09, 4) # read display status
        BSTON = status[0] >> 7
        MY = 1&(status[0] >> 6)
        MX = 1&(status[0] >> 5)
        MV = 1&(status[0] >> 4)
        ML = 1&(status[0] >> 3)
        RGB = 1&(status[0] >> 2)
        IFPF = 0b111&(status[1] >> 4)
        IDMON = 1&(status[1] >> 3)
        PLTON = 1&(status[1] >> 2)
        SLPOUT = 1&(status[1] >> 1)
        NORON = 1&(status[1])
        INVON = 1&(status[2] >> 5)
        DISON = 1&(status[2] >> 2)
        TEON = 1&(status[2] >> 1)
        TEM = 1&(status[3] >> 5)
        self.mv = bool(MV)
        return {'raw': status,
                'BSTON': BSTON,
                'MY': MY,
                'MX': MX,
                'MV': MV,
                'ML': ML,
                'RGB': RGB,
                'IFPF': IFPF,
                'IDMON': IDMON,
                'PLTON': PLTON,
                'SLPOUT': SLPOUT,
                'NORON': NORON,
                'INVON': INVON,
                'DISON': DISON,
                'TEON': TEON,
                'TEM': TEM}

    def gamma_set(self,
                  vp0 : int, vn0 : int,
                  vp1 : int, vn1 : int,
                  vp2 : int, vn2 : int,
                  vp4 : int, vn4 : int,
                  vp6 : int, vn6 : int,
                  vp13 : int, vn13 : int,
                  vp20 : int, vn20 : int,
                  vp27 : int, vn27 : int,
                  vp36 : int, vn36 : int,
                  vp43 : int, vn43 : int,
                  vp50 : int, vn50 : int,
                  vp57 : int, vn57 : int,
                  vp59 : int, vn59 : int,
                  vp61 : int, vn61 : int,
                  vp62 : int, vn62 : int,
                  vp63 : int, vn63 : int):
        '''
        Set the gray scale voltage to adjust gamma values
        vp#: positive voltage
        vn#: negative voltage
        '''
        self.internal_command_enable()
        self.interface(0xB3, [0b11]) # enable access
        vp0 &= 0b1111
        vp1 &= 0b111111
        vp2 &= 0b111111
        vp4 &= 0b1111
        vp6 &= 0b11111
        vp13 &= 0b1111
        vp20 &= 0b1111111
        vp27 &= 0b1111
        vp36 &= 0b1111
        vp43 &= 0b1111111
        vp50 &= 0b1111
        vp57 &= 0b11111
        vp59 &= 0b1111
        vp61 &= 0b111111
        vp62 &= 0b111111
        vp63 &= 0b1111
        parameters = [vp0, vp1, vp2, vp4, vp6, vp13, vp20, vp36 << 4 | vp27,
                      vp43, vp50, vp57, vp59, vp61, vp62, vp63]
        self.interface(0xF0, parameters) # set gamma1
        vn0 &= 0b1111
        vn1 &= 0b111111
        vn2 &= 0b111111
        vn4 &= 0b1111
        vn6 &= 0b11111
        vn13 &= 0b1111
        vn20 &= 0b1111111
        vn27 &= 0b1111
        vn36 &= 0b1111
        vn43 &= 0b1111111
        vn50 &= 0b1111
        vn57 &= 0b11111
        vn59 &= 0b1111
        vn61 &= 0b111111
        vn62 &= 0b111111
        vn63 &= 0b1111
        parameters = [vn0, vn1, vn2, vn4, vn6, vn13, vn20, vn36 << 4 | vn27,
                      vn43, vn50, vn57, vn59, vn61, vn62, vn63]
        self.interface(0xF1, parameters) # set gamma2
        return self

    def memory_write(self,
                     raw):
        '''
        Write pixels in the defined area
        raw: raw subpixel information to send
             may be a list, a list of int or bytestring
        '''
        # we trust that the user will input only integers that are one byte
        # in size
        if len(raw) % self.color_size:
            raise TypeError("Missing color information")
        self.interface(0x2C, raw)
        return self

    __call__ = memory_write

    def direction_set(self,
                      direction : str,
                      mh : bool = False,
                      reset : bool = True):
        '''
        Rotate and set direction
        direction: name according to "5.2.2. Frame Data Write Direction" table
            NORMAL
            Y-INVERT
            X-INVERT
            X-INVERT Y-INVERT
            X-Y EXCHANGE
            X-Y EXCHANGE Y-INVERT
            X-Y EXCHANGE X-INVERT
            X-Y EXCHANGE Y-INVERT X-INVERT
        mh: horizontal refresh order: origin to end
        reset: reset column and row limitations
        '''
        DIRECTION = {"NORMAL":
                        {"MV": False, "MX": False, "MY": False},
                    "Y-INVERT":
                        {"MV": False, "MX": False, "MY": True},
                    "X-INVERT":
                        {"MV": False, "MX": True, "MY": False},
                    "X-INVERT Y-INVERT":
                        {"MV": False, "MX": True, "MY": True},
                    "X-Y EXCHANGE":
                        {"MV": True, "MX": False, "MY": False},
                    "X-Y EXCHANGE Y-INVERT":
                        {"MV": True, "MX": False, "MY": True},
                    "X-Y EXCHANGE X-INVERT":
                        {"MV": True, "MX": True, "MY": False},
                    "X-Y EXCHANGE Y-INVERT X-INVERT":
                        {"MV": True, "MX": True, "MY": True}
                    }
        desired = DIRECTION[direction.upper()]
        current = self.read_display_status()
        self.memory_access_control(
                my=desired['MY'],
                mx=desired['MX'],
                mv=desired['MV'],
                ml=current['ML'],
                bgr=current['RGB'],
                mh=mh)
        if reset:
            self.column_address_set().row_address_set()
        return self

    def __repr__(self):
        return f"GC9106 controlled {self.WIDTH}x{self.HEIGHT} display"

    def init(self):
        '''
        Initialization sequence
        '''
        self.reset(hardware=True, software=False)

        self.pixel_format_set("RGB565")
        self.normal_display_mode_on()
        self.column_address_set()
        self.row_address_set()
        self.memory_access_control()
        self.frame_rate_set(0x00) # 91.8 Hz
        self.idle(False)

        self.sleep(False)
        sleep_ms(120) # according to datasheet
        self.display(True)
        return self

