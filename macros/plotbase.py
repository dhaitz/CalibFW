# -*- coding: utf-8 -*-
"""Basic plot formatting and helper module.

This module contains all the often used plotting tools

"""

import socket
import getpass
import os
import os.path
import sys
import copy
import numpy
import matplotlib
import matplotlib.pyplot as plt
from time import localtime, strftime, clock
import argparse
import math


from ROOT import gROOT

import getroot
import plotrc
import plotdatamc
import plot2d


def plot(modules, plots, datamc, op):
    """Search for plots in the module and run them."""
    # dont display any graphics
    gROOT.SetBatch(True)
    startop = copy.deepcopy(op)
    whichfunctions = []
    remaining_plots = copy.deepcopy(plots)
    for module in modules:
        print "Doing plots in", module.__name__, "..."
        if op.verbose:
            print "%1.2f | Start plotting" % clock()
        if not plots:
            print "Nothing to do. Please list the plots you want!"
            plots = []
        for p in plots:
            if hasattr(module, p):                                                        #plot directly as a function
                getattr(module, p)(datamc, op)
                remaining_plots.remove(p)
            elif hasattr(module, "plotdictionary") and p in module.plotdictionary:        #if no function available, try dictionary
                print "New plot: (from dictionary)", p, 
                module.plotfromdict(datamc, op, p)
                remaining_plots.remove(p)
        if op != startop:
            whichfunctions += [p+" in "+module.__name__]
        if op.verbose:
            print "%1.2f | End" % clock()
    # remaining plots are given to the function_selector
    if len(remaining_plots) > 0:
        print "Doing remaining plots ..."
        function_selector(remaining_plots, datamc, op)

    # check whether the options have changed and warn
    if op != startop:
        print "WARNING: The following options have been modified by a plot:"
        for key in dir(op):
            if "_" not in key and getattr(op, key) != getattr(startop, key):
                print " -", repr(key), "was:", getattr(startop, key), "is now:", getattr(op, key)
        print "These plots modify options:"
        for f in whichfunctions:
            print "  ", f
        print "This should not be the case! Options should not be changed within a plotting function."
        print "A solution could invoke copy.deepcopy() in such a case."


# function_selector: takes a list of plots and assigns them to the according funtion
def function_selector(plots, datamc, opt):

    plotlist = getroot.getplotlist(datamc, algorithm=opt.algorithm, filenames=opt.files)
    plotlist_nocuts = [i+"_nocuts" for i in getroot.getplotlist(datamc, folder="NoBinning_allevents", algorithm=opt.algorithm, filenames=opt.files)]
    plotlist_zcutsonly = [i+"_zcutsonly" for i in getroot.getplotlist(datamc,
                                        folder="NoBinning_zcutsonly", algorithm=opt.algorithm, filenames=opt.files)]

    plotlist_alleta = [i+"_alleta" for i in getroot.getplotlist(datamc,
                                        folder="NoBinning_alleta", algorithm=opt.algorithm, filenames=opt.files)]
    plotlist_all = [i+"_all" for i in getroot.getplotlist(datamc, 'all', algorithm=opt.algorithm, filenames=opt.files)]
    for plot in plots:
        if ('L1' in plot or 'L2' in plot) and plot in plotlist:
            plotdatamc.L1(plot, datamc, opt)
        elif '2D' in plot:
            if plot in plotlist:
                plot2d.twoD(plot, datamc, opt)
            elif plot in plotlist_nocuts:
                plot2d.twoD(plot[:-7], datamc, opt, changes={'incut':'allevents'})
            elif "_all" in plot and plot in plotlist_all:
                plot2d.twoD_all(plot[:-4], datamc, opt)
        elif plot in plotlist:
            plotdatamc.datamcplot(plot, datamc, opt)
        elif plot in plotlist_nocuts:
            plotdatamc.datamcplot(plot[:-7], datamc, opt, changes={'incut':'allevents'})
        elif plot in plotlist_zcutsonly:
            plotdatamc.datamcplot(plot[:-10], datamc, opt, changes={'incut':'zcutsonly'})
        elif plot in plotlist_alleta:
            plotdatamc.datamcplot(plot[:-7], datamc, opt, changes={'incut':'alleta'})
        elif "_all" in plot and plot in plotlist_all:
            plotdatamc.datamc_all(plot[:-4], datamc, opt)
        elif '_run' in plot:
            if '_nocuts' in plot:
                plotdatamc.runplot(plot[:-7], datamc, opt, changes={'incut':'allevents'})
            else:
                plotdatamc.runplot(plot, datamc, opt)


