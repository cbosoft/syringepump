from syrpu.transmittable import Transmittable

class GeometryOptions(Transmittable):

    def __init__(self, buffer_length=5.0, diameter=22.5):
        self.buffer_length = buffer_length
        self.diameter = diameter

    @staticmethod
    def get_key():
        return 'BD'

    def get_value(self):
        return '{self.buffer_length},{self.diameter}'
