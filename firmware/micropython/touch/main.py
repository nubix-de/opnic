'''!
@file
@author Vinícius Gabriel Linden
@copyright nubix Software-Design GmbH 2022
@date 2022-10-27
@brief Capacitive touch sensor example
'''

from utime import sleep_ms
from touch_sensor import Touch_sensor_t

def main():
    touchA = Touch_sensor_t(17)
    touchB = Touch_sensor_t(15)

    try:
        while True:
            print({"A raw": touchA.raw(),
                   "A calibration": touchA.calibration,
                   "A is touched": touchA(),
                   "B raw": touchB.raw(),
                   "B calibration": touchB.calibration,
                   "B is touched": touchB()})
            sleep_ms(100)
    except KeyboardInterrupt:
            pass

if __name__ == '__main__':
    main()
