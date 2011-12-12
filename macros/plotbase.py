# -*- coding: utf-8 -*-
"""Basic plot formatting and helper module.

This module contains all the often used plotting tools

"""

import socket
import getpass

import os
import os.path

import sys
import numpy
import matplotlib
import matplotlib.pyplot as plt
#from matplotlib.pyplot import figure as plt_figure
from time import localtime, strftime, clock
import argparse


from ROOT import gROOT

import getroot


matplotlib.rcParams.update({
    'text.usetex': False,
    'axes.linewidth': 0.8,  # thickness of main box lines
#    'patch.linewidth': 1.5,  # thickness of legend pictures and border
#    'grid.linewidth': 1.3,  # thickness of grid lines
#    'lines.linewidth': 2.5,  # thickness of error bars
#    'lines.markersize': 2.5,  # size of markers
    'font.size': 16,  # axislabels, text
    'legend.fontsize': 18,
    'xtick.labelsize': 16,
    'ytick.labelsize': 16,
    'text.fontsize': 18,
})


def plot(modules, plots, fdata, mc, op):
    # dont display any graphics
    gROOT.SetBatch( True )
  
    """Search for plots in the module and run them."""
    
    for module in modules:    
	if op.verbose:
	    print "%1.2f | Start plotting" % clock()
	if not plots:
	    print "Nothing to do. Please list the plots you want!"
	    plots = []
	for p in plots:
	    if hasattr(module, p):
		print "New plot:",
		getattr(module, p)(fdata, mc, op)
	if op.verbose:
	    print "%1.2f | End" % clock()


def options(
            # standard values go here:
            algorithm="ak5PFJets",
            correction="L1L2L3",
            lumi=0.0,
            energy=7,
            status=None,
            author=None,
            date=None,
            out="out",
            formats=['png', 'pdf'],
            files=[
                "../../data/data_Oct19.root",
                "../../data/powheg_Oct19.root",
            ],
            plots=None,
            # current default
            # new ones: [0, 3, 6, 12], [2, 5, 11, 100]
            npv =  [ (0,2), (3,5), (6,11 ), ( 12,100) ],
            bins=None):
    """Set standard options and read command line arguments

    To be turned into a class with str method and init
    """
    parser = argparse.ArgumentParser(
        description="%(prog)s does all the plotting.",
        epilog="Have fun.")
    # input files
    parser.add_argument('files', metavar='file', type=str, nargs='*',
        default=files,  # type=argparse.FileType('r'), nargs='+',
        help="data and Monte Carlo input root file(s). One data file and at " +
             "least one Monte Carlo file is assumed.")
#    parser.add_argument('-m','--mc', type=argparse.FileType('r'), nargs='+',
#        help="Monte Carlo input file(s) if one likes to use more than one "\
#             "data input file (not implemented yet)")
#    parser.add_argument('-d','--data', type=argparse.FileType('r'), nargs='+',
#        help="data input file(s). This is an alternative to simply"\
#             "specifying files if more than one data input file is required "\
#             "(not implemented yet)")
    parser.add_argument('-D', '--data-label', type=str, nargs='+',
        default="data",
        help="pile-up distributions")
    parser.add_argument('-M', '--mc-label', type=str, nargs='+',
        default="MC",
        help="pile-up distributions")
    parser.add_argument('-p', '--pudist', type=str, nargs='+',
        help="pile-up distributions")
    # lumi and energy settings
    parser.add_argument('-a', '--algorithm', type=str,
        default=algorithm,
        help="output directory for plots")

