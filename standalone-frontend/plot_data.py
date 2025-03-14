#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from matplotlib import pyplot as plt
import matplotlib as mpl

fig, ax = plt.subplots()
font = {"size": 20, "family": "sans-serif"}
mpl.rc("font", **font)
plt.rc("text", usetex=True)
ax.set_xlabel(r"$x$")
ax.set_ylabel(r"$y$")

with open("data.txt", "r") as f:
    x = []
    y = []
    for line in f.readlines():
        if line.startswith("#"):
            continue
        sl = line.split()
        x.append(float(sl[1]))
        y.append(float(sl[2]))
    ax.plot(x, y, "o", markersize=0.4)

plt.show()
