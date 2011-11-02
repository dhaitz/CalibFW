# -*- coding: utf-8 -*-
import math
import matplotlib
import csv
matplotlib.rc('text', usetex=False)
matplotlib.rc('font', size=16)
matplotlib.rcParams.update({'axes.linewidth': 0.8})

import getroot
import plotbase


def main():
    opt = plotbase.options(files=[plotbase.getpath() + "data_Oct19.root"])
    r, l = readRunsAndLumis()
    ev = getNumberOfEvents(r, opt)
    frac, err = getEventsPerLumi(r, l, ev)
    eventsOverLumi(frac, err, opt, average=sum(frac) / len(frac))


def readRunsAndLumis(filename="data/lumiPerRun.csv"):
    """Read lumis per run from csv file

    The file lumiPerRun.csv is produced via
    lumiCalc2.py -hltpath HLT_Mu13_Mu8 -i json.txt -o lumiPerRun.csv overview

    """
    csvfile = csv.reader(open(filename, 'rb'), delimiter=',')
    runs = []
    lumis = []
    for line in csvfile:
        if line[0] == 'Run':
            continue
        runs.append(int(line[0]))
        lumis.append(float(line[4]) / 1e6)  # conversion to /pb
    return runs, lumis


def getNumberOfEvents(runs, opt,
        histoname='NoBinning_incut/events_per_run_ak5PFJetsL1L2L3CHS_hist'):
    fdata = getroot.openfile(opt.data, opt.verbose)
    histo = getroot.getobject(fdata, histoname)
    events = []
    for run in runs:
        eventCount = int(histo.GetBinContent(histo.FindBin(run)))
        events.append(eventCount)
    return events


def getEventsPerLumi(runs, lumis, events, sumOverRuns=10):
    """divide events by lumi after summing over a couple of runs"""
    fractions = []
    errors = []
    sumev = 0.0
    sumlu = 0.0
    print "Run*   : Lumi      (* {0} runs up to this one)".format(sumOverRuns)
    for i in range(len(runs)):
        sumev += events[i]
        sumlu += lumis[i]
        if i % sumOverRuns == sumOverRuns - 1:
            fractions.append(sumev / sumlu)
            if sumev < 1.0:
                print "No events in runs around", runs[i]
            errors.append(math.sqrt(max(1.0, sumev) / sumlu +  # poisson error
                                    0.1 * sumev / sumlu))      # lumi error
            sumev = sumlu = 0.0
            print "{0:7d}: {1:6.3f}".format(runs[i], fractions[-1])
    return fractions, errors


def eventsOverLumi(fractions, errors, opt, filename="eventsPerLumi",
                   average=11.76):
    """plot the number of events/lumi over run periods"""
    print "Average: {0:6.3f}".format(average)
    fig, ax = plotbase.newplot()
    ax.axhline(y=average, color='green')
    ax.errorbar(range(len(fractions)), fractions, errors, fmt='o',
                color='black', label="Data")
    ax.set_xlim(0, len(fractions))
    plotbase.labels(ax, opt, legloc='center right', frame=False)
    plotbase.axislabel(ax, 'runlist', 'Z')
    plotbase.Save(fig, filename, opt)


if __name__ == "__main__":
    main()
