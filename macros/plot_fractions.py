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
import ROOT
import math

print "%1.2f Start with settings" % time.clock()
### SETTINGS
settings = plotBase.StandardSettings()
settings.outputformats = ['png', 'pdf', 'txt', 'dat']
settings.lumi = 2179.0
settings.verbosity = 2
bins = [25,30,36,43,51,61,73,87,104,124,148,177,212,254,304,364,1000]

#### INPUTFILES
print "%1.2f Open files:" % time.clock()
fdata = getROOT.OpenFile(plotBase.GetPath() + "data_Oct12_morebins.root", (settings.verbosity > 1))
fmc = getROOT.OpenFile(plotBase.GetPath() + "MC_Oct12_morebins.root", (settings.verbosity > 1))

def DrawComposition(algoname, bins):
	"""Plot the components of the leading jet"""
	tf, ta, tname = plotBase.makeplot("jetresp_mc")
	# use a finer binning
	ta = plotBase.AxisLabels(ta, 'components')
	
	# Name everything you want and take only the first <nbr> entries of them
	nbr = 5
	labels =     ["CHF",    "NEF",    "NHF",    "CEF",    r"MF $\,$", "PF", "EF"][:nbr]
	colours =    ['Orange','LightSkyBlue','YellowGreen','MediumBlue','Darkred', 'yellow','Blue'][:nbr] #['#DF5F5F','#7DB1FF','#4DB870','#94E9FF','#AD3333','yellow','orange'][:nbr]
	markers =    ['o','x','*','^','d','D','>'][:nbr]
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
	# drop the first bin and calculate the average of data-MC via fit
	fitd = []
	fitderr = []
	fitm = []
	fitmerr = []
	for i in range(len(mcG)):
		mcG[i].dropbin(0)
		dataG[i].dropbin(0)
		fitf = ROOT.TF1("fit1", "1*[0]", 1.0, 1000.0)
		getROOT.SafeGet(fdata,graphnames[i]).Fit(fitf,"QN")
		fitd.append(fitf.GetParameter(0))
		fitderr.append(fitf.GetParError(0))
		getROOT.SafeGet(fmc, graphnames[i]).Fit(fitf, "QN")
		fitm.append(fitf.GetParameter(0))
		fitmerr.append(fitf.GetParError(0))

	# calculate the difference between data and MC
	diff = []
	for i in range(len(mcG)):
	 	diff.append(map(lambda a,b: a-b, dataG[i].y, mcG[i].y))

	#Show a table of values
	if True:
		print "\n      Monte Carlo__" + "_"*7*(len(x)-2) + " Data_________" + "_"*7*(len(x)-2) + " Diff__"
		print "Bins:",
		for bn in x + x: print "%6.0f" % bn,
		for i in range(len(mcG)):
			print "\n%4s:" % (labels[i]),
			for y in mcG[i].y + dataG[i].y: print "%1.4f" % (y),
			print "%+1.2f%% +- %1.2f%%" % ((fitd[i]-fitm[i])*100, (fitderr[i]+fitmerr[i])*100),

	# stack the graphs for both MC and data
	for i in range(len(mcG)-1):
		mcG[i+1].y = map(lambda a,b: a+b, mcG[i+1].y,mcG[i].y)
		dataG[i+1].y = map(lambda a,b: a+b, dataG[i+1].y, dataG[i].y)
	# undo the jet energy correction for fractions, i.e. normalize to 1.0
	for i in range(len(mcG)):
		mcG[i].y = map(lambda a,b: a/b, mcG[i].y,mcG[len(mcG)-1].y)
		dataG[i].y = map(lambda a,b: a/b, dataG[i].y, dataG[len(mcG)-1].y)

	print "\nSum :",
	for y in mcG[-1].y + dataG[-1].y: print "%1.4f" % (y),
	print

	if settings.verbosity>2:
		print mcG[0].x
		print mcG[1].y
		print len(mcG), len(mcG[0].y), len(bins), len(x), len(barWidth)
		print diff
			
	# MC histograms (begin with the last one)
	for i in range(len(mcG)-1,-1,-1):
		plt.bar(x, mcG[i].y, width=barWidth, color=colours[i], edgecolor = None, linewidth=0, label=r"%s: $%1.3f(%d)\, %1.3f(%d)$" % (labels[i],fitd[i],math.ceil(1000*fitderr[i]),fitm[i], math.ceil(1000*fitmerr[i])))
		plt.plot(bins, mcG[i].y+[mcG[i].y[-1]], drawstyle='steps-post', color='black',linewidth=1)
	ta.text(0.625,0.43, r"Data $\quad\;$ MC", va='top', ha='left', transform=ta.transAxes)
	#data points
	for i in range(len(mcG)-1,-1,-1):
		plt.errorbar(dataG[i].x, dataG[i].y, dataG[i].yerr, elinewidth=1,
			marker = markers[i], ms =3, color="black", lw = 0, ecolor=None)

	plt.legend(loc='lower right', numpoints=1)
	ta = plotBase.captions(ta, settings)
	plotBase.Save(tf, algoname + "_fractions", settings, False)

	#plot the difference (with MC error neglected)
	tf, ta, tname = plotBase.makeplot("components_diff")
	tf.subplots_adjust(left=0.15)
	ta = plotBase.captions(ta, settings)
	ta = plotBase.AxisLabels(ta,'components_diff')
	ta.tick_params(which='both')
	ta.grid(True,which="both", ls='-',color='0.75', zorder=0)
	plt.axhline(0.0, color='black', lw=1, zorder=10)


	for i in range(len(mcG)):
		plt.errorbar( mcG[i].x, diff[i], dataG[i].yerr, label = labels[i], fmt = "o", capsize = 2, color = colours[i], zorder=15+i)
		plt.plot([1.0,1000.0], [fitd[i]-fitm[i]]*2, color=colours[i])

	plt.legend(loc='lower right', numpoints=1)
	plotBase.Save(tf, algoname + "_fractions_diff", settings, False)

DrawComposition( "ak5PFJetsL1L2L3CHS", bins)


