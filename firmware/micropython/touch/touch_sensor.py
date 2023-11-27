'''Capacitive touch sensor library

Author Vinícius Gabriel Linden
Copyright nubix Software-Design GmbH 2022
Date 2022-10-27
'''

from utime import sleep_ms
from rp2 import PIO, asm_pio, StateMachine
from machine import Pin

@asm_pio(set_init=PIO.OUT_HIGH)
def touch_pulse_get():
    set(pindirs,0)
    set(x,0)
    label('label1')
    jmp(x_dec,'label2')
    label('label2')
    nop()
    jmp( pin, 'label1')
    mov(isr,x)
    push()
    label('done')
    jmp('done')

class Touch_sensor_t():
    def __init__(self, gpio : int):
        self.sense = Pin(gpio, Pin.OUT, Pin.PULL_DOWN)
        self.sense(1)
        self.calibrate()

    def raw(self):
        sm = StateMachine(
                0,
                touch_pulse_get,
                in_base=self.sense,
                jmp_pin=self.sense,
                set_base=self.sense,
                freq=125_000_000)
        sm.active(1)
        return 0xffffffff-sm.get()

    def calibrate(self):
        self.calibration = self.raw() * 1.3

    def __call__(self):
        '''
        True: user is touching the sensor
        False: user is not touching the sensor
        '''
        return self.raw() > self.calibration
