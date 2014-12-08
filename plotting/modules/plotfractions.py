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
import plot1d
import fit

def getvalues(nickname, f, opt, over = 'zpt', settings=None, changes=None):

    graph = TGraphErrors()
    settings = plotbase.applyChanges(settings, changes)
  
    graph = getroot.getgraph(over, nickname, f, opt, settings=settings, 
                                                                changes=changes)

    # fit a horizontal line
    par, parerror = fit.fitline(graph, limits=settings['x'])[:2]

    # return plot (fit values are stored in the last point of result)
    result = getroot.root2histo(graph)
    #result.x.append(0)
    #result.xc.append(0)
    #result.y.append(par)
    #result.yerr.append(parerror)
    return result


def fractions(files, opt, over='npv', fig_axes=None, settings=None, changes=None):
    """Plot the components of the leading jet"""
    # Name everything you want and take only the first <nbr> entries of them
    if over == 'jet1abseta': nbr=7
    else: nbr = 5
    labels =     ["CHad", r"$\gamma$       ", "NHad", r"$e$       ",
                                        r"$\mu$       ", "HFem", "HFhad"][:nbr]
    colours =    ['Orange', 'LightSkyBlue', 'YellowGreen', 'MediumBlue',
                  'Darkred', 'yellow', 'grey'][:nbr]
    markers =    ['o','x','*','^','d','D','>'][:nbr]
    components = ["chargedhad", "photon", "neutralhad", "chargedem", "muon",
                                                         "HFem", "HFhad"][:nbr]
    graphnames = ["jet1" + component + "fraction" for component in components]


    settings = plotbase.getSettings(opt, changes=changes, settings=None, 
                                                quantity="components_%s" % over)
    

    algoname = settings['algorithm'] + settings['correction']


    # get x values and bar widths
    if over == 'zpt':
        bins = copy.deepcopy(opt.zbins)
        fit = True
        x = bins[:-1]
        barWidth = []
        for i in range(len(bins)-1):
            barWidth.append(bins[i+1] - bins[i])
    elif over == 'jet1abseta':
        settings['alleta'] = True
        bins = copy.deepcopy(opt.eta)
        fit = False
        x = bins[:-1]
        barWidth = []
        for i in range(len(bins)-1):
            barWidth.append(bins[i+1] - bins[i])
    elif over == 'npv':
        bins = [0]+[b+0.5 for a,b in opt.npv]
        fit = True
        x = bins[:-1]
        barWidth = []
        for i in range(len(bins)-1):
            barWidth.append(bins[i+1] - bins[i])
    elif over == 'recogen':
        bins = [0, 0.5, 1, 1.5, 2]
        fit = False
        x = bins[:-1]
        barWidth = []
        for i in range(len(bins)-1):
            barWidth.append(bins[i+1] - bins[i])

    # Get list of graphs from inputfiles
    if opt.verbose:
        print "Loading MC Plots"
    mcG = [getvalues(graphname, files[1], opt, over,settings, changes) for graphname in graphnames]
    if opt.verbose:
        print mcG[0]
        print "Loading Data Plots"
    dataG = [getvalues(graphname, files[0], opt, over,settings, changes) for graphname in graphnames]
    if opt.verbose:
        print "Loading done"

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

    if settings['subplot'] is not True:
        fig, ax = plotbase.newPlot()
    else:
        fig = fig_axes[0]
        ax = fig_axes[1]


    if settings['subtext'] is not None:
        ax.text(-0.04, 1.01, settings['subtext'], va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')

    # draw the MC bars:
    for i in range(len(mcG)-1,-1,-1):
        if over == 'zpt':
            ax.bar(x, mcG[i].y, width=barWidth, color=colours[i], edgecolor = None, linewidth=0,
         label=r"%s: $%1.3f(%d)\, %1.3f(%d)$" % (labels[i],fitd[i],math.ceil(1000*fitderr[i]),fitm[i], math.ceil(1000*fitmerr[i])))
        else:
            ax.bar(x, mcG[i].y, width=barWidth, color=colours[i], edgecolor = None, linewidth=0,label=labels[i])
        ax.plot(bins, mcG[i].y+[mcG[i].y[-1]], drawstyle='steps-post', color='black',linewidth=1)
        ax.errorbar(mcG[i].x, mcG[i].y, mcG[i].yerr, elinewidth=1, lw=0, color='grey')
       
        #data points
        ax.errorbar(dataG[i].x, dataG[i].y, dataG[i].yerr, elinewidth=1,
            marker=markers[i], ms=3, color="black", lw=0, ecolor=None)

    plotbase.labels(ax, opt, settings, settings['subplot'])
    plotbase.axislabels(ax, settings['xynames'][0], settings['xynames'][1], 
                                                            settings=settings)
    plotbase.setAxisLimits(ax, settings)

    if settings['subplot'] is not True:
        settings['filename'] = plotbase.getDefaultFilename(
                                    "fractions_%s" % over, opt, settings)
        plotbase.Save(fig, settings)
        fig, ax = plotbase.newPlot()
    else:
        ax=fig_axes[2]


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
        if fit: ax.plot([30.0*(over == 'zpt'), 1000.0], [fitd[i]-fitm[i]]*2, color=colours[i])

    settings['xynames'][1] = 'components-diff'
    settings['y'] = plotbase.getaxislabels_list(settings['xynames'][1])[:2]
    plotbase.labels(ax, opt, settings, settings['subplot'])
    plotbase.axislabels(ax, settings['xynames'][0], settings['xynames'][1], 
                                                            settings=settings)
    plotbase.setAxisLimits(ax, settings)

    if settings['subplot'] is not True:
        settings['filename'] = "_diff_".join(settings['filename'].split("_",1))
        plotbase.Save(fig, settings)

#fractions_run: a plot for the time dependence of the various jet components
def fractions_run(files, opt, changes=None, fig_ax=None, subplot=False, 
                                                    diff=False, response=False, nbr=4):
    # Name everything you want and take only the first <nbr> entries of them
    #nbr = 4
    labels =     ["CHad","photon", "NHad", "electron", "HFem", "HFhad"][:nbr]
    colours =    ['Orange', 'LightSkyBlue', 'YellowGreen', 'MediumBlue',
                  'Darkred', 'grey', 'black'][:nbr]
    markers =    ['o','x','*','^','d','D','>'][:nbr]
    components = ["chargedhad", "photon", "neutralhad", "chargedem", "HFem",
                                                                  "HFhad"][:nbr]

    if changes is None:
        changes = {}
    if diff:
        quantity = 'components-diff_run'
        changes['xynames'] = ["run", "components-diff"]
        changes['run'] = 'diff'
    else:
        quantity = 'components_run'
        changes['xynames'] = ["run", "components"]
        changes['run'] = True
        files = [files[0]]

    if response:
        changes['xynames'][1] += "-response"
        suffix = "*ptbalance"
        quantity = quantity.replace('components', 'components-response')
    else:
        suffix = ""

    settings = plotbase.getSettings(opt, changes=changes, settings=None, 
                                                              quantity=quantity)

    if fig_ax is None:
        fig, ax = plotbase.newPlot(run=True)
    else:
        fig, ax = fig_ax[0], fig_ax[1]
    
    changes = {}
    changes['subplot'] = True
    changes['legloc'] = 'lower right'
    changes['fit']=True
   

    for quant, label, color, marker in zip(components, labels, colours, 
                                                                       markers):
        changes['labels'] = [label]
        changes['colors'] = [color]
        changes['marker'] = [marker]
        plot1d.datamcplot("(jet1%sfraction%s)_run" % (quant, suffix), 
             files, opt, fig_axes=(fig, ax), changes=changes, settings=settings)

    if subplot:
        return
    else:
        settings['filename'] = plotbase.getDefaultFilename(quantity, opt, 
                                                                       settings)
        plotbase.Save(fig, settings)


#fractions_run for variations
def fractions_run_all(files, opt, change={}, diff=False, response=False):

    for quantity, variation_strings, var_bin in zip(['jet1eta', 'zpt', 'npv'], [getroot.etastrings(opt.eta), getroot.binstrings(opt.zbins), getroot.npvstrings(opt.npv)], ['var', 'bin', 'var']):

        fig, ax = plotbase.newPlot(subplots = len(variation_strings), run=True)

        if diff:
            title = "Time dependence of the leading jet composition data/MC difference for various %s bins  " % quantity
            filename = "fractions_diff_run_all-%s_" % quantity

        else:
            title = "Time dependence of the leading jet composition for various %s bins  " % quantity
            filename = "fractions_run_all-%s_" % quantity

        if response:
            filename = filename.replace("_run", "_run_response")

        for var, ax_element in zip(variation_strings, ax):
            change[var_bin] = var
            fractions_run(files, opt, changes=change, fig_ax = (fig, ax_element), subplot=True, diff=diff, response=response)
        del change[var_bin]

        fig.suptitle(title+opt.algorithm+opt.correction, size='xx-large')
        filename = plotbase.getDefaultFilename(filename, opt, change)
        plotbase.Save(fig, {})

# classic fraction plots
def fractions_zpt (files, opt):
    fractions(files, opt, over='zpt')
    
    
def fractions_recogen(files, opt):
    fractions(files, opt, over='recogen')

def fractions_jet1abseta (files, opt):
    fractions(files, opt, over='jet1abseta')

def fractions_npv (files, opt):
    fractions(files, opt, over='npv')

# run plots for fractions and data/mc difference
def fractions_diff_run(files, opt):
    fractions_run(files, opt, diff=True)

def fractions_diff_run_nocuts(files, opt):
    fractions_run(files, opt, changes={'incut':'allevents'}, diff=True)


# run plots for comparison of variations:
def fractions_diff_run_all(files, opt):
    fractions_run_all(files, opt, diff=True)


# run plots for comparison of variations weighted by RESPONSE:
def fractions_run_response(files, opt):
    fractions_run(files, opt, response=True)

def fractions_run_response_diff(files, opt):
    fractions_run(files, opt, response=True, diff=True)

def fractions_run_response_all(files, opt):
    fractions_run_all(files, opt, response=True)

def fractions_run_response_diff_all(files, opt):
    fractions_run_all(files, opt, response=True, diff=True)

def flavour_composition_eta(files, opt, changes=None):
    """MC flavour composition vs leading jet eta."""   
    flavour_composition(files, opt, changes, x="jet1abseta")

def flavour_composition(files, opt, changes=None, x="zpt"): 
    """MC flavour composition vs Z pT."""   
    flavourdef = "physflavour"

    quantity="_".join([flavourdef, x])
    settings = plotbase.getSettings(opt, changes, settings=None, quantity=quantity) 


    flist = ["(flavour>0&&flavour<4)", # uds
        "((flavour>0&&flavour<4)|| flavour==4)", #c
        "((flavour>0&&flavour<4)|| flavour==4 || flavour==5)", #b
        "((flavour>0&&flavour<4)|| flavour==4 || flavour==5 || flavour==21)", #g
        "((flavour>-1&&flavour<4)|| flavour==4 || flavour==5 || flavour==21)", #unmatched
        ]
    flist.reverse()
    q_names =['uds', 'c','b','gluon', 'unmatched']
    q_names.reverse()
    colors = ['#236BB2', '#CC2828', '#458E2F', '#E5AD3D', 'grey']
    colors.reverse()
    
    changes2 = {'subplot': True,
                'markers': ['f'],
                'legloc': 'lower left',
                'special_binning': True,
                'y': [0, 1],
               }

    if x =="jet1abseta":
        changes2['alleta'] = True
    elif x =="zpt":
        changes2['x'] = [0, 400]
    if changes is not None:
        changes2.update(changes)
    changes = changes2
    fig, ax = plotbase.newPlot()
    for f_id, selection, c in zip(q_names, flist, colors):
            changes['labels']=[f_id]
            changes['colors']=[c]
            changes['xynames'] = [x, "%sfrac" % flavourdef]
            q = selection.replace("flavour", 
                                flavourdef)
            plot1d.datamcplot("_".join([q, x]), files, opt,fig_axes=(fig, ax), changes=changes, settings=settings)
    settings['filename'] = plotbase.getDefaultFilename(quantity, opt, settings)
    plotbase.Save(fig, settings)


def pfcomposition_flavour(files, opt, changes=None):
    """Plot the PF composition as a function of the MC truth flavour."""

    flavourdef = "physflavour"
    quantity = "components_%s" % flavourdef
    settings = plotbase.getSettings(opt, changes, settings=None,
                                            quantity=quantity)
    nbr = 5
    labels = ["N. hadron", "C. hadron", r"$\gamma$       ",  r"$e$       ",
                                        r"$\mu$       ", "HFem", "HFhad"][:nbr]
    labels.reverse()
    colours = ['YellowGreen', 'LightSkyBlue', 'Orange', 'MediumBlue',
                  'Darkred', 'yellow', 'grey'][:nbr]
    colours.reverse()
    components = ["neutralhad", "chargedhad", "photon",  "chargedem", "muon",
                                                         "HFem", "HFhad"][:nbr]
    components.reverse()
    names = ["jet1" + component + "fraction" for component in components]

    stacked = []
    for i in range(len(names)):
        stacked += ["(%s)" % "+".join(names[i:])]
    fig, ax = plotbase.newPlot()

    if len(files) > 1:
        files = [files[1]]
    changes = {
        'subplot': True,
        'nbins': 25,
        'xynames': [flavourdef, 'components'],
        'markers': ['f'],
        'legbox': (0.04, 0.2),
        'legendframe': True,
        'legendcolumns': 1,
        'nolumilabel': True,
        'cutlabel': 'eta',
        'cutlabeloffset': 0.07,
        'mconly'  : True,
        'y': [0, 1],
        'legloc': '0.05,0.5',
        'legreverse': False,
    }

    for n, l, c in zip(stacked, labels, colours):
        changes['labels'] = [l, l]
        changes['colors'] = [c]

        plot1d.datamcplot("_".join([n,flavourdef]), files, opt,
                    fig_axes=(fig, ax), changes=changes, settings=settings)

    settings['filename'] = plotbase.getDefaultFilename(quantity, opt, settings)
    plotbase.Save(fig, settings)

