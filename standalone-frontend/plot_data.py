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
# ax.set_xlim(-2.0, 2.0)
# ax.set_ylim(-2.0, 2.0)

with open("data.txt", "r") as f:
    x = []
    y = []
    for line in f.readlines():
        if line.startswith("#"):
            continue
        sl = line.split()
        x.append(float(sl[0]))
        y.append(float(sl[1]))
    ax.plot(x, y, "o", markersize=0.4)

plt.show()
