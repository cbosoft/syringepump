from syrpu import Syringepump, PIDController, ConstantSetter, LogOptions, GeometryOptions



# Create syringepump controller (options shown are the default values)
sp = Syringepump(
        controller=PIDController(KP=0.05, KI=10.0, KD=0.0), 
        setter=ConstantSetter(constant_value=10, controlled_variable='flow'), 
        logoptions=LogOptions( log_time=True, log_flow=True, log_force=True, 
            log_ca=False, log_loadcell=False, log_ticks=False),
        geometryoptions=GeometryOptions(buffer_length=5, diameter=22.5))



# Change settings on-the-fly
sp.controller.KP = 0.5
sp.logoptions.log_loadcell = True
sp.geometryoptions.diameter = 20.0



# Swap out options
pid_slow = PIDController(KP=0.01, KI=0.5)
pid_fast = PIDController(KP=2.0, KI=10.0)

sp.controller = pid_slow
# or
sp.controller = pid_fast



# Run syringe pump
sp.run()
