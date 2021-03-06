from time import sleep

import serial

from os.path import exists

class NotConnectedError(Exception):
    '''Error raised when an action needs a connection, but the connection has not been made.'''

def needs_connection(f):
    def decorator(self, *args, **kwargs):
        if not self.connected:
            raise NotConnectedError(f'{f.__name__} need connection: arduino not connected.')
        else:
            return f(self, *args, **kwargs)
    return decorator

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

    @needs_connection
    def reset(self):
        self._connection.setDTR(False)
        sleep(0.05)
        self._connection.setDTR(True)
        sleep(3)


    def connect(self):
        self._connection = serial.Serial(self.serial_path, 9600, timeout=5)
        self.connected = True
        self.reset()

    def disconnect(self):
        if self.connected:
            self._connection.close()
            self.connected = False

    @needs_connection
    def send_data(self, data):

        if isinstance(data, str):
            data = data.encode()

        self._connection.write(data)
        self._connection.flush()

    @needs_connection
    def read_line(self):
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

    @needs_connection
    def send_packet(self, key, value):
        pass

