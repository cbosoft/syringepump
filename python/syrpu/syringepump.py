from syrpu.arduino_iop import ArduinoIOP
from syrpu.controller.controller import Controller
from syrpu.setter.setter import Setter
from syrpu.logoptions import LogOptions
from syrpu.geometryoptions import GeometryOptions


class Syringepump:


    def __init__(self, *, controller, setter, logoptions, geometryoptions, **kwargs):
        self.arduino = ArduinoIOP(**kwargs)

        if not isinstance(controller, Controller):
            raise TypeError("controller must be a syrpu.Controller type.")

        self.controller = controller

        if not isinstance(setter, Setter):
            raise TypeError("setter must be a syrpu.Setter type.")

        self.setter = setter

        if not isinstance(logoptions, LogOptions):
            raise TypeError("logsettings must be a syrpu.LogOptions type.")

        self.logoptions = logoptions

        if not isinstance(geometryoptions, GeometryOptions):
            raise TypeError("geometryoptions must be a syrpu.GeometryOptions type.")

        self.geometryoptions = geometryoptions


    def transmit(self, transmittable):
        key = transmittable.get_key()
        value = transmittable.get_value()

        if value is None:
            return

        if not isinstance(value, str):
            raise TypeError('Transmittable.get_value() should return a string or list or None.')

        self.arduino.send_packet(key, value)


    def run(self):
        self.arduino.reset()
        self.transmit(self.controller)
        self.transmit(self.setter)
        self.transmit(self.logoptions)
        self.transmit(self.geometryoptions)

        while (True):
            line = self.arduino.read_line()

            # do something with input (save to file, save in memory?)
            # TODO

            # exit condition:
            if 'STOP' in line:
                break


    def reset(self):
        self.arduino.reset()
