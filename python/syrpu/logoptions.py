from syrpu.transmittable import Transmittable

class LogOptions(Transmittable):

    def __init__(self, log_time=True, log_force=True, log_flow=True, log_ca=False, log_loadcell=False, log_ticks=False):
        self.log_time = log_time
        self.log_force = log_force
        self.log_flow = log_flow
        self.log_ca = log_ca
        self.log_loadcell = log_loadcell
        self.log_ticks = log_ticks

    @staticmethod
    def get_key():
        return 'LO'

    def get_value(self):
        v = int(self.log_time) << 5
        v += int(self.log_force) << 4
        v += int(self.log_flow) << 3
        v += int(self.log_ca) << 2
        v += int(self.log_loadcell) << 1
        v += int(self.log_ticks)
        return f'{v}'