#    print "Coor " + correction

    parser.add_argument('-c', '--correction', type=str,
        default=correction,
        help="output directory for plots")
    parser.add_argument('-l', '--lumi', type=float,
        default=lumi,
        help="luminosity for the given data in /pb")
    parser.add_argument('-e', '--energy', type=int,
        default=energy,
        help="centre-of-mass energy for the given samples in TeV")
    parser.add_argument('-s', '--status', type=str,
        default=status,
        help="status of the plot (e.g. CMS preliminary)")
    parser.add_argument('-A', '--author', type=str,
        default=author,
        help="status of the plot (e.g. CMS preliminary)")
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
    parser.add_argument('-L', '--layout', type=str, nargs='+',
        default='generic',
        help="layout for the plots. E.g. 'document': serif, LaTeX, pdf; " +
             "'slides': sans serif, big, png; 'generic': slides + pdf. " +
             "This is not implemented yet.")
    parser.add_argument('-P', '--plots', type=str, nargs='+',
        default=plots,
        help="do only this plot/these plots")
    parser.add_argument('-n', '--normalize', action='store_true',
        help="normalize Monte Carlo samples to the event count in data and " +
             "regardless of the given luminosity. This is only applied to " +
             "data/MC comparisons")
    parser.add_argument('-v', '--verbose', action='store_true',
        help="verbosity")
    opt = parser.parse_args()
    # to be substituted by commandline arguments (perhaps changed,
    # no formatting options here? but for multiple MC,
    # colors (predefined sequence?) and labels are needed)
    opt.mc_color = '#CBDBF9'
    opt.data_color = 'black'
    opt.factor = 1.0
    opt.data = opt.files[0]
    opt.mc = opt.files[-1]
    opt.bins = bins
    opt.brackets = False
    opt.npv = npv
    if opt.verbose:
        showoptions(opt)
    return opt

def fail( fail_message ):
    print fail_message
    exit ( 0 )


def showoptions(opt):
    print "Options:"
    for o, v in opt.__dict__.items():
        print "   {0:11}: {1}".format(o, v)


def getfactor(lumi, fdata, fmc, quantity='z_phi', change={}):
    """Get the normalization factor for the f_data file w.r.t. f_mc."""
    histo_data = getroot.gethisto(quantity, fdata, change)
    histo_mc = getroot.gethisto(quantity, fmc, change, True)
    histo_mc.scale(lumi)
    print "    >>> The additional scaling factor is:", (
        histo_data.ysum / histo_mc.ysum)
    return histo_data.ysum / histo_mc.ysum


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


def newplot(ratio=False):
    fig = plt.figure(figsize=[7, 7])
    ax = fig.add_subplot(111)
    ax.minorticks_on()

    if ratio:
        print "The ratio plot template is not yet implemented"
    return fig, ax


def labels(ax, opt=options(), jet=False, bin=None, result=None, legloc='upper right',
           frame=True):
    """This function prints all labels and captions in a plot.

    Several functions are called for each type of label.
    """
    lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
    statuslabel(ax, opt.status)
    energylabel(ax, opt.energy)
    if jet:
        jetlabel(ax, opt.algorithm, opt.correction)    # on demand
    binlabel(ax, bin)
    resultlabel(ax, result)
    authorlabel(ax, opt.author)
    datelabel(ax, opt.date)
    ax.legend(loc=legloc, numpoints=1, frameon=frame )
    return ax


def lumilabel(ax, lumi=0.0, xpos=0.00, ypos=1.01):
    if lumi >= 1000.0:
        ax.text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{fb}^{-1}$" %
            (lumi / 1000.0), va='bottom', ha='left', transform=ax.transAxes)
    elif lumi > 0.0:
        ax.text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{pb}^{-1}$" %
            (lumi), va='bottom', ha='left', transform=ax.transAxes)
    return ax


def energylabel(ax, energy=7, xpos=1.00, ypos=1.01):
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
        jet = r"anti-$k_{T}$ 0.5 PF jets"
    elif "ak7pfjets" in algorithm.lower():
        jet = r"anti-$k_{T}$ 0.7 PF jets"
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
        text = r"$%u < p_\mathrm{T}^\mathrm{Z} < %u" % (low, high)
    elif bin == 'pthat':
        text = r"$%u < \hat{p}_\mathrm{T} < %u" % (low, high)
    elif bin == 'eta':
        if low == 0:
            text = r"$|\eta_\mathrm{jet}| < %u" % (high)
        else:
            text = r"$%u < |\eta_\mathrm{jet}| < %u" % (low, high)
    else:
        text = bin
    ax.text(xpos, ypos, text, va='top', ha='left', transform=ax.transAxes)


