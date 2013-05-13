# -*- coding: utf-8 -*-
"""Plot Jet component fractions

   This module plots the component fractions of the leading jet and the
   differences between data and MC.
"""
import math
import copy
from ROOT import TF1, TGraphErrors, TEfficiency

import ROOT
import getroot
import plotbase


def flavours(files, opt, quantity='zpt', subplot=() , subtext="", normalized=False, stacked=True, rebin=2, correctly=True):
    """Plot the flavour mixture of the leading jet"""
    # Name everything you want and take only the first <nbr> entries of them
    nbr = 5
    labels =  ["gluon", "uds", "c", "b", "all", "other"][:nbr]
    colours = ['YellowGreen', 'Plum', 'Crimson', 'DeepSkyBlue', 'grey', 'grey'][:nbr]
    lines = ['green', 'Purple', 'DarkRed', 'Navy', 'black'][:nbr]
    markers = ['o','s','v','^','d','D','>'][:nbr]
    flavours = ["g", "uds", "c", "b", ""][:nbr]
    if quantity == 'jet1eta' or quantity == 'jet1abseta':
        select = {'incut':'alleta'}
    else:
        select = {}
    changelist = [{'var': "var_Flavour_" + flavour} if flavour else {} for flavour in flavours]
    print changelist
    if quantity == 'npv':
        rebin = 1
    elif quantity == 'zpt':
        rebin = 5
    
    def mkdict(original, it):
        result = {} 
        result.update(select)
        result.update(it)
        print "DICT", result
        return result
    # Get list of graphs from inputfiles
#    print [getroot.getnamefromnick(quantity, files[1], ch, rebin) for ch in changelist]
    graphs = [getroot.getplotfromnick(quantity, files[1], mkdict(select,ch), rebin) for ch in changelist]

    # normalize to last: g/s
    emptybins = 0
    if normalized:
        s = graphs[-1]
        for j in range(len(graphs)):
            g = graphs[j]
            for i in range(len(g)):
                # bad error treatment for empty bins
                if s.y[i] == 0:
                    s.y[i] = 1
                    emptybins += 1
                if g.y[i] == 0:
                    g.yerr[i] = g.y[i]/s.y[i] * s.yerr[i]/s.y[i]
                    emptybins += 1
                else:
                    g.yerr[i] = g.y[i]/s.y[i] * math.sqrt((g.yerr[i]/g.y[i])**2 + (s.yerr[i]/s.y[i])**2)
                g.y[i] /= s.y[i]

    
    # stack the graphs for both MC and data
    if stacked:
        graphs.reverse()
        for i in range(2, len(graphs)):
            graphs[i].y = map(lambda a,b: a+b, graphs[i-1].y, graphs[i].y)
        graphs.reverse()

    print "\nSum :",
    for y in graphs[-1].y: print "%1.4f" % (y),
    print

    if opt.verbose:
        print graphs[0].x
        print graphs[1].y
        print len(graphs), len(graphs[0].y)
        print emptybins

    if subplot:
        fig, ax = fa
    else:
        fig, ax = plotbase.newplot()


    if subtext is not 'None':
        ax.text(-0.04, 1.01, subtext, va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')

#        if change.has_key('algorithm') and 'GenJets' in change['algorithm']:
#            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=opt.colors[1], fmt='-', capsize=0 ,label=opt.labels[1])
#            ax.bar(f.x, f.y, (f.x[2] - f.x[1]), bottom=numpy.ones(len(f.x)) * 1e-6, fill=True, facecolor=opt.colors[1], edgecolor=opt.colors[1])
#        elif s=='f':
#            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c, fmt='-', capsize=0 ,label=l)
#            ax.bar(f.x, f.y, (f.x[2] - f.x[1]), bottom=numpy.ones(len(f.x)) * 1e-6, fill=True, facecolor=c, edgecolor=c)
#        else:
#            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c, fmt=s, capsize=0 ,label=l)

    w = graphs[0].x[2] - graphs[0].x[1]
    low = 1e-6 * (not normalized)

    for i in range(len(graphs)):
        graphs[i].x.insert(0, graphs[i].x[0]-w)
        graphs[i].xc.insert(0, graphs[i].xc[0]-w)
        graphs[i].y.insert(0, 0)
        graphs[i].yerr.insert(0, 0)
        print i, labels[i], colours[i], graphs[i].y[20]
        if i != len(graphs) -1 and stacked:
            ax.bar(graphs[i].x, graphs[i].y, width=w, bottom=low, color=colours[i], edgecolor = None, linewidth=0, label=labels[i])
            ax.errorbar(graphs[i].xc, graphs[i].y, graphs[i].yerr, drawstyle='steps-mid', color=colours[i], fmt='', capsize=0 )
        ax.errorbar(graphs[i].xc, graphs[i].y, #graphs[i].yerr,
            drawstyle='steps-mid', color=lines[i], fmt='-', capsize=0 )

#        else:
#            ax.bar(x, graphs[i].y, width=barWidth, color=colours[i], edgecolor = None, linewidth=0,label=labels[i])
#        ax.plot(bins, graphs[i].y+[graphs[i].y[-1]], drawstyle='steps-post', color='black',linewidth=1)
#        ax.errorbar(graphs[i].x, graphs[i].y, graphs[i].yerr, elinewidth=1, lw=0, color='grey')
    #ax.text(0.64,0.36, r"Data $\quad$MC", va='top', ha='left', transform=ax.transAxes)
#    fig.subplots_adjust(left=0.15)
#    ax.tick_params(which='both')
#    ax.grid(True,which="both", ls='-',color='0.75', zorder=0)
#    ax.axhline(0.0, color='black', lw=1, zorder=10)
    if normalized:
        ax = plotbase.axislabels(ax, quantity, 'flavourfrc')
    else:
        ax = plotbase.axislabels(ax, quantity)
    plotbase.labels(ax, opt, legloc='center right', frame=True, jet=bool(subplot), sub_plot=(subplot))
    if subplot is not True:
        name = ""
        if normalized:
            name += "norm_"
        if stacked:
            name += "stack_"
        name = "flavours_" + name + quantity + "_" + opt.algorithm + opt.correction
        plotbase.Save(fig, name, opt, not normalized)


# classic flavour plots
def flavour(files, opt):
    flavours(files, opt)

def flavour_etapm(files, opt):
    flavours(files, opt, quantity='jet1eta')

def flavour_eta(files, opt):
    flavours(files, opt, quantity='jet1abseta')

def flavour_npv(files, opt):
    flavours(files, opt, quantity='npv')

def flavourfrac(files, opt):
    flavours(files, opt, quantity='zpt', normalized=True)

def flavourfrac_eta(files, opt):
    flavours(files, opt, quantity='jet1abseta', normalized=True)

def flavourfrac_npv(files, opt):
    flavours(files, opt, quantity='npv', normalized=True)


plots = ['flavours', 'flavours_eta', 'flavours_npv'
        ]


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    import sys
    if len(sys.argv) < 2:
        print "Usage: python macros/plotfractions.py data_file.root mc_file.root"
        exit(0)
    files = [None, getroot.openfile(sys.argv[2])]
#    bins = getroot.getbins(files[1], [])
    flavours_zpt(files, opt=plotbase.options(bins=[]))


