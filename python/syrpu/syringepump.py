from syrpu.arduino_iop import ArduinoIOP

class Syringepump:

    def __init__(self, **kwargs):
        self.arduino = ArduinoIOP(**kwargs)

    def run(self):
        pass
