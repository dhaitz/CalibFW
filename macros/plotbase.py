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


from ROOT import gROOT

import getroot
import plotrc


def plot(modules, plots, datamc, op):
    """Search for plots in the module and run them."""
    # dont display any graphics
    gROOT.SetBatch(True)
    startop = copy.deepcopy(op)
    whichfunctions = []
    for module in modules:
        print "Doing plots in", module.__name__, "..."
        if op.verbose:
            print "%1.2f | Start plotting" % clock()
        if not plots:
            print "Nothing to do. Please list the plots you want!"
            plots = []
        for p in plots:
            if hasattr(module, p):
                print "New plot:", p, 
                getattr(module, p)(datamc, op)
                if op != startop:
                    whichfunctions += [p+" in "+module.__name__]
        if op.verbose:
            print "%1.2f | End" % clock()
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

def options(
            # standard values go here:
            algorithm="AK5PFJets",
            correction="L1L2L3",
            lumi=None,
            energy=None,
            status=None,
            author=None,
            date=None,
            out="out",
            labels=["data", "MC"],
            colors=['black', '#CBDBF9'],
            style=["o","-"],
            formats=['png'],
            layout='generic',
            files=None,
            eventnumberlabel=None,
            plots=None,
            npv=[(0, 2), (3, 5), (6, 11), (12, 100)],
            cut=[0.1, 0.15, 0.2, 0.3],
            eta=[0.0, 1.3, 2.8, 5.0],
            gen=None,
            bins=None):
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
        help="output directory for plots")

    parser.add_argument('-c', '--correction', type=str,
        default=correction,
        help="output directory for plots")
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
    parser.add_argument('-n', '--normalize', action='store_true',
        help="normalize Monte Carlo samples to the event count in data and " +
             "regardless of the given luminosity. This is only applied to " +
             "data/MC comparisons")
    parser.add_argument('-v', '--verbose', action='store_true',
        help="verbosity")
    parser.add_argument('-E', '--eventnumberlabel', action='store_true',
        help="add event number label")
    parser.add_argument('-g', '--gen', action='store_true',
        help="for GenJet plots")


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


def showoptions(opt):
    print "Options:"
    for o, v in opt.__dict__.items():
        print "   {0:11}: {1}".format(o, v)
    print "matplotlib settings:"
    for k, v in plotrc.getstyle(opt.layout).items():
        print "   {0:24}: {1}".format(k, v)


def getfactor(lumi, fdata, fmc, quantity='z_phi', change={}):
    """Get the normalization factor for the f_data file w.r.t. f_mc."""
    histo_data = getroot.gethisto(quantity, fdata, change)
    histo_mc = getroot.gethisto(quantity, fmc, change)
    histo_mc.scale(lumi)
    print "    >>> The additional scaling factor is:", (
        histo_data.ysum() / histo_mc.ysum())
    return histo_data.ysum() / histo_mc.ysum()


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


def newplot(ratio=False, subplots=1):
    fig = plt.figure(figsize=[7, 7])
    if subplots == 8:
        fig = plt.figure(figsize=[28, 14])
        ax = [fig.add_subplot(2,4,n+1) for n in range(8)]
        return fig, ax
    elif ratio: 
        ax = fig.add_subplot(111, position=[0.13, 0.35, 0.83, 0.58])
        ratio = fig.add_subplot(111, position=[0.13, 0.12, 0.83, 0.22], sharex=ax)
        ratio.axhline(1.0, color='black', lw=1)
        return fig, ax, ratio
    else:
        ax = fig.add_subplot(111)
        return fig, ax
    return fig


def labels(ax, opt=options(), jet=False, bin=None, result=None, legloc='upper right',
           frame=True):
    """This function prints all labels and captions in a plot.

    Several functions are called for each type of label.
    """
    if opt.lumi is not None:
        lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
    statuslabel(ax, opt.status)
    if opt.energy is not None:
        energylabel(ax, opt.energy)
    if jet:
        jetlabel(ax, opt.algorithm, opt.correction)    # on demand
    binlabel(ax, bin)
    resultlabel(ax, result)
    authorlabel(ax, opt.author)
    datelabel(ax, opt.date)
    ax.legend(loc=legloc, numpoints=1, frameon=frame)
    return ax



def eventnumberlabel(ax, opt, events):
    if opt.eventnumberlabel is True:
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

def jetlabel_string( algorithm, correction):
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

def jetlabel(ax, algorithm="", correction="", posx=0.05, posy=0.95):
  
    res = jetlabel_string( algorithm, correction)
    
    ax.text(posx, posy, res[0], va='top', ha='left', transform=ax.transAxes)
    ax.text(posx, posy - 0.07, res[1], va='top', ha='left',
            transform=ax.transAxes)
            
    if "CHS" in correction or "CHS" in algorithm:
        ax.text(posx, posy - 0.14, r"CHS applied",
            va='top', ha='left', transform=ax.transAxes)
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


