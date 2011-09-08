# -*- coding: utf-8 -*-

import getROOT
import time
import numpy as np
import matplotlib 
import matplotlib.pyplot as plt 
import matplotlib.font_manager as font_man

matplotlib.rc('text', usetex=True) 
matplotlib.rc('font', size=16)
import plotBase

print "%1.2f Start with settings" % time.clock()
### SETTINGS
settings = plotBase.StandardSettings()
settings.outputformats = ['png', 'pdf', 'txt', 'dat']
settings.lumi = 2096
settings.verbosity = 2
bins = [0, 30, 45, 60, 80, 105, 140, 1000]

#### INPUTFILES
print "%1.2f Open files:" % time.clock()
fdata = getROOT.OpenFile(plotBase.GetPath() + "data_Sept08.root", (settings.verbosity > 1))
fmc = getROOT.OpenFile(plotBase.GetPath() + "MC_Sept08.root", (settings.verbosity > 1))

def DrawComposition(algoname, bins):
	"""Plot the components of the leading jet"""
	tf, ta, tname = plotBase.makeplot("jetresp_mc")
	# use a finer binning
	ta = plotBase.AxisLabels(ta, 'components')
	
	# Name everything you want and take only the first <nbr> entries of them
	nbr = 5
	labels =     ["CHF",    "NEF",    "NHF",    "CEF",    "MF", "PF", "EF"][:nbr]
	colours =    ['#DF5F5F','#7DB1FF','#4DB870','#94E9FF','#AD3333','yellow','orange'][:nbr]
	markers =    ['_']*nbr #['o','x','*','^','d','D','>'][:nbr]
	components = ["chargedhadron", "neutralem", "neutralhadron", "chargedem", "muon", "photon", "electron"][:nbr]
	graphnames = ["jet1_" + component + "energy_fraction_" + algoname + "_graph" 
		for component in components]

	# Get list of graphs from inputfiles
	mcG = [ getROOT.SafeConvert(fmc,graphname) for graphname in graphnames ]
	dataG = [ getROOT.SafeConvert(fdata,graphname) for graphname in graphnames ]

	# get x values and bar widths
	x = bins[:-1]	
	barWidth = []
	for i in range(len(bins)-1):
		barWidth.append(bins[i+1] - bins[i])
	# drop the first bin
	for i in range(len(mcG)):
		mcG[i].dropbin(0)
		dataG[i].dropbin(0)
	# calculate the difference between data and MC
	diff = []
	for i in range(len(mcG)):
	 	diff.append(map(lambda a,b: a-b, dataG[i].y, mcG[i].y))
	
	#Show a table of values
	if True:
		print "\n      Monte Carlo________________ Data_______________________"
		print "Bins:",
		for bn in x + x: print "%6.0f" % bn,
		for i in range(len(mcG)):
			print "\n%4s:" % (labels[i]),
			for y in mcG[i].y + dataG[i].y: print "%1.4f" % (y),
	print	

#	def stack(histo, bottomhisto): # alternative for explicit map
#		histo.y = map(lambda a,b: a+b, histo.y, bottomhisto.y)
	# stack the graphs for both MC and data
	for i in range(len(mcG)-1):
		mcG[i+1].y = map(lambda a,b: a+b, mcG[i+1].y,mcG[i].y)
		dataG[i+1].y = map(lambda a,b: a+b, dataG[i+1].y, dataG[i].y)

	# Tell me ...
	if False:
		print mcG[0].x
		print mcG[1].y
		print len(mcG), len(mcG[0].y), len(bins), len(x), len(bar_width)
		print diff
			
	# MC histograms (begin with the last one)
	for i in range(len(mcG),-1,-1):
		plt.bar(x, mcG[i].y, width=barWidth, color=colours[i], label=labels[i])

	#data points
	for i in range(len(mcG),-1,-1):
		plt.errorbar(dataG[i].x, dataG[i].y, dataG[i].yerr, elinewidth=2, 
			marker = markers[i], color="black", lw = 0, ecolor=None)

	plt.legend(loc='lower right', numpoints=1)
	plotBase.Save(tf, algoname + "_fractions", settings, False)

	#plot the difference (with MC error neglected)
	tf, ta, tname = plotBase.makeplot("components_diff")
	ta = plotBase.AxisLabels(ta,'components_diff')

	for i in range(len(mcG)):
		plt.errorbar( mcG[i].x, diff[i], dataG[i].yerr, label = labels[i], fmt = "o", capsize = 2, color = colours[i])

	plt.axhline(0.0, color='black', lw=1)
	plt.legend(loc='lower right', numpoints=1)
	plotBase.Save(tf, algoname + "_fraction_diff", settings, False)

DrawComposition( "ak5PFJetsL1", bins)