def options(
            # standard values go here:
            algorithm="AK5PFJetsCHS",
            correction="L1L2L3Res",
            lumi=None,
            energy=8,
            status=None,
            author=None,
            date=None,
            out="out",
            labels=["data", "MC"],
            colors=['black', '#CBDBF9'],
            style=["o","f"],
            formats=['png'],
            layout='generic',
            files=None,
            title="",
            eventnumberlabel=None,
            plots=None,
            npv=[(0, 4), (5, 8), (9, 15), (16, 21), (22, 100)],
            cut=[0.2, 0.4],
            eta=[0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191],
            bins=[30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]):
    """Set standard options and read command line arguments

    To be turned into a class with str method and init
    """
    parser = argparse.ArgumentParser(
        description="%(prog)s does all the plotting.",
        epilog="Have fun.")

    # input files with colors and labels
    parser.add_argument('files', metavar='file', type=str, nargs='*',
        default=files,
        help="data and Monte Carlo input root file(s). One data file and at " +
             "least one Monte Carlo file is assumed.")
    parser.add_argument('-C', '--colors', type=str, nargs='+',
        default=colors,
        help="colors for the plots in the order of the files. Default is: "+
             ", ".join(colors))
    parser.add_argument('-k', '--labels', type=str, nargs='+',
        default=labels,
        help="labels for the plots in the order of the files. Default is: "+
             ", ".join(labels))
    parser.add_argument('-S', '--style', type=str, nargs='+',
        default=style,
        help="style for the plot in the order of the files. 'o' for points, '-' for lines, 'f' for fill. Default is: "+
             ", ".join(style))
    parser.add_argument('-p', '--pudist', type=str, nargs='+',
        help="pile-up distributions")

    # lumi and energy settings
    parser.add_argument('-a', '--algorithm', type=str,
        default=algorithm,
        help="jet algorithm")

    parser.add_argument('-c', '--correction', type=str,
        default=correction,
        help="jet energy correction")
    parser.add_argument('-l', '--lumi', type=float,
        default=lumi,
        help="luminosity for the given data in /pb")
    parser.add_argument('-e', '--energy', type=int,
        default=energy,
        help="centre-of-mass energy for the given samples in TeV")

    # plot decoration
    parser.add_argument('-s', '--status', type=str,
        default=status,
        help="status of the plot (e.g. CMS preliminary)")
    parser.add_argument('-A', '--author', type=str,
        default=author,
        help="author name of the plot")
    parser.add_argument('--date', type=str,
        default=date,
        help="show the date in the top left corner. 'iso' is YYYY-MM-DD, " +
             "'today' is DD Mon YYYY and 'now' is DD Mon YYYY HH:MM.")
    # output and selection settings
    parser.add_argument('-o', '--out', type=str,
        default=out,
        help="output directory for plots")
    parser.add_argument('-f', '--formats', type=str, nargs='+',
        default=formats,
        help="output format for the plots")
    parser.add_argument('-L', '--layout', type=str,
        default='generic',
        help="layout for the plots. E.g. 'document': serif, LaTeX, pdf; " +
             "'slides': sans serif, big, png; 'generic': slides + pdf. " +
             "This is not implemented yet.")
    parser.add_argument('-P', '--plots', type=str, nargs='+',
        default=plots,
        help="do only this plot/these plots. " +
             "The function names are required here.")
    parser.add_argument('-n', '--normalize', action='store_false',
        help="don't normalize Monte Carlo samples to the event count in data " +
             "and regardless of the given luminosity. This is only applied " +
             "to data/MC comparisons")
    parser.add_argument('-v', '--verbose', action='store_true',
        help="verbosity")
    parser.add_argument('-E', '--eventnumberlabel', action='store_true',
        help="add event number label")
    parser.add_argument('-t', '--title', type=str,
        default=title,
        help="plot title")


    opt = parser.parse_args()
    # to be substituted by commandline arguments (perhaps changed,
    # no formatting options here? but for multiple MC,
    # colors (predefined sequence?) and labels are needed)
    opt.factor = 1.0
    opt.bins = bins
    opt.brackets = False
    opt.npv = npv
    opt.cut = cut
    opt.eta = eta
    if opt.verbose:
        showoptions(opt)
    matplotlib.rcParams.update(plotrc.getstyle(opt.layout))
    return opt

def fail(fail_message):
    print fail_message
    exit(0)

def printfiles(filelist):
    if len(filelist) > 0:
        print "Data file:", filelist[0]
    else:
        print "No input files given!"
        exit(0)
    if len(filelist) == 2:
        print "MC file:", filelist[1]
    elif len(filelist) > 2:
        print "MC files:", ", ".join(filelist[1:])

def createchanges(opt, change={}):
    change["correction"] = opt.correction
    change["algorithm"] = opt.algorithm
    return change

def getchanges(opt, change={}):
    if 'algorithm' not in change: change['algorithm'] = opt.algorithm
    if 'correction' not in change: change['correction'] = opt.correction
    return change

def getvariationlist (quantity, opt):
    # returns a list with 'changes'-dictionaries, to be used
    # to vary over a certain quantity in the closure root file
    if quantity == 'zpt':
        key = 'bin'
        var=getroot.binstrings(opt.bins)
    elif quantity == 'npv':
        key = 'var'
        var=getroot.npvstrings(opt.npv)
    elif quantity == 'alpha':
        key = 'var'
        var=getroot.cutstrings(opt.cut)
    else:
        key = 'var'
        var=getroot.etastrings(opt.eta)
    ch_list = [{key:v} for v in var]
    return ch_list

def getcorralgovariations():
    # returns a list with 'changes'-dictionaries, to be used
    # to vary over a all agorithms / correction levels in the closure root file
    list_ac = []
    for a in ['AK5PFJets','AK5PFJetsCHS']:
        for c in ["", "L1", "L1L2L3", "L1L2L3Res"]:
            ch={}
            ch['algorithm'] = a
            ch['correction'] = c
            list_ac.append(ch)
    return list_ac

def getdefaultsubtexts():
    subtexts = ["a)", "b)", "c)", "d)", "e)", "f)", "g)", "h)", "i)", "j)"]
    return subtexts

def showoptions(opt):
    print "Options:"
    for o, v in opt.__dict__.items():
        print "   {0:11}: {1}".format(o, v)
    print "matplotlib settings:"
    for k, v in plotrc.getstyle(opt.layout).items():
        print "   {0:24}: {1}".format(k, v)


def getfactor(lumi, fdata, fmc, quantity='z_phi', change={}):
    """Get the normalization factor for the f_data file w.r.t. f_mc."""
    histo_data = getroot.getplot(quantity, fdata, change)
    histo_mc = getroot.getplot(quantity, fmc, change)
    histo_mc.scale(lumi)
    print "    >>> The additional scaling factor is:", (
        histo_data.ysum() / histo_mc.ysum())
    return histo_data.ysum() / histo_mc.ysum()

def getalgorithms(algorithm):
    if "AK7" in algorithm: algorithms = ['AK7PFJets', 'AK7PFJetsCHS']
    else: algorithms = ['AK5PFJets', 'AK5PFJetsCHS']
    return algorithms

def getgenname(opt):
    if "AK7" in opt.algorithm: gen = 'AK7GenJets'
    else: gen = 'AK5GenJets'
    return gen

