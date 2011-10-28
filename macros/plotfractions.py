# -*- coding: utf-8 -*-
"""
"""
import math
from ROOT import TF1

import getroot
import plotbase


def fractions(fdata, fmc, opt):
    """Plot the components of the leading jet"""
    algoname = opt.algorithm + opt.correction

    # Name everything you want and take only the first <nbr> entries of them
    nbr = 5
    labels =     ["CHF",    "NEF",    "NHF",    "CEF",    r"MF $\,$", "PF", "EF"][:nbr]
    colours =    ['Orange','LightSkyBlue','YellowGreen','MediumBlue','Darkred', 'yellow','Blue'][:nbr]
    markers =    ['o','x','*','^','d','D','>'][:nbr]
    components = ["chargedhadron", "neutralem", "neutralhadron", "chargedem", "muon", "photon", "electron"][:nbr]
    graphnames = ["jet1_" + component + "energy_fraction_" + algoname + "_graph"
        for component in components]

    # Get list of graphs from inputfiles
    mcG = [getroot.gethisto(graphname, fmc) for graphname in graphnames]
    dataG = [getroot.gethisto(graphname, fdata) for graphname in graphnames]

    # get x values and bar widths
    x = opt.bins[1:-1]
    bins = opt.bins
    if bins[0] == 0:
        bins.pop(0)
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
        fitf = TF1("fit1", "1*[0]", 1.0, 1000.0)
        getroot.getobject(fdata, graphnames[i]).Fit(fitf,"QN")
        fitd.append(fitf.GetParameter(0))
        fitderr.append(fitf.GetParError(0))
        getroot.getobject(fmc, graphnames[i]).Fit(fitf, "QN")
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
            print "\n%4s:" % (labels[i].replace("$\,$","")),
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

    if opt.verbose:
        print mcG[0].x
        print mcG[1].y
        print len(mcG), len(mcG[0].y), len(bins), len(x), len(barWidth)
        print diff

    fig, ax = plotbase.newplot()
    # MC histograms (begin with the last one)
    for i in range(len(mcG)-1,-1,-1):
        ax.bar(x, mcG[i].y, width=barWidth, color=colours[i], edgecolor = None, linewidth=0,
            label=r"%s: $%1.3f(%d)\, %1.3f(%d)$" % (labels[i],fitd[i],math.ceil(1000*fitderr[i]),fitm[i], math.ceil(1000*fitmerr[i])))
        ax.plot(bins, mcG[i].y+[mcG[i].y[-1]], drawstyle='steps-post', color='black',linewidth=1)
    ax.text(0.625,0.43, r"Data $\quad\;$ MC", va='top', ha='left', transform=ax.transAxes)
    #data points
    for i in range(len(mcG)-1,-1,-1):
        ax.errorbar(dataG[i].x, dataG[i].y, dataG[i].yerr, elinewidth=1,
            marker = markers[i], ms =3, color="black", lw = 0, ecolor=None)
    plotbase.labels(ax, opt, legloc='lower right')
    plotbase.axislabel(ax, 'components')
    plotbase.Save(fig, algoname + "_fractions", opt, False)

    #plot the difference (with MC error neglected)
    fig, ax = plotbase.newplot()
    fig.subplots_adjust(left=0.15)
    ax.tick_params(which='both')
    ax.grid(True,which="both", ls='-',color='0.75', zorder=0)
    ax.axhline(0.0, color='black', lw=1, zorder=10)

    for i in range(len(mcG)):
        ax.errorbar( mcG[i].x, diff[i], dataG[i].yerr, label = labels[i], fmt = "o", capsize = 2, color = colours[i], zorder=15+i)
        ax.plot([1.0,1000.0], [fitd[i]-fitm[i]]*2, color=colours[i])
    ax = plotbase.labels(ax, opt, legloc='lower right')
    ax = plotbase.axislabel(ax,'components_diff')
    plotbase.Save(fig, algoname + "_fractions_diff", opt, False)


plots = ['fractions']


if __name__ == "__main__":
    fdata = getROOT.openFile(plotbase.GetPath() + "data_Oct19.root")
    fmc = getROOT.openFile(plotbase.GetPath() + "pythia_Oct19.root")
    bins = plotbase.guessBins(fdata, [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]) #binning must be after file open. plots do this later: if bins[0] == 0 bins.pop(0)
    fractions(fdata, fmc, opt=plotbase.commandlineOptions(bins=bins))