def statuslabel(ax, status=None, xpos=0.25, ypos=1.018):
    if status is not None:
        ax.text(xpos, ypos, r"%s" % status, va='bottom', ha='left',
                transform=ax.transAxes)


def resultlabel(ax, text="", xpos=0.05, ypos=0.05):
    if text is not None:
	if "\n" in text:
	    # two lined thingy
	    ypos = 0.12
	
        ax.text(xpos, ypos, text, va='top', ha='left', transform=ax.transAxes)



# same as the old version, but can handle and and y axis indpendetly      
def axislabel_2d(ax, y_q, y_obj, x_q='pt', x_obj='Z', brackets=False):
        
    # set for specific objects
    if x_q == 'npv':
        ax.set_xlabel(r"Number of Primary Vertices [1]",
                      ha="right", x=1)
	ax.set_xlim(0, 20)                      
    else:
	fail ( "x_q " + x_q + " not supported" )
                      
    if y_q == 'datamc_ratio':
        ax.set_ylabel(r"Data/MC", va="top", y=1)
        ax.set_ylim(0.8, 1.1)
#        ax.legend(bbox_to_anchor=(0.65, 0.6, 0.3, 0.2), loc='upper right',
#                  numpoints=1, frameon=True)        
    else:
	fail ( "y_q " + y_q + " not supported" )
        
def axislabel(ax, q='pt', obj='Z', brackets=False):
    """label the axes according to the plotted quantity"""
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


def Save(figure, name, opt, alsoInLogScale=False):
    _internal_Save(figure, name, opt)

    if alsoInLogScale:
        figure.get_axes()[0].set_yscale('log')
        _internal_Save(figure, name + "_log_scale", opt)

def EnsurePathExists( path ):
    
    full_path = ""
    for p in path.split ( "/" ):
	full_path += p + "/";
	print "Checking " + full_path
	if not os.path.exists( full_path ):
	    print "Creating " + full_path
	    os.mkdir ( full_path )

        
def _internal_Save(figure, name, opt):
    """Save this figure in all listed data formats.

    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    """

    EnsurePathExists( opt.out )
    
    name = opt.out + '/' + name
    print ' -> Saving as',
    first = True
    for f in opt.formats:
        if f in ['pdf', 'png', 'ps', 'eps', 'svg']:
            if not first:
                print ",",
            else:
                first = False
            print name + '.' + f,
            figure.savefig(name + '.' + f, dpi=100)

#        elif format in ['txt', 'npz', 'dat']:
#            pass    #Ignore this here, as it is respected elsewhere
        else:
            print f, "failed. Output type is unknown or not supported."
    print


# is that used?
def hist_baseplot(plot_collection, caption, settings, modifierBeforeSave,
                  alsoInLogScale=True):
    tf, ta, tname = makeplot(caption)
    ta = captions(ta, settings, False)

    for (quantName, inpFile, drawParameters, modifierFunc,
         modifierDataFunc) in plot_collection:
        rootHisto = getroot.SafeConvert(inpFile, quantName)

        if modifierDataFunc is not None:
            modifierDataFunc(rootHisto, settings)

        my_fmt = "-"
        my_color = "Red"
        my_drawstyle = "steps-mid"
        my_label = ""

        for drawKey, drawValue in drawParameters.items():
            if drawKey == "fmt":
                my_fmt = drawValue
            if drawKey == "color":
                my_color = drawValue
            if drawKey == "drawstyle":
                my_drawstyle = drawValue
            if drawKey == "label":
                my_label = drawValue
            if drawKey == "log":
                my_label = drawValue

        #print rootHisto.xc
        #print rootHisto.y
        mplHisto = ta.errorbar(rootHisto.xc, rootHisto.y, rootHisto.yerr,
                               drawstyle=my_drawstyle, color=my_color,
                               fmt=my_fmt, capsize=0, label=my_label)

        if modifierFunc is not None:
            modifierFunc(mplHisto, settings)

    if modifierBeforeSave is not None:
        modifierBeforeSave(tf, ta, tname, plot_collection, caption, settings)

    Save(tf, caption, settings, alsoInLogScale)


# obsolete functions (please do not use)
def GetDataOrMC(quantity, rootfile, changes={}, rebin=1):
    print "'GetDataOrMC' is deprecated and has moved to 'getroot.gethisto'"
    getroot.gethisto(quantity, rootfile, changes, rebin)