def nicetext(s):
    if "run" in s:
        return r"Time dependence for "+nicetext(s[:-4])
    elif s in ['z_pt', 'zpt']: return r"$p_\mathrm{T}^\mathrm{Z}$"
    elif s in ['jet1_pt', 'jet1pt']: return r"$p_\mathrm{T}^\mathrm{Jet 1}$"
    elif s in ['jet2_pt', 'jet2pt']: return r"$p_\mathrm{T}^\mathrm{Jet 2}$"
    elif s in ['jet1eta', 'eta']: return r"$\eta^\mathrm{Jet1}$"
    elif s in ['jets_valid', 'jetsvalid']: return r"Number of valid jets"
    elif s == 'npv': return 'NPV'
    elif s == 'alpha': return r"$\alpha$"
    elif s == 'balresp': return r"$p_\mathrm{T}$ balance"
    elif s == 'baltwojet': return r"two-jet balance"
    elif s == 'mpfresp': return "MPF response"
    elif s == 'sumEt': return r"$\sum E^\mathrm{T}$"
    elif s == 'METsumEt': return r"Total transverse energy $\sum E^\mathrm{T}$"
    elif s == 'METpt': return r"$p_\mathrm{T}^\mathrm{MET}$"
    elif s == 'METphi': return r"$\phi^\mathrm{MET}$"
    elif s == 'met': return r"E_\mathrm{T}^\mathrm{miss}"
    elif s == 'MET': return r"E_\mathrm{T}^\mathrm{miss}"
    elif s == 'muonsvalid': return "Number of valid muons"
    elif s == 'muonsinvalid': return "Number of invalid muons"
    elif s == 'muplus': return "mu plus"
    elif s == 'muminus': return "mu minus"
    elif s == 'leadingjet': return r"Leading \/Jet"
    elif s == 'secondjet': return r"Second \/Jet"
    elif s == 'leadingjetsecondjet': return r"Leading\/ Jet,\/ Second\/ Jet"
    elif s == 'jet1': return r"Leading Jet"
    elif s == 'jet2': return r"Second \/Jet"
    elif s == 'z': return r"Z"
    elif s == 'leadingjetMET': return r"Leading\/ Jet,\/ MET"
    elif s == 'jet1MET': return r"Leading\/ Jet,\/ MET"
    elif s == 'zMET': return r"Z, MET"
    elif s == 'jet2toZpt': return r"2nd Jet Cut"
    return s

def getreweighting(datahisto, mchisto, drop=True):
    if drop:
        datahisto.dropbin(0)
        datahisto.dropbin(-1)
        mchisto.dropbin(0)
        mchisto.dropbin(-1)
    reweighting = []
    for i in range(len(mchisto)):
        if i > 13:
            break
        reweighting.append(datahisto.y[i] / mchisto.y[i])
    return reweighting


def getpath():
    """Return datapath depending on machine and user name."""
    host = socket.gethostname()
    username = getpass.getuser()
    datapath = ""
    if username == 'berger':
        if 'naf'in host:
            datapath = "/scratch/hh/lustre/cms/user/berger/analysis/"
        elif 'ekplx46'in host:
            datapath = "/local/scratch/berger/data/"
        elif 'pccms' in host:
            datapath = "/data/berger/data/"
    elif username == 'poseidon':
        # thomas local machines
        datapath = "/home/poseidon/uni/code/CalibFW/"
    elif username == 'hauth':
        if 'ekplx'in host:
            datapath = "/local/scratch/hauth/data/ZPJ2010/"
    elif username == 'piparo':
        if 'ekplx'in host:
            datapath = ""
    else:
        datapath = ""

    try:
        os.listdir(datapath)
    except:
        print "Input path", datapath, "does not exist."
        sys.exit(1)
    return datapath


def newplot(ratio=False, run=False, subplots=1, opt=options(), subplots_X=None, subplots_Y=None):
    fig = plt.figure(figsize=[7, 7])
    fig.suptitle(opt.title, size='xx-large')
    if subplots is not 1: #Get 4 config numbers: FigXsize, FigYsize, NaxesY, NaxesX
        d = {3:3, 2:2}
        if subplots_Y is not None:
            y = subplots_Y
            x = int(round(subplots/float(y)))
            if x * y < subplots:
                y = y+1
        else:
            if subplots_X is not None:
                x = subplots_X
            elif subplots in d:
                x = d[subplots]
            else:        
                x = int(math.sqrt(subplots))
            y = int(round(subplots/float(x)))
            if x * y < subplots:
                x = x+1
        print subplots, x, y
        if run:
            a = [14*x, 7*y, y, x]
        else:
            a = [7*x, 7*y, y, x]
        fig = plt.figure(figsize=[a[0], a[1]]) #apply config numbers
        ax = [fig.add_subplot(a[2],a[3],n+1) for n in range(subplots)]
        return fig, ax
    elif ratio: 
        ax = fig.add_subplot(111, position=[0.13, 0.35, 0.83, 0.58])
        ratio = fig.add_subplot(111, position=[0.13, 0.12, 0.83, 0.22], sharex=ax)
        ratio.axhline(1.0, color='black', lw=1)
        return fig, ax, ratio
    elif run:
        fig = plt.figure(figsize=[14, 7])
        ax = fig.add_subplot(111)
        return fig, ax
    else:
        ax = fig.add_subplot(111)
        return fig, ax
    return fig


def labels(ax, opt=options(), jet=False, bin=None, result=None, legloc='upper right',
           frame=True, sub_plot=False, changes={}, ratiosubplot=False, mc='False', color='black', energy_label=True):
    """This function prints all labels and captions in a plot.

    Several functions are called for each type of label.
    """
    if not ratiosubplot:
        if opt.lumi is not None and mc is not True:
            lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
        statuslabel(ax, opt.status)
        if opt.energy is not None and energy_label==True:
            energylabel(ax, opt.energy)
        if jet==True:  jetlabel(ax, changes, sub_plot)    # on demand
        if changes.has_key('var') or changes.has_key('bin'): binlabel(ax, bin, changes=changes, color=color)
        if 'incut' in changes: incutlabel(ax, color, changes['incut'])
        resultlabel(ax, result)
        authorlabel(ax, opt.author)
        datelabel(ax, opt.date)
    if legloc is not False:
        legend = ax.legend(loc=legloc, numpoints=1, frameon=frame)
    return ax