def binlabel(ax, bin=None, low=0, high=0, xpos=0.05, ypos=0.05):
    if bin is None:
        return ax
    if bin == 'ptz':
        text = r"$%u < p_\mathrm{T}^\mathrm{Z} / \mathrm{GeV} < %u$" % (low, high)
    elif bin == 'pthat':
        text = r"$%u < \hat{p}_\mathrm{T} / \mathrm{GeV} < %u$" % (low, high)
    elif bin == 'eta':
        if low == 0:
            text = r"$|\eta_\mathrm{jet}| < %u$" % (high)
        else:
            text = r"$%u < |\eta_\mathrm{jet}| < %u$" % (low, high)
    else:
        text = bin
    ax.text(xpos, ypos, text, va='top', ha='left', transform=ax.transAxes)


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
        if brackets:	# units with [] (not allowed by SI system!)
            quantity = r"%s [%s]" % (quantity, unit)
        else:		# units with /
           quantity = r"%s / %s" % (quantity, unit)
    #print "The axis legend string is:", repr(quantity)
    return quantity


def axislabels(ax, x='z_pt', y='events', brackets=False):
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

    #ax.set_ymargin(0.6)
    #ax.set_ylim( bottom =0 )
    # set x labelling
    if x == 'z_pt_log':
        setxaxis((25, 500), r"$p_\mathrm{T}^\mathrm{%s}$" % x[:-7].title(), "GeV")
        ax.semilogx()
        ax.set_xticklabels([r"$10$", r"$100$", r"$1000$"])
        ax.set_xticklabels([r"$20$", r"$30$", r"$40$", r"$50$", r"$60$", r"",
                            r"$80$", r"", r"$200$", r"$300$", r"$400$"],
                            minor=True)
    elif x in ['z_pt', 'jet1_pt', 'jet2_pt', 'jet3_pt']:
        setxaxis((0, 250), r"$p_\mathrm{T}^\mathrm{%s}$" % x[:-3].title(), "GeV")
    elif x in ['abs_z_eta', 'abs_jet1_eta', 'abs_jet2_eta', 'abs_jet3_eta']:
        setxaxis((0.0, 3.5), r"$|\eta^\mathrm{%s}|$" % x[4:-4].title())
    elif x in ['z_eta', 'jet1_eta', 'jet2_eta', 'jet3_eta']:
        setxaxis((-5, 5), r"$\eta^\mathrm{%s}$" % x[:-4].title())
        if 'response' in y: setxaxis((0, 3.5), r"$\eta^\mathrm{%s}$" % x[:-4].title())
        #if obj == 'Z': ax.legend(loc='lower center', numpoints=1, frameon=True)
    elif x in ['z_phi', 'jet1_phi', 'jet2_phi', 'jet3_phi']:
        setxaxis((-3.5, 3.5), r"$\phi^\mathrm{%s}$" % x[:-4].title())
        ax.set_xticks([-3.14159265, -1.57079633, 0.0, 1.57079633, 3.14159265])
        ax.set_xticklabels([r"$-\pi$", r"$-\frac{\pi}{2}$", r"$0$", r"$\frac{\pi}{2}$", r"$\pi$"])
    elif x in ['z_mass']:
        setxaxis((70, 110), r"$m^\mathrm{%s}$" % x[:-5].title(), "GeV")
    elif x == 'balresp':
        setxaxis((0.0, 1.8), r"$p_\mathrm{T}$ balance")
    elif x == 'mpfresp':
        setxaxis((0.3, 1.8), r"$R_\mathrm{MPF}$")
    elif 'numputruth' == x:
        setxaxis((0, 35), r"Pile-up Truth (Poisson mean)")
    elif 'numpu' == x:
        setxaxis((0, 35), r"Number of Primary Vertices")
    elif 'npv' == x:
        setxaxis((0, 35), r"Number of Reconstructed Vertices $n$")
    elif x == 'rho':
        setxaxis((0, 50), r"$\rho$")
    elif 'constituents' == x:
        setxaxis((0, 60), r"Number of Jet Constituents")
    elif 'jet2ratio' == x:
        setxaxis((0, 0.4), r"$p_\mathrm{T}^\mathrm{Jet_2}/p_\mathrm{T}^{Z}$")
    elif 'runs' == x:
        setxaxis((160404, 190000), r"Run")
    else:
        print "x = " + x + " is not defined and therefore directly written to x-label."
        setxaxis(quantity=x)
        #fail("x = " + x + " not supported. You could use e.g. 'z_pt' if appropriate.")

    # set y labelling
    ratio = ""
    if 'ratio' in y:
        ratio = " (data/MC ratio)"
    if 'arb' == y:
        setyaxis(bottom=0.0, quantity="arb. u.")
    elif 'events' == y:
        setyaxis(bottom=0.0, quantity="Events")
    elif 'fracevents' == y:
        setyaxis(bottom=0.0, quantity="Fraction of Events")
    elif 'balresp' in y:
        setyaxis((0.75, 1.00), r"$p_\mathrm{T}$ balance"+ratio)
    elif 'mpfresp' in y:
        setyaxis((0.75, 1.00), r"MPF"+ratio)
    elif 'response' in y:
        setyaxis((0.85, 1.11), r"Response"+ratio)
        if x == 'jet1_eta' : setyaxis((0.75, 1.11), r"Response"+ratio)
    elif 'kfsr' in y:
        setyaxis((0.90, 1.101), r"$k_\mathrm{FSR}$"+ratio)
    elif y == 'resolution':
        setyaxis((0.0, 0.3), "Jet Resolution Ratio")
    elif y == 'resolutionratio':
        setyaxis((-0.5, 2.5), "Jet Resolution Ratio")
    elif 'npv' == y:
        setyaxis((0, 35), r"Number of Reconstructed Vertices $n$")
    elif 'z_pt' == y:
        setyaxis((50, 150), "$p_\mathrm{T}^\mathrm{Z}$")
    elif 'jet1_pt' == y:
        setyaxis((0, 150), "$p_\mathrm{T}^\mathrm{Jet1}$")
    elif 'z_mass' == y:
        setyaxis((90, 93), "$m_\mathrm{Z}$")
    elif 'z_eta' == y:
        setyaxis((-0.3, 0.3), "$\eta_\mathrm{Z}$")
    elif 'z_mass_ratio' == y:
        setyaxis((0.98, 1.02), "$m_\mathrm{Z}$ ratio")
    elif 'datamc_ratio' == y:
        setyaxis((0.80, 1.10), ratio)
    elif 'cut' in y:
        setyaxis(quantity="Cut Infficiency")
    elif 'components' == y or 'fraction' in y:
        setyaxis((0, 1), r"Leading Jet Component Fraction")
    elif 'components_diff' == y:
        setyaxis((-0.05, 0.05), r"Data-MC of Leading Jet Components")
    elif 'extrapol' == y:
        setyaxis((0.86, 1.04), r"Response")
    elif 'xsec' == y:
        setyaxis((0, 20), r"$n_\mathrm{Events} / \mathcal{L}$", "pb$^{-1}$")
    else:
        print "y = " + y + " is not defined and therefore directly written to y-label."
        setyaxis(quantity=y)
        #fail("y = " + y + " not supported. You could use e.g. 'events' if appropriate." )

    return ax
        
