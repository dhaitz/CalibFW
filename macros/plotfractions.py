# -*- coding: utf-8 -*-
"""Plot Jet component fractions

   This module plots the component fractions of the leading jet and the
   differences between data and MC.
"""
import math
import copy
from ROOT import TF1, TGraphErrors

import ROOT
import getroot
import plotbase
import plotdatamc

def getvalues(nickname, f, opt, over = 'zpt', changes={}):
    graph = TGraphErrors()
    ch = plotbase.getchanges(opt, {})
    
    # Get list of bin names
    if over == 'zpt':
        bins = getroot.binstrings(opt.bins)
        fitf = TF1("fit1", "1*[0]", 0.0, 1000.0)
    elif over == 'jet1eta':
        bins = getroot.etastrings(opt.eta)
        fitf = TF1("fit1", "[0]", opt.eta[0], opt.eta[-2])
    elif over == 'npv':
        bins = getroot.npvstrings(opt.npv)
        fitf = TF1("fit1", "[0]", 0, 100)

    # read the values from bin folders & store them in a root graph
    for i in range(len(bins)):
        if over == 'zpt':
            ch['bin'] = bins[i]
            ojx = getroot.getobjectfromnick(over, f, ch)
            ojy = getroot.getobjectfromnick(nickname, f, ch)
            graph.SetPoint(i, ojx.GetMean(), ojy.GetMean())
            graph.SetPointError(i, ojx.GetMeanError(), ojy.GetMeanError())
        elif over == 'jet1eta':
            ch['var'] = bins[i]
            ojx = getroot.getobjectfromnick(over, f, ch)
            ojy = getroot.getobjectfromnick(nickname, f, ch)
            if ojx.GetRMS() < opt.eta[i] or ojx.GetRMS() > opt.eta[i+1]:
                graph.SetPoint(i, 0.5*(opt.eta[i+1]+opt.eta[i]), ojy.GetMean())
            else:
                graph.SetPoint(i, ojx.GetRMS(), ojy.GetMean())
            graph.SetPointError(i, ojx.GetMeanError(), ojy.GetMeanError())
        elif over == 'npv':
            ch['var'] = bins[i]
            ojx = getroot.getobjectfromnick(over, f, ch)
            ojy = getroot.getobjectfromnick(nickname, f, ch)
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