def incutlabel(ax, color='black', incut=''):
    if incut == 'allevents':
        text = r"(before cuts)"
    if incut == 'zcutsonly':
        text = r"(only $\mu$ and Z cuts applied)"
    elif incut == 'alleta':
        text = r"(jet 1 all $\eta$)"
    else:
        return
    ax.text(0.97, 0.97, text, va='top', ha='right', transform=ax.transAxes, color=color)
    return ax

def eventnumberlabel(ax, opt, events):
    text=""
    for f, l in zip(events, opt.labels):
        text += "\n" + l + " Events: " + str("%1.1e"% f)
    ax.text(0.7,1.01, text, size='xx-small', va='bottom', ha='right',transform=ax.transAxes)
        

def lumilabel(ax, lumi=0.0, xpos=0.00, ypos=1.01):
    if lumi >= 1000.0:
        ax.text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{fb}^{-1}$" %
            (lumi / 1000.0), va='bottom', ha='left', transform=ax.transAxes)
    elif lumi > 0.0:
        ax.text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{pb}^{-1}$" %
            (lumi), va='bottom', ha='left', transform=ax.transAxes)
    return ax


def energylabel(ax, energy, xpos=1.00, ypos=1.01):
    if energy is not None:
        ax.text(xpos, ypos, r"$\sqrt{s} = %u\,\mathrm{TeV}$" % (energy),
            va='bottom', ha='right', transform=ax.transAxes)

def jetlabel_string( changes, opt):
    if changes.has_key('algorithm'): algorithm = changes['algorithm']
    else: algorithm = opt.algorithm
    if changes.has_key('correction'): correction = changes['correction']
    else: correction = opt.correction
    if "L1L2L3Res" in correction:
        corr = r"L1L2L3 Residual corrected"
    elif "L1L2L3" in correction:
        corr = r"L1L2L3 corrected"
    elif "L1L2" in correction:
        corr = r"L1L2 corrected"
    elif "L1" in correction:
        corr = r"L1 corrected"
    else:
        corr = r"uncorrected"
    if "ak5pfjets" in algorithm.lower():
        jet = r"Anti-$k_{T}$ 0.5 PF jets"
    elif "ak7pfjets" in algorithm.lower():
        jet = r"Anti-$k_{T}$ 0.7 PF jets"
    else:
        jet = ""
        corr = ""
    return ( jet , corr )

def jetlabel(ax, changes={}, sub_plot=False, posx=0.05, posy=0.95, opt=options()):
    res = jetlabel_string(changes,opt)
    
    if sub_plot: col = 'red'
    else: col='black'
    
    #if "AK5" not in opt.algorithm: ax.text(posx, posy, res[0], va='top', ha='left', transform=ax.transAxes)
    #else: 
    posy=posy+0.07
    if changes.has_key('correction'):
        ax.text(posx, posy-0.07, res[1], va='top', ha='left', transform=ax.transAxes, color=col)
            
    if changes.has_key('algorithm'):
        if "CHS" in changes['algorithm']:
            ax.text(posx, posy - 0.14, r"CHS applied", va='top', ha='left', transform=ax.transAxes, color=col)
        #if "CHS" not in changes['algorithm']:
        #    ax.text(posx, posy - 0.07, r"CHS not applied", va='top', ha='left', transform=ax.transAxes, color=col)

    return ax


def authorlabel(ax, author=None, xpos=0.01, ypos=1.10):
    if author is not None:
        ax.text(xpos, ypos, author, va='top', ha='left',
                transform=ax.transAxes)
    return ax


def datelabel(ax, date='iso', xpos=0.99, ypos=1.10):
    if date is None:
        return ax
    if date == 'now':
        ax.text(xpos, ypos, strftime("%d %b %Y %H:%M", localtime()),
        va='top', ha='right', transform=ax.transAxes)
    elif date == 'today':
        ax.text(xpos, ypos, strftime("%d %b %Y", localtime()),
        va='top', ha='right', transform=ax.transAxes)
    elif date == 'iso':
        ax.text(xpos, ypos, strftime("%Y-%m-%d", localtime()),
        va='top', ha='right', transform=ax.transAxes)
    else:
        ax.text(xpos, ypos, date, va='top', ha='right', transform=ax.transAxes)
    return ax


def binlabel(ax, bin=None, low=0, high=0, xpos=0.03, ypos=0.97, changes={}, color='black'):
    if bin is None:
        if 'var' in changes and 'Cut' in changes['var'] and len(changes['var']) > 35:
            changes['var'] = 'var'+changes['var'].split('var')[2]
        if 'bin' in changes:
            ranges = changes['bin'][2:].split('to')
            bin= 'ptz'
            low = int(ranges[0])
            high = int(ranges[1])
        elif 'var' in changes and 'Eta' in changes['var']:
            ranges = changes['var'][11:].replace('_','.').split('to')
            bin = 'eta'
            low = float(ranges[0])
            high = float(ranges[1])
        elif 'var' in changes and 'PtBin' in changes['var']:
            ranges = changes['var'][10:].replace('_','.').split('to')
            bin = 'ptbin'
            low = float(ranges[0])
            high = float(ranges[1])   
        elif 'var' in changes and 'Npv' in changes['var']:
            ranges = changes['var'][8:].split('to')
            bin = 'Npv'
            low = int(ranges[0])
            high = int(ranges[1])
        elif 'var' in changes and 'Cut' in changes['var']:
            ranges = changes['var'][27:].replace('_','.')
            bin ='alpha'
            low = float(ranges)
        else:
            return ax
    if bin == 'ptz':
        text = r"$%u < p_\mathrm{T}^\mathrm{Z} / \mathrm{GeV} < %u$" % (low, high)
    elif bin == 'pthat':
        text = r"$%u < \hat{p}_\mathrm{T} / \mathrm{GeV} < %u$" % (low, high)
    elif bin == 'eta':
        if low == 0:
            text = r"$|\eta_\mathrm{jet}| < %1.3f$" % (high)
        else:
            text = r"$%1.3f < |\eta_\mathrm{jet}| < %1.3f$" % (low, high)
    elif bin == 'alpha':
        text = r"$ \alpha < %1.2f$" % (low)
    elif bin == 'ptbin':
        #text = r"%1.0f < $|\pt_\mathrm{jet}|$ < %1.0f" % (low, high)
        text = r"%1.0f GeV < $p_T^\mathrm{Jet}$ < %1.0f GeV" % (low, high)
    elif bin == 'Npv':
        if low == 0:
            text = r"$ NPV \leq %u$" % (high)
        else:
            text = r"$%u \leq NPV \leq %u$" % (low, high)
    else:
        text = bin
    ax.text(xpos, ypos, text, va='top', ha='left', size='x-large', transform=ax.transAxes, color=color )


