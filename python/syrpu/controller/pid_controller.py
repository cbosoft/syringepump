from syrpu.controller.controller import Controller

class PIDController(Controller):

    def __init__(self, KP=0.01, KI=10.0, KD=0.0):
        self.KP = KP
        self.KI = KI
        self.KD = KD

    def get_key(self):
        return 'TP'

    def get_value(self):
        return f'P{self.KP}{self.KI}{self.KD}'
