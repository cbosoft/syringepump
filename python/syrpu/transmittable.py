class Transmittable:

    def get_key(self):
        raise NotImplementedError('Method not implemented.')

    def get_value(self):
        raise NotImplementedError('Method not implemented.')

    def get_key_value_pair(self):
        return self.get_key(), self.get_value()
