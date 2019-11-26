#!/usr/bin/python

from matplotlib import pyplot as plt
import numpy as np

def read_csv(path):
    with open(path) as csvf:
        data = list(zip(*[[float(cell) for cell in l.split(',')] for l in csvf.readlines()[1:]]))
    return data

#volumes, readings = read_csv("loadcell_data.csv")
volumes, readings = read_csv("loadcell_data_nov8.csv")

volumes = np.array(volumes)
volumes = np.multiply(volumes, 1E-6)

# calibration set up is two syringes with a length of tubing in between. One
# syring is fully open, the other is fully closed. This is such that as you
# close one, the other opens. Or, if one is constricted, the volume of gas is
# compressed.

# Initial pressure
initial_pressure = 101325.0 # pa

# volume is one syringe volume plus volume in tubing.
v_syringe = 30E-6 # m^3
v_tubing = np.pi * ((2.5E-3)**2.0) * 100E-3 # m^3
initial_volume = v_syringe + v_tubing
print(initial_volume)

# volumes actually include tubing volumes
volumes = np.add(volumes, v_tubing)

# final pressures
final_pressures = np.divide(initial_pressure*initial_volume, volumes)
final_pressures_guage = np.subtract(final_pressures, initial_pressure)
print(np.average(final_pressures_guage))

# area pressure is acting on
plunger_diameter = 21.5E-3
plunger_radius = plunger_diameter / 2.0
plunger_area = np.pi * ((plunger_radius) ** 2.0) # m^2

## finally get the forces
forces = np.multiply(final_pressures_guage, plunger_area)

coefs = np.polyfit(readings, forces, 1)
print(coefs)
fit_forces = np.add(np.multiply(coefs[0], readings), coefs[1])

plt.plot(np.multiply(volumes, 1e6), forces, 'x')
plt.xlabel("Volume (ml)")
plt.ylabel("Force (N)")
plt.show()
plt.close()

plt.figure()
plt.plot(readings, fit_forces, '--')
plt.xlabel("Reading (24 bit)")
plt.ylabel("Force (N)")
plt.show()
