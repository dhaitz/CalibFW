# -*- coding: utf-8 -*-
"""Plot Jet component fractions

   This module plots the component fractions of the leading jet and the
   differences between data and MC.
"""
import math
import copy
from ROOT import TF1, TGraphErrors

import getroot
import plotbase

def getvalues(nickname, f, opt):
    graph = TGraphErrors()
    fitf = TF1("fit1", "1*[0]", 1.0, 1000.0)
    changes = getroot.createchanges(opt)
    bins = plotbase.binstrings(opt.bins)
    for i in range(len(opt.bins)-2):
        # read the values from pt-bin folders
        changes['bin'] = bins[i+1]
        ojx = getroot.getobjectfromnick('z_pt', f, changes)
        ojy = getroot.getobjectfromnick(nickname, f, changes)
        # store them in a root graph
        graph.SetPoint(i, ojx.GetMean(), ojy.GetMean())
        graph.SetPointError(i, ojx.GetMeanError(), ojy.GetMeanError())
    # fit a horizontal line
    graph.Fit(fitf,"QN")
    # return plot (fit values are stored in the last point of result)
    result = getroot.root2histo(graph)
    result.x.append(0)
    result.xc.append(0)
    result.y.append(fitf.GetParameter(0))
    result.yerr.append(fitf.GetParError(0))
    return result


def fractions(fdata, fmc, opt):
    """Plot the components of the leading jet"""
    # Name everything you want and take only the first <nbr> entries of them
    nbr = 5
    labels =     ["CHF", "NEF", "NHF", "CEF", r"MF $\,$", "PF", "EF"][:nbr]
    colours =    ['Orange', 'LightSkyBlue', 'YellowGreen', 'MediumBlue',
                  'Darkred', 'yellow', 'Blue'][:nbr]
    markers =    ['o','x','*','^','d','D','>'][:nbr]
    components = ["charged_had", "neutral_em", "neutral_had", "charged_em", 
                  "muon", "photon", "electron"][:nbr]
    graphnames = ["jet1_" + component + "_fraction"
        for component in components]

    algoname = opt.algorithm + opt.correction

    # Get list of graphs from inputfiles
    if opt.verbose:
        print "Loading MC Plots"
    mcG = [getvalues(graphname, fmc, opt) for graphname in graphnames]
    if opt.verbose:
        print mcG[0]
        print "Loading Data Plots"
    dataG = [getvalues(graphname, fdata, opt) for graphname in graphnames]
    if opt.verbose:
        print "Loading done"

    # get x values and bar widths
    bins = copy.deepcopy(opt.bins)
    x = bins[1:-1]
    if bins[0] == 0:
        bins.pop(0)
    barWidth = []
    for i in range(len(bins)-1):
        barWidth.append(bins[i+1] - bins[i])
    # separate fit values, stored in the last entry (see getvalues)
    fitd = []
    fitderr = []
    fitm = []
    fitmerr = []
    for i in range(len(mcG)):
        fitd.append(dataG[i].y[-1])
        fitderr.append(dataG[i].yerr[-1])
        fitm.append(mcG[i].y[-1])
        fitmerr.append(mcG[i].yerr[-1])
        mcG[i].dropbin(-1)
        dataG[i].dropbin(-1)

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
            print "\n%4s:" % (labels[i].replace("$\,$","")),
            for y in mcG[i].y + dataG[i].y: print "%1.4f" % (y),
            print "%+1.2f%% +- %1.2f%%" % ((fitd[i]-fitm[i])*100, (fitderr[i]+fitmerr[i])*100),

    # stack the graphs for both MC and data
    for i in range(len(mcG)-1):
        mcG[i+1].y = map(lambda a,b: a+b, mcG[i+1].y,mcG[i].y)
        dataG[i+1].y = map(lambda a,b: a+b, dataG[i+1].y, dataG[i].y)

    print "\nSum :",
    for y in mcG[-1].y + dataG[-1].y: print "%1.4f" % (y),
    print

    if opt.verbose:
        print mcG[0].x
        print mcG[1].y
        print len(mcG), len(mcG[0].y), len(bins), len(x), len(barWidth)
        print diff

    fig, ax = plotbase.newplot()
    # MC histograms (begin with the last one)
    assert len(mcG[0]) == len(x)
    assert len(bins) == len(mcG[0]) + 1
    for i in range(len(mcG)-1,-1,-1):
        ax.bar(x, mcG[i].y, width=barWidth, color=colours[i], edgecolor = None, linewidth=0,
            label=r"%s: $%1.3f(%d)\, %1.3f(%d)$" % (labels[i],fitd[i],math.ceil(1000*fitderr[i]),fitm[i], math.ceil(1000*fitmerr[i])))
        ax.plot(bins, mcG[i].y+[mcG[i].y[-1]], drawstyle='steps-post', color='black',linewidth=1)
        ax.errorbar(mcG[i].x, mcG[i].y, mcG[i].yerr, elinewidth=1, lw=0, color='grey')
    ax.text(0.64,0.36, r"Data $\quad$MC", va='top', ha='left', transform=ax.transAxes)
    #data points
    for i in range(len(mcG)-1,-1,-1):
        ax.errorbar(dataG[i].x, dataG[i].y, dataG[i].yerr, elinewidth=1,
            marker=markers[i], ms=3, color="black", lw=0, ecolor=None)
    plotbase.labels(ax, opt, legloc='lower right', frame=True)
    plotbase.axislabels(ax, 'z_pt_log', 'components')
    plotbase.Save(fig, "fractions_" + algoname, opt, False)

    #plot the difference (with MC error neglected)
    fig, ax = plotbase.newplot()
    fig.subplots_adjust(left=0.15)
    ax.tick_params(which='both')
    ax.grid(True,which="both", ls='-',color='0.75', zorder=0)
    ax.axhline(0.0, color='black', lw=1, zorder=10)

    for i in range(len(mcG)):
        ax.errorbar( mcG[i].x, diff[i], dataG[i].yerr, label=labels[i], 
            fmt="o", capsize=2, color=colours[i], zorder=15+i)
        ax.plot([1.0,1000.0], [fitd[i]-fitm[i]]*2, color=colours[i])
    ax = plotbase.labels(ax, opt, legloc='lower left', frame=True)
    ax = plotbase.axislabels(ax,'z_pt_log', 'components_diff')
    plotbase.Save(fig, "fractions_diff_" + algoname, opt, False)


plots = ['fractions']


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    import sys
    if len(sys.argv) < 2:
        print "Usage: python macros/plotfractions.py data_file.root mc_file.root"
        exit(0)
    fdata = getroot.openfile(sys.argv[1])
    fmc = getroot.openfile(sys.argv[2])
    bins = getroot.getbins(fdata, [])
    fractions(fdata, fmc, opt=plotbase.options(bins=bins))
