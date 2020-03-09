#!/usr/bin/python

from matplotlib import pyplot as plt
import numpy as np

def read_csv(path):
    with open(path) as csvf:
        lines = csvf.readlines()
        lines = lines[11:]
        data = list()
        for line in lines:
            cells = line.split(',')
            cells = [float(c) for c in cells]
            if len(data) and len(cells) < len(data[-1]):
                break
            data.append(cells)
        data = list(zip(*data))
    return data

time, force, flowrate, ca, loadcell, ticks = read_csv("loadcell_data_9mar20.csv")

mass = list()
readings = list()
for t, lc in zip(time, loadcell):
    if t < 21392.6 or 78632.1 < t < 98136:
        mass.append(0.0)
        readings.append(lc)
    elif 25208.5 < t < 42592.4:
        mass.append(0.05)
        readings.append(lc)
    elif 48952.4 < t < 63368.2:
        mass.append(0.1)
        readings.append(lc)
    elif 66336.2 < t < 75240.2:
        mass.append(0.15)
        readings.append(lc)
    elif 100256 < t < 111280:
        mass.append(0.2)
        readings.append(lc)
    elif 114248 < t < 121032:
        mass.append(0.25)
        readings.append(lc)
    elif 127392 < t < 137568:
        mass.append(0.3)
        readings.append(lc)
    elif 141808 < t < 163431:
        mass.append(0.35)
        readings.append(lc)

force = np.multiply(mass, 9.81)
cal = np.polyfit(readings, force, 1)

plt.plot(readings, force)

calx = np.linspace(-123800, -134750)
caly = np.add(np.multiply(calx, cal[0]), cal[1])
plt.plot(calx, caly)
plt.show()

print(cal)
