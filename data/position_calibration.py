#!/usr/bin/python

from matplotlib import pyplot as plt
import numpy as np

def read_csv(path):
    with open(path) as csvf:
        data = list(zip(*[[float(cell) for cell in l.split(',')] for l in csvf.readlines()[1:]]))
    return data

positions, readings = read_csv("position_data.csv")
coefs = np.polyfit(readings, positions, 1)
print(coefs)
fit_positions = np.add(np.multiply(coefs[0], readings), coefs[1])

plt.plot(readings, positions, 'x')
plt.plot(readings, fit_positions, '--')
plt.show()
