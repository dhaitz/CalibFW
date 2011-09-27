# -*- coding: utf-8 -*-
import getROOT
import sys, os, math #, kein pylab!
import time
import numpy as np
import matplotlib
matplotlib.rc('text', usetex = False) 
matplotlib.rc('font', size = 16)
#matplotlib.rc('axes', linewidth = 20)
matplotlib.rcParams.update({
	'axes.linewidth': 0.8 # thickness of main box lines 
#	'patch.linewidth': 1.5, # thickness of legend pictures and border
#	'grid.linewidth': 1.3, # thickness of grid lines
#	'lines.linewidth': 2.5, # thickness of error bars
#	'lines.markersize': 2.5, # size of markers
#	'legend.fontsize': 20,
#	'xtick.labelsize': 18,
#	'ytick.labelsize': 18,
#	'text.fontsize': 16,
#	'font.size': 18
})
#import matplotlib.pyplot as plt
import plotBase

### SETTINGS
settings = plotBase.StandardSettings()
settings.outputformats = ['png', 'pdf', 'svg', 'txt', 'dat']
settings.outputformats = ['png']
settings.lumi = 2179.0
settings.verbosity = 2
factor = settings.lumi # is being overwritten
factor10 = 36*0.001*0.75
mc11color = '#CBDBF9'#FireBrick'
edgeColor = '#0033CC'
mc10color = 'MidnightBlue'
data11color = 'black'
data10color = 'gray'

###quantity R(ptZ) pt eta phi
###set algo, cone size, #pv, L1L2L3, 
###data/mc(generator,tune)
###document: serif,LaTeX ; slides: sans serif 
#mpl.rc('text', usetex=True)#does not work on the naf! no latex there
#matplotlib.rc('font', family='serif')
#mpl.pylab.rcParams['legend.loc'] = 'best' rc lines.color linewidth font family weight size
# most settings should go in plotBase
# different plots: data/mc (p4, response); extrapolation; special: Z-scale, npu, zeitabhängig, flavour, pf_fractions; cuts, cuteff; crosschecks: 1-2/more pu, eta (endcap, HF), jet algo, jet size, Calo/PF, Pythia/Herwig
# run_it( True, "Text", False, "Set of plots")

#### INPUTFILES
print "%1.2f Open files:" % time.clock()
fdata = getROOT.OpenFile(plotBase.GetPath() + "data_Sept23.root", (settings.verbosity>1))

print "%1.2f Do plots ..." % time.clock()

### cvs file lesen
import csv
csvfile = csv.reader(open('data/lumiPerRun.csv', 'rb'), delimiter=',')
runs = []
lumis = []
for line in csvfile: 
	if line[0]=='Run': continue
	runs.append(int(line[0]))
	lumis.append(float(line[4])/1e6)

print len(runs), len(lumis)

histo = getROOT.SafeGet(fdata,'NoBinning_incut/events_per_run_ak5PFJetsL1L2L3CHS_hist')
events = []
fractions = []
errors =[]
for run in runs:
	eventCount = int(histo.GetBinContent(histo.FindBin(run)))
	events.append(eventCount)
#	print eventCount

print len(events)

for i in range(len(runs)):
	fractions.append(events[i]/lumis[i])
	errors.append(math.sqrt(max(1,events[i]))/lumis[i] + 0.1*events[i]/lumis[i]*lumis[i])
	print run, ":", fractions[i]

def eventsOverLumi():
	fzeta, azeta, plt_name = plotBase.makeplot('runlist')
	azeta = plotBase.AxisLabels(azeta,'runlist', 'Z')
	#l = plt.axhline(y=91.19, color='0.5', alpha=0.5)

	#histo_data.x.pop()
#	histo0 = azeta.fill_between(histo_mc.x,histo_mc.y, facecolor=settings.mcColor, label ='')
	histo2 = azeta.errorbar(range(len(runs)), fractions,errors)
	#def fitfunc(x):
	#	return  a/(c*c*g*g+(x*x-c*c)*(x*x-c*c))
	#x = mcdata['x']#numpy.arange(60.0, 120.0, .06)
	#y = fitfunc(x)

	azeta = plotBase.captions(azeta,settings)
	azeta = plotBase.tags(azeta, 'Private work', 'Joram Berger')
	azeta.legend(loc='center right', numpoints=1, frameon=False)

	#plt.minorticks_on()

	plotBase.Save(fzeta,'eventsPerLumi', settings, False)

# The actual plotting starts here:
#List of plots to do - leave empty for all plots
plots = [eventsOverLumi]

if len(plots)==0:
	plots = []
for plot in plots:
	print "New plot:",
	plot()


print "%1.2f Done." % time.clock()
