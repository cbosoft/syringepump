from syrpu.setter.setter import Setter

class ConstantSetter(Setter):
    
    def __init__(self, *, constant_value, controlled_variable='flow'):
        super().__init__(self, controlled_variable)

        self.constant_value = constant_value

    def get_value(self):
        return f'{self.controlled_variable}C{self.constant_value}'