def axislabel(ax, q='pt', obj='Z', brackets=False):
    """label the axes according to the plotted quantity"""
    print "plotbase.axislabel is deprecated! Use axislabels instead!"
    # according to quantity q
    def unit(s="", brackets=brackets):
        if s != "":
            if brackets:
                return r"\;[\mathrm{%s}]" % s
            else:
                return r" / \mathrm{%s}" % s
        else:
            return s

    def gev():
        return unit("GeV")
    
        
    # all labels va top ha right x=1 y =1,
    if q == 'pt':
        ax.set_xlabel(r"$p_\mathrm{T}^\mathrm{" + obj + r"} / \mathrm{GeV}$",
                      ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(0, 200)
        ax.set_ylim(bottom=0.0)
    elif q == 'phi':
        ax.set_xlabel(r"$\phi^\mathrm{" + obj + r"}$", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(-3.5, 3.5)
        ax.set_xticks([-3.14159265, -1.57079633, 0.0, 1.57079633, 3.14159265])
        ax.set_xticklabels([r"$-\pi$", r"$-\frac{\pi}{2}$", r"$0$",
                            r"$\frac{\pi}{2}$", r"$\pi$"])
        ax.set_ylim(bottom=0.0)
        ax.legend(loc='lower center', numpoints=1, frameon=True)
    elif q == 'eta':
        ax.set_xlabel(r"$\eta^\mathrm{" + obj + "}$", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(-5.0, 5.0)
        ax.set_ylim(bottom=0.0)
        if obj == 'Z':
            ax.legend(loc='lower center', numpoints=1, frameon=True)
    elif q == 'mass':
        ax.set_xlabel(r"$m_\mathrm{" + obj + "} / \mathrm{GeV}$", ha="right",
                      x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(70, 110)
        ax.set_ylim(bottom=0.0)
    elif q == 'balresp':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} %s$" % gev(), ha="right", x=1)
        ax.set_ylabel(r"$p_\mathrm{T}$ balance", va="top", y=1)
        ax.set_xlim(10, 240)
        ax.set_ylim(0.75, 1.0)
    elif q == 'mpfresp':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"MPF", va="top", y=1)
        ax.set_xlim(10, 240)
        ax.set_ylim(0.75, 1.0)
    elif q == 'datamc_ratio':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"Data/MC", va="top", y=1)
        ax.set_xlim(10, 240)
        ax.set_ylim(0.8, 1.1)
        ax.legend( loc='upper right',
                  numpoints=1, frameon=True)
    elif q == 'cutineff':
        ax.set_ylabel(r"Cut Infficiency", y=1, va="top")
        ax.set_xlabel(r"NRV", x=1)
        ax.set_xlim(0, 25)
        ax.set_ylim(0.0, 1.0)
    elif q == 'recovert':
        ax.set_xlabel(r"Number of reconstructed vertices $n$", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(0, 25)
        ax.set_ylim(bottom=0.0)
    elif q == 'jetconstituents':
        ax.set_xlabel(r"Jet Constituents", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(1, 60)
        #ax.set_xlim(0, 350)
    elif q == 'components':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"Leading Jet Component Fraction", va="top", y=1)
        ax.set_xlim(25, 500)
        ax.set_ylim(0.0, 1.0)
        ax.semilogx()
        ax.set_xticklabels([r"$10$", r"$100$", r"$1000$"])
        ax.set_xticklabels([r"$20$", r"$30$", r"$40$", r"$50$", r"$60$", r"",
                            r"$80$", r"", r"$200$", r"$300$", r"$400$"],
                            minor=True)
    elif q == 'components_diff':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"Data$-$MC of Leading Jet Components", va="top", y=1)
        ax.set_xlim(25, 500)
        ax.set_ylim(-0.05, 0.05)
        ax.semilogx()
        ax.set_xticklabels([r"$10$", r"$100$", r"$1000$"])
        ax.set_xticklabels([r"$20$", r"$30$", r"$40$", r"$50$", r"$60$", r"",
                            r"$80$", r"", r"$200$", r"$300$", r"$400$"],
                            minor=True)
    elif q == 'extrapol':
        if obj == 'jet2':
            ax.set_xlabel(r"$p_\mathrm{T}^{\mathrm{" + obj +
                          r"}}/p_\mathrm{T}^{Z}$", ha="right", x=1)
        elif obj == 'deltaphi':
            ax.set_xlabel(r"$\Delta\phi$", ha="right", x=1)
        ax.set_ylabel(r"Response", va="top", y=1)
        ax.set_xlim(0.0, 0.4)
        ax.set_ylim(0.86, 1.04)
    elif q == 'runs':
        ax.set_xlabel(r"run", ha="right", x=1)
        ax.set_ylabel(r"n_\mathrm{Events} / $\mathcal{L} \; [\mathrm{pb}]$",
                      va="top", y=1)
        ax.set_xlim(160404, 173692)
        ax.set_ylim(0.0, 20.0)
    elif q == 'runlist':
        ax.set_xlabel(r"run range", ha="right", x=1)
        ax.set_ylabel(r"$n_\mathrm{Events} / \mathcal{L} \; [\mathrm{pb}]$",
                      va="top", y=1)
        ax.set_xlim(0, 300)
        ax.set_ylim(0.0, 20.0)
    else:
        print "The quantity", q, "was not found.",
        print "A default formatting of the axis labels is used."
        ax.set_xlabel(r"$p_\mathrm{T} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"arb. units", va="top", y=1)
        ax.set_xlim(0, 350)
        ax.set_ylim(bottom=0.0)
        
    return ax


def Save(figure, name, opt, alsoInLogScale=False, crop=True):
    _internal_Save(figure, name, opt)

    if alsoInLogScale:
        figure.get_axes()[0].set_yscale('log')
        _internal_Save(figure, name + "_log_scale", opt, crop)

def EnsurePathExists(path):
    full_path = ""
    for p in path.split("/"):
        full_path += p + "/"
        #print "Checking " + full_path
        if not os.path.exists(full_path):
            print "Creating " + full_path
            os.mkdir(full_path)
        
def _internal_Save(figure, name, opt, crop=True):
    """Save this figure in all listed data formats.

    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    """
    EnsurePathExists( opt.out )
    
    name = opt.out + '/' + name
    print ' -> Saving as',
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
                figure.savefig(name + '.' + f, bbox_inches='tight',
                               bbox_extra_artists=[title])
            else:
                figure.savefig(name + '.' + f)

        else:
            print f, "failed. Output type is unknown or not supported."
    print

