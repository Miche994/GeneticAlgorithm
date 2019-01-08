#!/bin/env python
import sys
import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def plotComparison(x, y, title, ylabel, xlabel, legend=None, xtick=None, ytick=None, toMark=None, filename=None):
    fig, ax = plt.subplots(figsize=(10,8))
    df = pd.DataFrame(y)
    df.plot.bar(ax=ax)
    ax.set_xticklabels(x)	
    if legend is not None:
    	ax.legend(legend)
    if ytick is not None:
    	ax.set_yticks(ytick, minor = True) #to set the minor ticks
    	ax.tick_params(axis = 'y', which = 'minor', labelsize = 7)
    	ax.get_yaxis().set_minor_formatter(matplotlib.ticker.StrMethodFormatter("{x:.0f}"))
    #ax.set_yscale('log', basey=2)
    if toMark is not None:
    	for xi, yi in zip([k-1 for k in x],toMark):
    		ax.annotate(str(yi),xy=(xi,yi+1000), fontsize=7)
    ax.set_title(title, fontsize=18)
    ax.set_ylabel(ylabel, fontsize=12, labelpad=12)
    ax.set_xlabel(xlabel, fontsize=12, labelpad=12)
    fig.tight_layout()
    if filename is not None:
        plt.savefig(fname='res/' + filename, transparent=True)

def plotLinear(x, y, title, ylabel, xlabel, legend=None, xtick=None, ytick=None, toMark=None, optima=None, filename=None):
    fig, ax = plt.subplots(figsize=(10,8))
    for toPrint, col in zip(y,colors):
        plt.plot(x, toPrint, c=col)
        plt.scatter(x, toPrint, c=col)
    if xtick is None:
        ax.set_xticklabels(x)
    else:
        plt.xticks(xtick)
    if legend is not None:
        ax.legend(legend)
    if ytick is not None:
        ax.set_yticks(ytick, minor = True) #to set the minor ticks
        ax.tick_params(axis = 'y', which = 'minor', labelsize = 7)
        ax.get_yaxis().set_minor_formatter(matplotlib.ticker.StrMethodFormatter("{x:.0f}"))
    if toMark is not None:
        for xi, yi in zip([k-1 for k in x],toMark):
            ax.annotate(str(yi),xy=(xi,yi+1000), fontsize=7)
    if optima is not None:
        for maximum, col in zip(optima, colors):
            plt.plot(x, [maximum for i in range(0, len(x))], c=col)
    ax.set_title(title, fontsize=18)
    ax.set_ylabel(ylabel, fontsize=12, labelpad=12)
    ax.set_xlabel(xlabel, fontsize=12, labelpad=12)
    fig.tight_layout()
    if filename is not None:
        plt.savefig(fname='res/'+filename, transparent=True)
#x
timeGood = [10, 30, 60, 100, 180]
timeBad = [10, 30, 60, 100, 180, 220, 240]
instanceN = np.arange(1, 21, 1)

#y
prof = [5951, 1513, 7484, 22010, 13874, 7003, 73772, 32999, 1710, 1672, 10749, 2287, 26938, 29280, 12351, 14110, 12218, 2081, 4257, 3406]
our = [5951, 1513, 7484, 22010, 13874, 7003, 73772, 32999, 1710, 1672, 10749, 2287, 27332, 28358, 12356, 14343, 12218, 2146, 4067, 2687]

#instances y
inst1 = [5892, 5951, 5951, 5951, 5951]
inst3 = [7391, 7484, 7484, 7484, 7484]
inst6 = [6520, 6609, 6937, 7003, 7003]
inst15= [11716, 12044, 11749, 12356, 12356]
inst16= [12139, 13098, 14035, 14087, 14343]
inst17= [10628, 11438, 11291, 12218, 12218]
inst18= [2039, 2077, 2081, 2092, 2146]
inst19= [2624, 3008, 3086, 2961, 3466, 3737, 4067]
inst20= [1628, 1790, 2127, 2126, 2539, 2634, 2687]

#Colors
colors = ['b', 'r', 'g', 'y', 'b', 'p']
plotComparison(instanceN, np.c_[prof, our], 'Benchmark comparison', 'Objective Function f(x)', 'Instance n°', legend=['Given values', 'Our values'], ytick=np.linspace(5000, 75000, 8), toMark=our, filename='benchmarkComparison.png')
plotLinear(timeGood, [inst1, inst3, inst6], 'Multi-instance good behaviour results', 'Objective Function f(x)', 'Time (s)', xtick=np.linspace(0, 180, 19, endpoint=True), legend=['Instance01', 'Instance03', 'Instance06'], filename='goodInstances.png')
plotLinear(timeBad, [inst19, inst20], 'Multi-instance worst behaviour results', 'Objective Function f(x)', 'Time (s)', xtick=np.linspace(0, 240, 25, endpoint=True), legend=['Instance19', 'Instance20'], optima=[prof[19 - 1], prof[20 - 1]], filename='badInstances.png')
plotLinear(timeGood, [inst18], 'Best improving instance', 'Objective Function f(x)', 'Time (s)', xtick=np.linspace(0, 180, 19, endpoint=True), legend=['Instance18'], optima=[prof[18 - 1]], filename='bestImproving.png')
plotLinear(timeGood, [inst15, inst17], 'Not so much improving instances', 'Objective Function f(x)', 'Time (s)', xtick=np.linspace(0, 180, 19, endpoint=True), legend=['Instance15', 'Instance17'], optima=[prof[15 - 1], prof[17 - 1]], filename='notMuchImproving.png')

#plt.show()