def fractions(files, opt, over='zpt', fa=() , subplot=False, changes={}, subtext=""):
    """Plot the components of the leading jet"""
    # Name everything you want and take only the first <nbr> entries of them
    if over == 'zpt': nbr=5
    else: nbr = 7
    labels =     ["CHad", r"$\gamma$       ", "NHad", r"$e$       ", r"$\mu$       ", "HFem", "HFhad"][:nbr]
    colours =    ['Orange', 'LightSkyBlue', 'YellowGreen', 'MediumBlue',
                  'Darkred', 'yellow', 'grey'][:nbr]
    markers =    ['o','x','*','^','d','D','>'][:nbr]
    components = ["chargedhad", "photon", "neutralhad", "electron", "muon", "HFem", "HFhad"][:nbr]
    graphnames = ["jet1" + component + "fraction"
        for component in components]

    algoname = opt.algorithm + opt.correction

    # Get list of graphs from inputfiles
    if opt.verbose:
        print "Loading MC Plots"
    mcG = [getvalues(graphname, files[1], opt, over, changes) for graphname in graphnames]
    if opt.verbose:
        print mcG[0]
        print "Loading Data Plots"
    dataG = [getvalues(graphname, files[0], opt, over, changes) for graphname in graphnames]
    if opt.verbose:
        print "Loading done"

    # get x values and bar widths
    if over == 'zpt':
        bins = copy.deepcopy(opt.bins)
        x = bins[:-1]
        barWidth = []
        for i in range(len(bins)-1):
            barWidth.append(bins[i+1] - bins[i])
    elif over == 'jet1eta':
        bins = copy.deepcopy(opt.eta)
        x = bins[:-1]
        print x
        barWidth = []
        for i in range(len(bins)-1):
            barWidth.append(bins[i+1] - bins[i])
    elif over == 'npv':
        bins = [0]+[b+0.5 for a,b in opt.npv]
        x = bins[:-1]
        print x
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
        print "\n      Monte Carlo__" + "_"*7*(len(x)-1) + " Data_________" + "_"*7*(len(x)-1) + " Diff__"
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

   

    # MC histograms (begin with the last one)

    assert len(mcG[0]) == len(x)
    assert len(bins) == len(mcG[0]) + 1

    if over == 'jet1eta':
        axisname = "jet1abseta"
    else:
        axisname = over

    if subplot is not True:
        fig, ax = plotbase.newplot()
    else:
        fig = fa[0]
        ax = fa[1]


    if subtext is not 'None':
        ax.text(-0.04, 1.01, subtext, va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')


    for i in range(len(mcG)-1,-1,-1):
        if over == 'zpt':
            ax.bar(x, mcG[i].y, width=barWidth, color=colours[i], edgecolor = None, linewidth=0,
         label=r"%s: $%1.3f(%d)\, %1.3f(%d)$" % (labels[i],fitd[i],math.ceil(1000*fitderr[i]),fitm[i], math.ceil(1000*fitmerr[i])))
        else:
            ax.bar(x, mcG[i].y, width=barWidth, color=colours[i], edgecolor = None, linewidth=0,label=labels[i])
        ax.plot(bins, mcG[i].y+[mcG[i].y[-1]], drawstyle='steps-post', color='black',linewidth=1)
        ax.errorbar(mcG[i].x, mcG[i].y, mcG[i].yerr, elinewidth=1, lw=0, color='grey')
    #ax.text(0.64,0.36, r"Data $\quad$MC", va='top', ha='left', transform=ax.transAxes)
   
    
    #data points
    for i in range(len(mcG)-1,-1,-1):
        ax.errorbar(dataG[i].x, dataG[i].y, dataG[i].yerr, elinewidth=1,
            marker=markers[i], ms=3, color="black", lw=0, ecolor=None)
    plotbase.labels(ax, opt, legloc='lower left', frame=True, jet=subplot, sub_plot=subplot, changes=changes)
    plotbase.axislabels(ax, axisname, 'components')

    if subplot is not True:
        plotbase.Save(fig, "fractions_" + over+ "_" + algoname, opt, False)

    if subplot is not True:
        fig, ax = plotbase.newplot()
    else:
        ax=fa[2]
    #plot the difference (with MC error neglected)
    fig.subplots_adjust(left=0.15)
    ax.tick_params(which='both')
    ax.grid(True,which="both", ls='-',color='0.75', zorder=0)
    ax.axhline(0.0, color='black', lw=1, zorder=10)

    for i in range(len(mcG)):
        if over is "jet1eta": label = labels[i]
        else: label = r"%s: $%+1.3f(%d)$" % (labels[i],fitd[i]-fitm[i],math.ceil(1000*(fitderr[i]+fitmerr[i])))
        ax.errorbar( mcG[i].x, diff[i], dataG[i].yerr, label=label,
            fmt="o", capsize=2, color=colours[i], zorder=15+i)
        ax.plot([30.0,1000.0], [fitd[i]-fitm[i]]*2, color=colours[i])
    ax = plotbase.labels(ax, opt, legloc='lower right', frame=True, sub_plot=subplot, jet=subplot, changes=changes)
    ax = plotbase.axislabels(ax, axisname, 'components_diff')
    if subplot is not True: plotbase.Save(fig, "fractions_diff_" + over+ "_" + algoname, opt, False)

#fractions_run: a plot for the time dependence of the various jet components
def fractions_run(files, opt, changes={}, fig_ax=None, subplot=False, diff=False):
    # Name everything you want and take only the first <nbr> entries of them
    nbr = 6
    labels =     ["CHad","photon", "NHad", "electron", "HFem", "HFhad"][:nbr]
    colours =    ['Orange', 'LightSkyBlue', 'YellowGreen', 'MediumBlue',
                  'Darkred', 'grey', 'black'][:nbr]
    markers =    ['o','x','*','^','d','D','>'][:nbr]
    components = ["chargedhad", "photon", "neutralhad", "electron", "HFem", "HFhad"][:nbr] 

    opt_change = copy.deepcopy(opt)

    if diff == True: 
        y_name = 'components_diff'
        title = "fractions_diff_run_"
    else:
        y_name = 'components'
        title = "fractions_run_"

    if fig_ax is None:
        fig, ax = plotbase.newplot(run=True)
    else:
        fig, ax = fig_ax[0], fig_ax[1]

    #changes['var'] = "var_JetEta_2_5to2_964"
    for quantity , label, color, marker in zip(components, labels, colours, markers):
        opt_change.labels = [label]
        opt_change.colors = [color]
        opt_change.style = [marker]
        plotdatamc.datamcplot("jet1%sfraction_run" % quantity, files, opt_change, changes=changes, 
                    xy_names=['run', y_name], fig_axes = (fig, ax), subplot=True, rebin=1000, legloc = 'lower right', runplot_diff = diff)

    if subplot: return

    filename = plotbase.getdefaultfilename(title, opt_change, changes)
    plotbase.Save(fig, filename, opt_change)

#fractions_run for all eta bins in one plot
def fractions_run_jet1eta(files, opt, change={}, diff=False):
    fig, ax = plotbase.newplot(subplots = 7, run=True)

    if diff:
        title = "Time dependence of the leading jet composition data/MC difference for various eta bins  "
        filename = "fractions_diff_run_all-eta_"

    else:
        title = "Time dependence of the leading jet composition for various eta bins  "
        filename = "fractions_run_all-eta_"

    for etavar, ax_element in zip(getroot.etastrings(opt.eta), ax):
        change['var'] = etavar
        fractions_run(files, opt, changes=change, fig_ax = (fig, ax_element), subplot=True, diff=diff)
    del change['var']

    fig.suptitle(title+opt.algorithm+opt.correction, size='xx-large')
    filename = plotbase.getdefaultfilename(filename, opt, change)
    plotbase.Save(fig, filename, opt)

# classic fraction plots
def fractions_zpt (files, opt):
    fractions(files, opt, over='zpt')

def fractions_jet1eta (files, opt):
    fractions(files, opt, over='jet1eta')

def fractions_npv (files, opt):
    fractions(files, opt, over='npv')

#plots for fractions and data/mc difference
def fractions_run_nocuts(files, opt):
    fractions_run(files, opt, changes={'incut':'allevents'})

def fractions_diff_run(files, opt):
    fractions_run(files, opt, diff=True),

def fractions_diff_run_nocuts(files, opt):
    fractions_run(files, opt, changes={'incut':'allevents'}, diff=True)


# plots for comparison of eta bins:
def fractions_run_jet1eta_nocuts(files, opt):
    fractions_run_jet1eta(files, opt, change={'incut':'allevents'})

def fractions_diff_run_jet1eta(files, opt):
    fractions_run_jet1eta(files, opt, diff=True)

def fractions_diff_run_jet1eta_nocuts(files, opt):
    fractions_run_jet1eta(files, opt, change={'incut':'allevents'}, diff=True)


plots = ['fractions_zpt', 'fractions_jet1eta', 
        'fractions_run', 'fractions_run_nocuts', 'fractions_run_jet1eta',
        'fractions_diff_run', 'fractions_diff_run_nocuts', 'fractions_diff_run_jet1eta']


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    import sys
    if len(sys.argv) < 2:
        print "Usage: python macros/plotfractions.py data_file.root mc_file.root"
        exit(0)
    fdata = getroot.openfile(sys.argv[1])
    fmc = getroot.openfile(sys.argv[2])
    bins = getroot.getbins(fdata, [])
    fractions_zpt(fdata, fmc, opt=plotbase.options(bins=bins))


