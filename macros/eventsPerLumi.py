# -*- coding: utf-8 -*-
import getROOT
import math
import matplotlib
import csv
matplotlib.rc('text', usetex = False) 
matplotlib.rc('font', size = 16)
matplotlib.rcParams.update({'axes.linewidth': 0.8 })
import plotBase

### SETTINGS
settings = plotBase.StandardSettings()
settings.outputformats = ['png']
settings.lumi = 2179.0
settings.verbosity = 2
factor = settings.lumi # is being overwritten

def main():
	r, l = readRunsAndLumis()
	ev = getNumberOfEvents(r)
	frac, err = getEventsPerLumi(r, l, ev)
	eventsOverLumi(frac, err, average=sum(frac)/len(frac))

### read cvs file
# the file.csv is produced via
def readRunsAndLumis(filename="data/lumiPerRun.csv"):
	csvfile = csv.reader(open(filename, 'rb'), delimiter=',')
	runs = []
	lumis = []
	for line in csvfile:
		if line[0]=='Run': continue
		runs.append(int(line[0]))
		lumis.append(float(line[4])/1e6)
	return runs, lumis


def getNumberOfEvents(runs,filename="data_Oct12.root", histoname='NoBinning_incut/events_per_run_ak5PFJetsL1L2L3CHS_hist'):
	fdata = getROOT.OpenFile(plotBase.GetPath() + filename, (settings.verbosity>1))
	histo = getROOT.SafeGet(fdata,histoname)
	events = []
	for run in runs:
		eventCount = int(histo.GetBinContent(histo.FindBin(run)))
		events.append(eventCount)
	return events


# divide events / lumi after summing over a couple of runs
def getEventsPerLumi(runs, lumis, events, sumOverRuns = 10):
	fractions = []
	errors =[]
	sumev = 0.0
	sumlu = 0.0
	for i in range(len(runs)):
		sumev += events[i]
		sumlu += lumis[i]
		if i%sumOverRuns==sumOverRuns-1:
			fractions.append(sumev/sumlu)
			if sumev<1.0: print "No events in runs around", runs[i]
			errors.append(math.sqrt(max(1.0,sumev)/sumlu + 0.1*sumev/sumlu))
			sumev = sumlu = 0.0
			print runs[i], ":", fractions[-1]
	return fractions, errors


# plot the number of events/lumi over run periods
def eventsOverLumi(fractions, errors, filename="eventsPerLumi", average=11.76):
	fig, ax, name = plotBase.makeplot('runlist')
	ax.axhline(y=average, color='green')
	ax.errorbar(range(len(fractions)), fractions, errors, fmt = 'o', color = 'black')
	plotBase.AxisLabels(ax,'runlist', 'Z')
	ax.set_xlim(0,len(fractions))
	plotBase.captions(ax,settings)
	plotBase.tags(ax, 'Private work', 'Joram Berger')
	ax.legend(loc='center right', numpoints=1, frameon=False)
	plotBase.Save(fig, filename, settings, False)
	print average

if __name__ == "__main__":
	main()
