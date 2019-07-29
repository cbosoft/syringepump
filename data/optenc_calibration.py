#!/usr/bin/env python
from matplotlib import pyplot as plt
import numpy as np

def read_csv(path):
    with open(path) as csvf:
        data = list(zip(*[[float(cell) for cell in l.split(',')] for l in csvf.readlines()[1:]]))
    return data

data = read_csv("optenc_data.csv")
trips = data[0]

std = np.std(trips)
plt.errorbar(trips, trips, xerr=std, yerr=std, fmt='o')
med = np.median(trips)
plt.axhline(med, linestyle='--', color='C1')
plt.axvline(med, linestyle='--', color='C1')
plt.show()

print(f"Median number of ticks {med}")

total_length = 113.0 # mm
ticks_per_mm = med / total_length

print(f"Ticks per mm {ticks_per_mm}")
