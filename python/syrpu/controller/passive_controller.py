from syrpu.controller.controller import Controller

class PassiveController(Controller):

    def __init__(self, meas_time=10.0):
        self.meas_time = meas_time

    def get_key(self):
        return 'TP'

    def get_value(self):
        return f'M{self.meas_time}'

