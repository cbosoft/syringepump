from time import sleep

import serial

from os.path import exists

class ArduinoIOP:

    def __init__(self, serial_path='/dev/ttyACM0'):

        if not exists(serial_path):
            raise Exception(f'path to arduino ({serial_path}) doesn\'t exist.')

        self.serial_path = serial_path
        self.connect()

    def __enter__(self):
        return self

    def __exit__(self, *args):
        if self.connected: 
            self.disconnect

    def connect(self):
        self._connection = serial.Serial(self.serial_path, 9600, timeout=5)
        self.connected = True

    def disconnect(self):
        self._connection.close()
        self.connected = False

    def send_data(self, data):

        if not self.connected:
            raise Exception('Need to connect before sending data.')

        sleep(0.5)

        if isinstance(data, str):
            data = data.encode()

        self._connection.write(data)
        self._connection.flush()

    def read_line(self):

        if not self.connected:
            raise Exception('Need to connect before can receive data.')

        sleep(0.5)

        buf = ''
        ch = ''
        while True:
            ch = self._connection.read().decode()
            if ch == '\n':
                break
            elif ch == '\r':
                self._connection.read()
            else:
                buf += ch
        return buf

