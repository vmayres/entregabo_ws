import sys
import time
import serial
from robot import Robot

esp32 = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

class MainWindow:
    def __init__(self):

        self.robot = Robot()
        self.movements = list()
        self.atual_move = 0
        self.edit_mode = False
        self.playing = False

    def play_move(self):
        frameframe = [[90,90,110,90],[90,59,100,90],[100,59,100,90],[100,90,100,90],[180,90,100,90],[180,70,100,90],[90,90,110,90]]

        for moves in frameframe:
            self.robot.set_all_joints_angle(moves)
            time.sleep(0.1)


if __name__ == '__main__':
    main_win = MainWindow()
    try:
        esp32.write(b'1')
        while True:
            #recebe dados do esp32
            if esp32.in_waiting > 0:
                data = esp32.read(1)
                if data == b'1':
                    main_win.play_move()
            time.sleep(1)
    except KeyboardInterrupt:
        esp32.close()
        sys.exit(0)
    finally:
        esp32.close()
        sys.exit(0)