def genericplot(quantity, q, obj, fdata, custom_keys_data, fmc, custom_keys_mc,
                factor, opt, legloc='center right'):
    #print q, "of the", obj
    print "'genericplot' is deprecated, use to 'plotdatamc.datamcplot'"
    histo_data = getroot.gethisto(quantity, fdata, custom_keys_data)
    histo_mc = getroot.gethisto(quantity, fmc, custom_keys_mc)
    histo_mc.scale(factor)
    tf, ta = newplot(quantity)
    ta.bar(histo_mc.x, histo_mc.y, (histo_mc.x[2] - histo_mc.x[1]),
            bottom=numpy.ones(len(histo_mc.x)) * 1e-6, fill=True,
            facecolor=opt.mc_color, edgecolor=opt.mc_color)
    ta.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr,
            drawstyle='steps-mid', color='#CBDBF9', fmt='-', capsize=0,
            label='MC')
    ta.errorbar(histo_data.xc, histo_data.y, histo_data.yerr,
            drawstyle='steps-mid', color='black', fmt='o', capsize=0,
            label='data')
    labels(ta, opt, legloc=legloc)
    ta.set_ylim(top=histo_mc.ymax * 1.2)
    ta = axislabel(ta, q, obj)
    Save(tf, quantity, opt)


def moregenericplot(quantity, q, obj, fdata, fmc, factor, opt,
                    legloc='center right'):
    print "This function 'moregenericplot' is deprecated"
    print q, "of the", obj
    histo_data = getroot.gethisto(quantity, fdata)
    histo_mc = getroot.gethisto(quantity, fmc)
    histo_mc.scale(factor)
    tf, ta = newplot()
    ta.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr,
            drawstyle='steps-mid', color='FireBrick', fmt='-', capsize=0,
            label='MC')
    ta.errorbar(histo_data.xc, histo_data.y, histo_data.yerr,
            drawstyle='steps-mid', color='black', fmt='o', capsize=0,
            label='data', fillcolor=opt.mcColor)
    labels(ta, opt, legloc=legloc)
    ta.set_ylim(top=histo_mc.ymax * 1.2)
    axislabel(ta, q, obj)
    Save(tf, quantity, opt)


def GetScaleResolution(filename='scale_and_resolution.txt'):
    print "This function 'GetScaleResolution' is deprecated"
    """Read the values for the jet energy scale and the jet energy resolution

    The file should look like:
    #Jet energy scale, scale error-, scale error+:
    #Jet energy resolution, resolution error-, resolution error+:
    1.00 0.01 0.01
    1.00 0.01 0.01
    """
    f = file(filename, 'r')
    scale = []
    resol = []
    while True:
        line = f.readline()
        if  not line:
            break
        if line.find('#') < 0:
            if not scale:
                for v in line.split():
                    scale.append(float(v))
            else:
                for v in line.split():
                    resol.append(float(v))
    print scale
    print resol
    return scale, resol


#deprecated
def captions(ax, opt=options(), algo=""):
    print "This function 'captions' is deprecated and now called 'labels'"
    labels(ax, opt)
    AddAlgoAndCorrectionCaption(ax, algo)
    return ax


def AddAlgoAndCorrectionCaption(ax, algo="ak5PFJets"):
    print "'AddAlgoAndCorrectionCaption' is deprecated, use 'jetlabel'"
    jetlabel(ax, algo, algo)


def AxisLabels(ax, q='resp', obj='jet'):
    print "'AxisLabels' is now deprecated, please use 'axislabel' instead."
    axislabel(ax, q, obj)


def GetNameFromSelection(quantity='zmass', common={}, variation={}):
    print "'GetNameFromSelection' is deprecated, use 'getroot.gethistoname'"
    return [getroot.gethistoname(quantity, common)]


def makeplot(quantity):
    print "This function 'makeplot' is deprecated and now called 'newplot'"
    fig = matplotlib.pyplot.figure(figsize=[7, 7])
    ax = fig.add_subplot(111)
    name = quantity
    return fig, ax, name
