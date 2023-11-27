'''!
@file
@author Vinícius Gabriel Linden
@date 2022-11-07
@copyright nubix Software-Design GmbH
@brief 8-bit MCU 8080 parallel interface implementation.
@note Python is slow and thus does not require sleep between commands.
@todo TODO remake this using PIO.
'''

from machine import Pin

class Mcu_t:
    '''
    Parallel 8-bit MCU 8080 interface
    '''
    # For future use
    __name__ = "parallel"
    def __init__(self,
                 d : list[int],
                 rd : int,
                 wr : int,
                 dc : int,
                 rst : int,
                 cs : int):
        '''
        d: D[0..7]
        rd: RD, RDX, read signal
        wr: WR, WRX, write signal
        dc: D/CX, RS, A0, DC, data or command signal
        rst: RESET, RESX, reset
        cs: CS, CSX, chip select
        '''
        self.d_pins = d
        self.d = [Pin(i, Pin.OUT, Pin.PULL_UP) for i in self.d_pins]
        self.rd = Pin(rd, Pin.OUT, Pin.PULL_UP)
        self.wr = Pin(wr, Pin.OUT, Pin.PULL_UP)
        self.dc = Pin(dc, Pin.OUT, Pin.PULL_UP)
        self.rst = Pin(rst, Pin.OUT, Pin.PULL_UP)
        self.cs = Pin(cs, Pin.OUT, Pin.PULL_UP)
        self.cs(1)
        self.dc(1)
        self.wr(1)
        self.rd(1)
        self.toggle(True)

    def d_reconfigure(self, write_dir : bool):
        '''
        Reconfigure D[7..0] to be output or input pins
        write_dir: True to make them output pins
        '''
        # init function is somehow not working
        if write_dir:
            self.d = [Pin(i, Pin.OUT, Pin.PULL_UP) for i in self.d_pins]
        else:
            self.d = [Pin(i, Pin.IN) for i in self.d_pins]
        return self

    def d_set(self, value : int):
        '''
        Set D[7..0] as a single byte integer
        Note: Only use this if d are output pins
        '''
        for i in range(8):
            self.d[i](1 & (value >> i))
        return self

    def d_get(self) -> int:
        '''
        Get D[7..0] as a single byte integer
        '''
        return sum([self.d[i]()*(2**i) for i in range(8)])

    def write_commando(self, command):
        '''
        Write the "header"
        Important: it does not reset CS
        '''
        self.cs(0)
        self.dc(0)

        self.d_set(command)
        self.wr(0)
        self.wr(1)
        self.dc(1)
        return self

    def write(self, command : int, parameters = []):
        '''
        Write a command
        If the command does not have a parameter, just pass an empty list
        "parameters" may be an empty list, a list of int or a bytes or bytearray
        '''
        if type(parameters) != list and \
            type(parameters) != bytes and type(parameters) != bytearray:
            raise TypeError("Invalid parameter")
        self.write_commando(command)

        for parameter in parameters:
            self.d_set(parameter)
            self.wr(0)
            self.wr(1)

        self.cs(1)
        return self

    def read(self, command : int, parameters : int) -> list[int]:
        '''
        Read a register
        parameters: number of parameters to read, ignoring the first byte
        '''
        self.write_commando(command)

        self.d_reconfigure(False)
        data = []
        for i in range(-1, parameters):
            self.rd(0)
            self.rd(1)
            if i >= 0:
                data.append(self.d_get())

        self.d_reconfigure(True)
        self.cs(1)
        return data

    def __call__(self, command : int, parameters = None):
        '''
        Write or read.
        Write: "parameter" is none or is a list or is bytestring
        Read: "parameter" is an integer
        '''
        if parameters == None:
            parameters = []
        if type(parameters) is list or \
                type(parameters) is bytes or type(parameters) is bytearray:
            return self.write(command, parameters)
        return self.read(command, parameters)

    def toggle(self, on : bool = True):
        '''
        Hardware toggle interface on and off
        '''
        self.rst(int(on))
        return self

    def __repr__(self):
            return "MCU Parallel interface {\n" + \
              f"\tD[7..0]: {self.d[0]} -> 0x{self.d_get():02X}\n" + \
              f"\tRD: {self.rd} -> {self.rd()}\n" + \
              f"\tWR: {self.wr} -> {self.wr()}\n" + \
              f"\tCS: {self.cs} -> {self.cs()}\n" + \
              f"\tRESET: {self.rst} -> {self.rst()}\n" + \
              f"\tDC: {self.dc} -> {self.dc()}\n" + \
               "}"

def self_test():
    '''
    Requirements:
    - GC9106 display
import mcu_interface; mcu_interface.self_test()
    '''
    from time import sleep_ms
    from utils import hex_list

    interface = Mcu_t(d=[0, 1, 2, 3, 4, 5, 6, 7], rd=8, wr=9, dc=10, rst=11, cs=12)

    interface.toggle(False)
    sleep_ms(5)
    interface.toggle(True)
    sleep_ms(5)
    interface(0x01) # software reset

    print(f"read display identification: {hex_list(interface(0x00, 3))}")
    # note: 0x04 register read is always returning a wrong value
    print(f"read manufacturer programming identification: {hex_list(interface(0x04, 3))}")
    status = interface(0x09, 4)
    print(f"read display status: {hex_list(status)}")
    print(interface)

