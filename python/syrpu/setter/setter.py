from syrpu.transmittable import Transmittable

CONTROLLED_VARIABLE_TRANSLATER = {'flow': 'Q', 'force': 'F', 'voltage': 'D'}

class Setter(Transmittable):
    
    def __init__(self, controlled_variable='flow'):

        if controlled_variable not in ['flow', 'force', 'voltage']:
            raise Exception(f'Controlled variable must be one of \'force\', \'flow\', or \'voltage\'. Got \'{controlled_variable}\'')
        
        self.controlled_variable = CONTROLLED_VARIABLE_TRANSLATER[controlled_variable]

    @staticmethod
    def get_key():
        return 'SP'