def statuslabel(ax, status=None, xpos=0.25, ypos=1.018):
    if status is not None:
        ax.text(xpos, ypos, r"%s" % status, va='bottom', ha='left',
                transform=ax.transAxes)


def resultlabel(ax, text=None, xpos=0.05, ypos=0.05):
    if text is not None:
        if "\n" in text:
            # two lined thingy
            ypos += 0.07
        ax.text(xpos, ypos, text, va='top', ha='left', transform=ax.transAxes)


def axislabel_2d(ax, y_q, y_obj, x_q='pt', x_obj='Z', brackets=False):
    print "Please use axislabels instead of axislabel_2d."
    return axislabels(ax, x_q, y_q, brackets)

def fit(fit, ax, quantity, rootfile, change, rebin, color, index, runplot_diff=False, mc_mean = None, run_min=0, run_max=1):
    """One of several fits is added to an axis element, fit parameters are added as text element"""
    if color == '#CBDBF9': color = 'blue'
    if fit == 'chi2':
        # fit a horizontal line
        intercept, ierr, chi2, ndf = getroot.fitline(getroot.getobjectfromnick(quantity, rootfile, change, rebin))
        ax.axhline(intercept, color=color, linestyle='--')
        ax.axhspan(intercept+ierr, intercept-ierr, color=color, alpha=0.2)

        # and display chi^2
        ax.text(0.97, 0.20+(index/20.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
        va='top', ha='right', transform=ax.transAxes, color=color)
              
    else:
        intercept, ierr, slope, serr,  chi2, ndf = getroot.fitline2(getroot.getobjectfromnick(quantity, rootfile, change, rebin))
        if runplot_diff:
            intercept = intercept - mc_mean

        # fit line:
        line_fit = ax.plot([run_min, run_max],[intercept+run_min*slope, intercept+run_max*slope], color = color, linestyle='--')
        ax.plot([run_min, run_max],[intercept+ierr+run_min*(slope-serr), intercept+ierr+run_max*(slope-serr)], alpha=0.2, color = color, linestyle='--')
        ax.plot([run_min, run_max],[intercept-ierr+run_min*(slope+serr), intercept-ierr+run_max*(slope+serr)], alpha=0.2, color = color, linestyle='--')
        
        if fit == 'slope':
            #display slope
            ax.text(0.97, 0.95-(index/10.), r"$\mathrm{Fit\/slope} = (%1.2f\pm%1.2f) \times 10^{-6}$" % (slope*1000000, serr*1000000),
               va='top', ha='right', transform=ax.transAxes, color=color,
               size='x-large')
        elif fit == 'intercept':
            #display intercept
            ax.text(0.97, 0.35-(index/10.), r"$\mathrm{Fit\/intercept} = (%1.3f\pm%1.3f)$" % (intercept, ierr),
               va='top', ha='right', transform=ax.transAxes, color=color,
               size='x-large')


def unitformat(quantity="", unit="", brackets=False):
    """Returns a string according to SI standards

       (r"$p_\mathrm{T}$", "GeV") yields "$p_\mathrm{T}$ / GeV"
       brackets are not SI!
       "quantity|unit"
    """

    if unit != "":
        if "/" in quantity:
            quantity = "(%s)" % quantity
        if "/" in unit:
            unit = "(%s)" % unit
        if brackets:        # units with [] (not allowed by SI system!)
            quantity = r"%s [%s]" % (quantity, unit)
        else:                # units with /
           quantity = r"%s / %s" % (quantity, unit)
    #print "The axis legend string is:", repr(quantity)
    return quantity



# put everything into one dictionary  key:[min, max, Name, unit, z_min, z_max]
d={         
        'abseta':[0.0, 5.5, r"$|\eta^\mathrm{%s}|$", ""],
        'absphi':[0, 3.141593, r"$|\phi^\mathrm{%s}|$", ""],
        'alpha':[0, 0.2, r"$p_\mathrm{T}^\mathrm{Jet 2}/p_\mathrm{T}^{Z}$", ""],
        'bal':[0.0, 1.8, r"$p_\mathrm{T}$ balance", ""],
        'baljet2z':[0, 1, r"$p_\mathrm{T}^\mathrm{Jet 2}/p_\mathrm{T}^{Z}$", ""],
        'balresp':[0.0, 1.8, r"$p_\mathrm{T}$ balance", ""],
        'baltwojet':[0.0, 1.8, r"$p_\mathrm{T}$ balance for 2 jets", ""],
        'chargedem':[0,1, r"%s charged em fraction", ""],
        'chargedhad':[0,1, r"%s charged hadron fraction", ""],
        'components_diff':[-0.05, 0.05, r"Data-MC of Leading Jet Components", ""],
        'components':[0, 1, r"Leading Jet Component Fraction", ""],
        'constituents':[0, 60, r"Number of Jet Constituents", ""],
        'correction':[0.85, 1.02, "Correction factor", ""],
        'cut':[0, 1.1, r"Cut Inefficiency (%s)", ""],
        'datamcratio':[0.88, 1.03, r"data/MC ratio", ""],
        'deltaeta':[0, 15, r"$\Delta \eta(\mathrm{%s})$", ""],
        'deltaeta':[0, 5, r"$\Delta \eta(\mathrm{%s,\/ %s})$", ""],
        'deltaphi':[0, 3.141593, r"$\Delta \phi(\mathrm{%s,\/%s})$", ""],
        'deltar':[0, 20, r"$\Delta \/R(\mathrm{%s,\/ %s})$", ""],
        'electron':[0,1, r"%s electron fraction", ""],
        'eta':[-5, 5, r"$\eta^\mathrm{%s}$", ""],
        'eventcount':[0, 1.1, r"Eventcount", ""],
        'extrapol':[0.86, 1.04, r"Response", ""],
        'HFem':[0,1, r"%s HF em fraction", ""],
        'HFhad':[0,1, r"%s HF hadron fraction", ""],
        'jet1area':[0.6, 1, r"Leading Jet area", ""],
        'jet1charged':[0,30, r"%s charged", ""],
        'jet1const':[0,30, r"%s const", ""],
        'jet1pt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{Leading Jet}$", 'GeV'],        
        'jet2pt':[ 0, 100, r"$p_\mathrm{T}^\mathrm{Jet2}$", 'GeV'],
        'jet3pt':[ 0, 100, r"$p_\mathrm{T}^\mathrm{Jet3}$", 'GeV'],
        'METpt':[ 0, 80, r"$E_\mathrm{T}^\mathrm{miss}$", 'GeV'],
        'muminuspt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{\mu-}$", 'GeV'],
        'mupluspt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{\mu+}$", 'GeV'],

        'eta':[-5, 5, r"$\eta^\mathrm{%s}$", ""],
        'deltaeta':[0, 15, r"$\Delta \eta(\mathrm{%s})$", ""],
        'METeta':[-0.1, 0.1, r"$\eta^\mathrm{MET}$", ""],
        'METsumEt':[0, 2500, r"$\sum E^\mathrm{T}$", "GeV"],
        'sumEt':[0, 2500, r"$\sum E^\mathrm{T}$", "GeV"],
        'METfraction':[0, 0.2, r"MET / $E^T_Total$", ""],

        'cut':[0, 1.1, r"Cut Inefficiency (%s)", ""],

        'phi':[-3.2, 3.2, r"$\phi^\mathrm{%s}$", ""],
        'absphi':[0, 3.141593, r"$|\phi^\mathrm{%s}|$", ""],
        'deltaphi':[0, 3.141593, r"$\Delta \phi(\mathrm{%s,\/%s})$", ""],
        'deltaeta':[0, 5, r"$\Delta \eta(\mathrm{%s,\/ %s})$", ""],
        'deltar':[0, 20, r"$\Delta \/R(\mathrm{%s,\/ %s})$", ""],

        'zmass':[70, 110, r"$m^\mathrm{Z}$", "GeV"],

        'npv':[0, 35, r"Number of Reconstructed Vertices $n$",""],
        'reco':[0, 35, r"Number of Reconstructed Vertices $n$",""],

        'L1':[0, 1.2, r"L1 correction factor",""],
        'L1abs':[-25, 25, r"L1 absolute correction","GeV"],
        'L2':[0, 1.2, r"L2 correction factor",""],
        'L2abs':[-25, 25, r"L2 absolute correction","GeV"],
        'L1L2L3':[0, 1.2, r"L1L2L3 correction factor",""],
        'L1L2L3abs':[-25, 25, r"L1L2L3 absolute correction","GeV"],
        'L3':[0, 1.2, r"L3 correction factor",""],
        'L3abs':[-25, 25, r"L3 absolute correction","GeV"],
        'METeta':[-0.1, 0.1, r"$\eta^\mathrm{MET}$", ""],
        'METfraction':[0, 0.2, r"MET / $E^T_Total$", ""],
        'METpt':[ 0, 80, r"$E_\mathrm{T}^\mathrm{miss}$", 'GeV'],
        'METsumEt':[0, 2500, r"$\sum E^\mathrm{T}$", "GeV"],
        'mpf':[0.9, 1.1, r"$MPF$ Response", ""],
        'mpfresp-notypeI':[0.0, 1.8, r"$MPF$ Response (raw MET)", ""],
        'mpfresp':[0.8, 1.1, r"$MPF$ Response", ""],
        'muminuspt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{\mu-}$", 'GeV'],
        'muon':[0,1, r"%s muon fraction", ""],
        'muonsinvalid':[0, 5, "Number of invalid muons", ""],
        'muonsvalid':[0, 5, "Number of valid muons", ""],
        'mupluspt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{\mu+}$", 'GeV'],
        'neutralem':[0,1, r"%s neutral em fraction", ""],
        'neutralhad':[0,1., r"%s neutral hadron fraction", ""],
        'npv':[0, 35, r"Number of Reconstructed Vertices $n$",""],
        'numpu':[0, 35, r"Number of Primary Vertices", ""],
        'numputruth':[0, 35, r"Pile-up Truth (Poisson mean)", ""],
        'phi':[-3.2, 3.2, r"$\phi^\mathrm{%s}$", ""],
        'photon':[0,1., r"%s photon fraction", ""],
        'pt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{%s}$", 'GeV'],        
        'ptbalance':[0.88, 1.04, r"$p_\mathrm{T}$ balance", ""],
        'ratio':[0.89, 1.08, r"%s / %s ratio", ""],
        'reco':[0, 35, r"Number of Reconstructed Vertices $n$",""],
        'response':[0.81, 1.05, r"Jet Response", ""],
        'ratio':[0.89, 1.08, r"%s / %s ratio", ""],
        'responseratio':[0.88, 1.03, r"data/MC ratio", ""],
        'datamcratio':[0.88, 1.03, r"data/MC ratio", ""],

        'numputruth':[0, 35, r"Pile-up Truth (Poisson mean)", ""],
        'numpu':[0, 35, r"Number of Primary Vertices", ""],
        'alpha':[0, 3, r"$p_\mathrm{T}^\mathrm{Jet 2}/p_\mathrm{T}^{Z}$", ""],
        'baljet2z':[0, 1, r"$p_\mathrm{T}^\mathrm{Jet 2}/p_\mathrm{T}^{Z}$", ""],
        'rho':[0, 50, r"$\rho$", ""],
        'constituents':[0, 60, r"Number of Jet Constituents", ""],
        'jet2ratio':[0, 0.4, r"$p_\mathrm{T}^\mathrm{Jet_2}/p_\mathrm{T}^{Z}$", ""],
        'run':[190000, 206000, r"Run", ""],
        'eventcount':[0, 1.1, r"Eventcount", ""],

        'jet1area':[0.6, 1, r"Leading Jet area", ""],
        'jet2area':[0.6, 1, r"Second Jet area", ""],

        'jetptabsdiff':[0, 100, r"$p_\mathrm{T}^\mathrm{Jet 1} - p_\mathrm{T}^{Jet 2}$", "GeV"],
        'jetptratio':[0, 10, r"$p_\mathrm{T}^\mathrm{Jet 1} / p_\mathrm{T}^{Jet 2}$", ""],

        'components_diff':[-0.05, 0.05, r"Data-MC of Leading Jet Components", ""],
        'components':[0, 1, r"Leading Jet Component Fraction", ""],

        'chargedhad':[0,1, r"%s charged hadron fraction", ""],
        'chargedem':[0,1, r"%s charged em fraction", ""],
        'neutralem':[0,1, r"%s neutral em fraction", ""],
        'neutralhad':[0,1., r"%s neutral hadron fraction", ""],
        'muon':[0,1, r"%s muon fraction", ""],
        'electron':[0,1, r"%s electron fraction", ""],
        'photon':[0,1., r"%s photon fraction", ""],
        'HFhad':[0,1, r"%s HF hadron fraction", ""],
        'HFem':[0,1, r"%s HF em fraction", ""],

        'jet1charged':[0,30, r"%s charged", ""],
        'jet1const':[0,30, r"%s const", ""],
        'summedf':[0.8,1.2, r"$%s$ fraction sum", ""],

        }

def axislabels(ax, x='z_pt', y='events', brackets=False, opt=options()):
    """same as the old version, but can handle and and y axis indpendetly

       new idea: improve automatic scaling:
       bottom=0
       top
       autoscaling
       dict = { quantity: axsetting}
    """

    def setxaxis(limits=(0, 200), quantity="x", unit=""):
        ax.set_xlabel(unitformat(quantity, unit, brackets), ha="right", x=1)
        ax.set_xlim(limits)

    def setyaxis(limits=(0, 1), quantity="y", unit="", bottom=None):
        ax.set_ylabel(unitformat(quantity, unit, brackets), va="top", y=1)
        if bottom is not None:
            ax.set_ylim(bottom=bottom)
        else:
            ax.set_ylim(limits)

    for quantity, function in zip([x,y],[[setxaxis, ax.set_xticks, ax.set_xticklabels], [setyaxis, ax.set_yticks, ax.set_yticklabels]]):
    # special formatting options for some quantities ...
        if 'phi' in quantity:
            function[1]([-3.14159265, -1.57079633, 0.0, 1.57079633, 3.14159265])
            function[2]([r"$-\pi$", r"$-\frac{\pi}{2}$", r"$0$", r"$\frac{\pi}{2}$", r"$\pi$"])
            if 'deltaphi' in quantity:
                function[0]((d['deltaphi'][0], d['deltaphi'][1]), d['deltaphi'][2] % (nicetext(quantity.replace("deltaphi-","").split("-")[0]), 
                        nicetext(quantity.replace("deltaphi-","").split("-")[1])), d['deltaphi'][3]) 
            elif 'phiabsdiff' in quantity:
                function[0]((d['phiabsdiff'][0], d['phiabsdiff'][1]), d['phiabsdiff'][2] % nicetext(quantity.replace("phiabsdiff","")),
                        d['phiabsdiff'][3]) 
            elif 'abs' in quantity:
                function[0]((d['absphi'][0], d['absphi'][1]), d['absphi'][2] % 
                        nicetext(quantity.replace("abs_","").replace("_phi","")) , d['absphi'][3]) 
            else:
                function[0]((d['phi'][0], d['phi'][1]), d['phi'][2] % nicetext(quantity.replace("phi","")) , d['phi'][3]) 
        elif 'eta' in quantity:
            if 'deltaeta' in quantity:
                function[0]((d['deltaeta'][0], d['deltaeta'][1]), d['deltaeta'][2] % (nicetext(quantity.replace("deltaeta-","").split("-")[0]),
                         nicetext(quantity.replace("deltaeta-","").split("-")[1])), d['deltaeta'][3]) 
            elif 'etaabsdiff' in quantity:
                function[0]((d['deltaeta'][0], d['deltaeta'][1]), d['deltaeta'][2] % nicetext(quantity.replace("etaabsdiff","")) , d['deltaeta'][3]) 
            elif 'abseta' in quantity:
                function[0]((d['abseta'][0], d['abseta'][1]), d['abseta'][2] % nicetext(quantity.replace("abseta","")) , d['abseta'][3]) 
            else:
                function[0]((d['eta'][0], d['eta'][1]), d['eta'][2] % nicetext(quantity.replace("eta","")) , d['eta'][3]) 
        elif 'deltar' in quantity:
            function[0]((d['deltar'][0], d['deltar'][1]), d['deltar'][2] % (nicetext(quantity.replace("deltar-","").split("-")[0]), 
                    nicetext(quantity.replace("deltar-","").split("-")[1])), d['deltar'][3]) 
        elif 'events' == quantity:
            function[0](bottom=0.0, quantity="Events")
        elif 'cut' in quantity:
            function[0]((d['cut'][0], d['cut'][1]), d['cut'][2] % nicetext(quantity.replace("cut-","")), d['cut'][3]) 
        elif 'fraction' in quantity and 'MET' not in quantity:
            function[0]((d[quantity[4:-8]][0], d[quantity[4:-8]][1]), d[quantity[4:-8]][2] % nicetext(quantity[:4]), d[quantity[4:-8]][3])
        elif quantity == 'ratio':
            function[0]((d['ratio'][0], d['ratio'][1]), d['ratio'][2] % (opt.labels[0], opt.labels[1]), d['ratio'][3]) 
        elif quantity in d:     # if no special options, read from dictionary
            function[0]((d[quantity][0], d[quantity][1]), d[quantity][2], d[quantity][3])
        else:
            print '"'+quantity + '" is not defined and therefore directly written to label.'
            function[0](quantity=quantity)
    return ax

def getaxislabels_list(quantity, ax=None):
# can we integrate this function somehow into axislabels??
# currently we need one function to change a given ax elemnt and one to simply return limits+label

    # lower limit, upper limit, label, unit
    if 'phi' in quantity:
        if 'deltaphi' in quantity:
           labels_list = [d['deltaphi'][0], d['deltaphi'][1], d['deltaphi'][2] % (nicetext(quantity.replace("deltaphi-","").split("-")[0]), 
                        nicetext(quantity.replace("deltaphi-","").split("-")[1])), d['deltaphi'][3]]
        elif 'phiabsdiff' in quantity:
             labels_list = [d['phiabsdiff'][0], d['phiabsdiff'][1], d['phiabsdiff'][2] % nicetext(quantity.replace("phiabsdiff","")),
                        d['phiabsdiff'][3]]
        elif 'abs' in quantity:
            labels_list = [d['absphi'][0], d['absphi'][1], d['absphi'][2] % 
                        nicetext(quantity.replace("abs_","").replace("_phi","")) , d['absphi'][3]]
        else:
                labels_list = [d['phi'][0], d['phi'][1], d['phi'][2] % nicetext(quantity.replace("phi","")) , d['phi'][3]]
    elif 'eta' in quantity:
        if 'deltaeta' in quantity:
            labels_list = [d['deltaeta'][0], d['deltaeta'][1], d['deltaeta'][2] % (nicetext(quantity.replace("deltaeta-","").split("-")[0]),
                         nicetext(quantity.replace("deltaeta-","").split("-")[1])), d['deltaeta'][3]]
        elif 'etaabsdiff' in quantity:
            labels_list = [d['deltaeta'][0], d['deltaeta'][1], d['deltaeta'][2] % nicetext(quantity.replace("etaabsdiff","")) , d['deltaeta'][3]]
        elif 'abseta' in quantity:
            labels_list = [d['abseta'][0], d['abseta'][1], d['abseta'][2] % nicetext(quantity.replace("abseta","")) , d['abseta'][3]]
        else:
            labels_list = [d['eta'][0], d['eta'][1], d['eta'][2] % nicetext(quantity.replace("eta","")) , d['eta'][3]]
    elif 'deltar' in quantity:
        labels_list = [d['deltar'][0], d['deltar'][1], d['deltar'][2] % (nicetext(quantity.replace("deltar-","").split("-")[0]), 
                    nicetext(quantity.replace("deltar-","").split("-")[1])), d['deltar'][3]]
    elif 'fraction' in quantity and 'MET' not in quantity:
        labels_list = [d[quantity[4:-8]][0], d[quantity[4:-8]][1], d[quantity[4:-8]][2] % nicetext(quantity[:4]), d[quantity[4:-8]][3]]
    elif quantity in d:
        labels_list = [d[quantity][0], d[quantity][1], d[quantity][2] , d[quantity][3]]
    else:
        labels_list =  [0,1,quantity, ""]
    return labels_list

def getdefaultfilename(quantity, opt, change):
    #create a default filename based on quantity, changes and algorithm/correction
    filename = quantity

    if 'bin' in change:
        filename += "_"+change['bin']

    if 'var' in change:
        filename += "_"+change['var']

    if 'incut' in change:
        if change['incut'] == 'allevents':
            filename += "_nocuts"
        elif change['incut'] is not 'incut':
            filename += "_"+change['incut']

    if 'algorithm' in change:
        filename += "__"+change['algorithm']
    else:
        filename += "__"+opt.algorithm

    if 'correction' in change:
        filename += change['correction']
    else:
        filename += opt.correction

    return filename    

def Save(figure, name, opt, alsoInLogScale=False, crop=True, pad=None):
    _internal_Save(figure, name, opt, pad=pad)

    if alsoInLogScale:
        figure.get_axes()[0].set_yscale('log')
        _internal_Save(figure, name + "_log_scale", opt, crop, pad=pad)

def EnsurePathExists(path):
    full_path = ""
    for p in path.split("/"):
        full_path += p + "/"
        #print "Checking " + full_path
        if not os.path.exists(full_path):
            print "Creating " + full_path
            os.mkdir(full_path)
        
def _internal_Save(figure, name, opt, crop=True, pad=None):
    """Save this figure in all listed data formats.

    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    """
    EnsurePathExists( opt.out )
    
    name = opt.out + '/' + name
    name = name.replace("PFJets", "PF")
    print ' -> Saving as',
    if opt.title is not "": figure.suptitle(opt.title, size='xx-large')
    # this helps not to crop labels
    if crop:
        title = figure.suptitle("I", color='white')
    first = True
    for f in opt.formats:
        if f in ['pdf', 'png', 'ps', 'eps', 'svg']:
            if not first:
                print ",",
            else:
                first = False
            print name + '.' + f,
            if crop:
                if pad is not None:
                    figure.savefig(name + '.' + f,bbox_inches='tight', bbox_extra_artists=[title], pad_inches=pad)
                else:
                    figure.savefig(name + '.' + f,bbox_inches='tight', bbox_extra_artists=[title])
            else:
                figure.savefig(name + '.' + f)
            plt.close(figure)

        else:
            print f, "failed. Output type is unknown or not supported."
    print

