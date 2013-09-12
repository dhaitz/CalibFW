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
import matplotlib
import matplotlib.pyplot as plt
from time import localtime, strftime, clock
import argparse
import inspect
import math
from ROOT import gROOT, PyConfig
PyConfig.IgnoreCommandLineOptions = True #prevents Root from reading argv
gROOT.SetBatch(True)

# include the 'tools' subfolder:
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(
                        inspect.getfile( inspect.currentframe() ))[0],"tools")))
if cmd_subfolder not in sys.path: sys.path.insert(0, cmd_subfolder)

import plotrc
import plotdatamc
import plotfractions
import plotresponse
import plot_resolution
import plot_mikko
import plot2d
import plot_sandbox
import plot_tagging

import getroot
from labels import *
from fit import *


def plot(modules, plots, datamc, op):
    """Search for plots in the module and run them."""
    # dont display any graphics
    gROOT.SetBatch(True)
    startop = copy.deepcopy(op)
    whichfunctions = []
    remaining_plots = copy.deepcopy(plots)
    for module in modules:
        #print "Doing plots in", module.__name__, "..."
        if op.verbose:
            print "%1.2f | Start plotting" % clock()
        if not plots:
            print "Nothing to do. Please list the plots you want!"
            plots = []
        for p in plots:
            if hasattr(module, p):    #plot directly as a function
                print "Doing %s in %s" % (p, module.__name__)
                getattr(module, p)(datamc, op)
                remaining_plots.remove(p)
            elif module == plotdatamc and p in d_plots:    #if no function available, try dictionary
                print "New plot: (from dictionary)", p, 
                plotdatamc.plotfromdict(datamc, op, p)
                remaining_plots.remove(p)
        if op != startop:
            whichfunctions += [p+" in "+module.__name__]
        if op.verbose:
            print "%1.2f | End" % clock()
    # remaining plots are given to the function_selector
    if len(remaining_plots) > 0:
        print "Doing remaining plots via function selector..."
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
    for plot in plots:
        if '2D' in plot:
            plot2d.twoD(plot[3:], datamc, opt)
        elif "_all" in plot:
            plotdatamc.datamc_all(plot[:-4], datamc, opt)

        # for responseratio-plots
        elif 'responseratio' in plot and len(plot.split('_'))>2:
            #plot = plot.replace('bal','balresp')
            plotresponse.responseratio(datamc, opt, 
                            types=plot.split('_responseratio_')[0].split('_'),
                            over=plot.split('_responseratio_')[1])
        elif 'response' in plot and len(plot.split('_'))>2:
            plot = plot.replace('bal','balresp')
            plotresponse.responseplot(datamc, opt, 
                            types=plot.split('_response_')[0].split('_'),
                            over=plot.split('_response_')[1])
        elif 'ratio' in plot and len(plot.split('_'))>2:
            plot = plot.replace('bal','balresp')
            plotresponse.ratioplot(datamc, opt, 
                            types=plot.split('_ratio_')[0].split('_'),
                            over=plot.split('_ratio_')[1])
        else:
            plotdatamc.datamcplot(plot, datamc, opt)


def options(
            # standard values go here:

            files=None,

            plots=None,

            algorithm="AK5PFJetsCHS",
            correction="L1L2L3Res",

            out="out",
            formats=['png'],

            labels=["data", "MC"],
            colors=['black', '#CBDBF9'],
            markers=["o","f"],
            lumi=19790,
            energy=8,
            status=None,
            author=None,
            date=None,
            layout='generic',
            title="",
            eventnumberlabel=None,
            legloc='center right',

            rebin=5,
            ratio=False,
            fit=None,
            filename=None,

            npv=[(0, 4), (5, 8), (9, 15), (16, 21), (22, 100)],
            cut=[0.3],
            eta=[0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191],
            bins=[30, 40, 50, 60, 75, 95, 125, 180, 300, 1000],
    ):
    """Set standard options and read command line arguments.

    To be turned into a class with str method and init
    """

    parser = argparse.ArgumentParser(
        description="%(prog)s does all the plotting.",
        epilog="Have fun.")

    # input files
    parser.add_argument('files', metavar='file', type=str, nargs='*',
        default=files,
        help="data and Monte Carlo input root file(s). One data file and at " +
             "least one Monte Carlo file is assumed.")
    #plots
    parser.add_argument('-P', '--plots', type=str, nargs='+', default=plots,
        help="do only this plot/these plots. " +
             "The function names are required here.")


    # source options
    source = parser.add_argument_group('Source options')
    source.add_argument('--selection', '-S', type=str,
        default=None,
        help='selection (cut) expression for C++ expressions are valid')
    source.add_argument('--folder', type=str,
        default='incut',
        help="folder in rootfile: 'incut' or 'allevents'")
    source.add_argument('--allalpha', action='store_true',
        default=False,
        help='Extend the alpha range up to 0.4')
    source.add_argument('--alleta', action='store_true',
        default=False,
        help='Extend the eta range beyond 1.3')
    source.add_argument('-a', '--algorithm', type=str,
        default=algorithm, help="Jet algorithm. Default is %(default)s")
    source.add_argument('-c', '--correction', type=str, default=correction,
        help="Jet energy correction level. Default is %(default)s")


    # more general options
    general = parser.add_argument_group('General options')
    general.add_argument('-r', '--rebin', type=int, default=rebin,
        help="Rebinning value n")
    general.add_argument('-R', '--ratio', action='store_true',
        help="do a ratio plot from the first two input files")
    general.add_argument('--ratiosubplot', action='store_true',
        help="Add a ratio subplot")
    general.add_argument('-F', '--fit', type=str, default=fit,
        help="Do a fit. Options: vertical, chi2, gauss, slope, intercept")
    general.add_argument('--extrapolation', type=str, default=False,
        help='For response plots: Apply alpha-extrapolation. Possible values \
              are bin or global')
    general.add_argument('--run', type=str, default=False,
        help='Some special options for runplots. Valid options are true or diff')
    general.add_argument('--special_binning', action='store_true', default=False,
        help='special binning for npv, zpt, eta')
    general.add_argument('-n', '--normalize', action='store_false',
        help="don't normalize Monte Carlo samples to the event count in data ")


    # output settings
    output = parser.add_argument_group('Output options')
    output.add_argument('-o', '--out', type=str, default=out,
        help="output directory for plots")
    output.add_argument('-f', '--formats', type=str, nargs='+', default=formats,
        help="output format for the plots.  Default is %(default)s")
    output.add_argument('--filename', type=str, default=filename,
        help='specify a filename')
    output.add_argument('--root', type=str, default=False,
        help="Name of the histogramm which is then saved into a root file")
    output.add_argument('--save_individually', action='store_true',
        default=False,
        help='save each plot separately')


    # plot labelling 
    formatting = parser.add_argument_group('Formatting options')
    formatting.add_argument('-l', '--lumi', type=float, default=lumi,
        help="luminosity for the given data in /pb. Default is %(default)s")
    formatting.add_argument('-e', '--energy', type=int, default=energy,
        help="centre-of-mass energy for the given samples in TeV. \
                                                       Default is %(default)s" )

    formatting.add_argument('-s', '--status', type=str, default=status,
        help="status of the plot (e.g. CMS preliminary)")
    formatting.add_argument('-A', '--author', type=str, default=author,
        help="author name of the plot")
    formatting.add_argument('--date', type=str, default=date,
        help="show the date in the top left corner. 'iso' is YYYY-MM-DD, " +
             "'today' is DD Mon YYYY and 'now' is DD Mon YYYY HH:MM.")
    formatting.add_argument('-E', '--eventnumberlabel', action='store_true',
        help="add event number label")
    formatting.add_argument('-t', '--title', type=str, default=title,
         help="plot title")
    formatting.add_argument('-L', '--layout', type=str,
        default='generic',
        help="layout for the plots. E.g. 'document': serif, LaTeX, pdf; " +
             "'slides': sans serif, big, png; 'generic': slides + pdf. " +
             "This is not implemented yet.")
    formatting.add_argument('-g', '--legloc', type=str, nargs="?", default=legloc,
        help="Location of the legend. Default is %(default)s")
    formatting.add_argument('--subtext', type=str, default=None,
        help='Add subtext')
    formatting.add_argument('-C', '--colors', type=str, nargs='+', default=colors,
        help="colors for the plots in the order of the files. Default is: "+
             ", ".join(colors))
    formatting.add_argument('-k', '--labels', type=str, nargs='+', default=labels,
        help="labels for the plots in the order of the files. Default is: "+
             ", ".join(labels))
    formatting.add_argument('-m', '--markers', type=str, nargs='+', default=markers,
        help="style for the plot in the order of the files. 'o' for points, \
              '-' for lines, 'f' for fill. Default is: %s" % ", ".join(markers))
    formatting.add_argument('--text', type=str,
        default=None,
        help='Place a text at a certain location. Syntax is --text="abs" or \
                                                          --text="abc,0.5,0.9"')
        

    # AXIS
    axis = parser.add_argument_group('Axis options')
    axis.add_argument('--log', action='store_true', default=None,
         help="log plot")
    axis.add_argument('--xlog', action='store_true', default=None,
         help="xlog plot")
    axis.add_argument('-y', type=float, nargs='+', default=None,
        help="upper and lower limit for y-axis")
    axis.add_argument('-x', type=float, nargs='+', default=None,
        help="upper and lower limit for x-axis")
    axis.add_argument('-z', type=float, nargs='+', default=None,
        help="upper and lower limit for z-axis")
    axis.add_argument('--xynames', type=str, nargs='+', default=None,
        help='x-y-axis label names,')

    axis.add_argument('--xticks', type=float, nargs='+', default=None,
        help="add custom xticks")

    group = parser.add_argument_group('Other options')
    group.add_argument('-v', '--verbose', action='store_true',
        help="verbosity")


    opt = parser.parse_args()

    opt.fit_offset = 0
    parser.set_defaults(fit_offset = 0)
    opt.subplot = False
    parser.set_defaults(subplot = False)

    # get a separate dictionary with only the user-set values
    user_options = {}
    default_options = {}
    for key in vars(opt):
        default_options[key] = parser.get_default(key)
        if vars(opt)[key] is not parser.get_default(key):
            user_options[key] = vars(opt)[key]
    opt.user_options = user_options
    opt.default_options = default_options

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



def debug(string):
    """Print a string and the line number + file of function call."""
    frame,filename,line_number,function_name,lines,index=\
        inspect.getouterframes(inspect.currentframe())[1]
    print "%s  (line %s in %s)" % (string, line_number, filename)


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

def get_selection(settings):
    """Prepare the selections."""

    if 'selection' in settings and settings['selection'] is not None:
        selections = settings['selection'].split(" && ")
    else:
        selections = []
    return " && ".join(list(set(selections)))

def apply_changes(settings, changes):
    """This function updates the settings dictionary with the changes function 
       in a way that the selection is not overwritten but combined.
    """
    settings = copy.deepcopy(settings)
    if changes is not None:
        if 'selection' in changes and settings['selection'] is not None:
            
            changes['selection'] = " && ".join([changes['selection'],
                                                         settings['selection']])

        settings.update(changes)
        changes=None
    return settings


def getsettings(opt, changes=None, settings=None, quantity=None):
    """Create the local settings dictionary.

       The customizable parameters can be accessed via the settings directory 
       that is created from the global 'opt' module and optional other arguments.
    """
    opt = copy.deepcopy(opt)
    
    # if a setting dictionary is given, apply changes (if any)
    if settings is not None:
        apply_changes(settings, changes)

    # 1. create a local copy of the default_settings 
    settings =  copy.deepcopy(opt.default_options)
    # 2. overwrite the default_settings with settings(function argument, e.g. from dictionary):
    if changes is not None:
        settings = apply_changes(settings, changes)

    # 3. overwrite the default_settings with user-settings (=command line arguments):
    if opt.user_options is not {}:
        settings = apply_changes(settings, opt.user_options)

    # 4. create the local settings for quantites and axes:
    if quantity is not None and settings['xynames'] is None:
        settings['xynames'] = quantity.split("_")[::-1]

        if len(settings['xynames'])<2:
            settings['xynames'] += ['events']
    if settings['x'] is None:
        settings['x'] = plotbase.getaxislabels_list(settings['xynames'][0])[:2]

    if settings['y'] is None:
        settings['y'] = plotbase.getaxislabels_list(settings['xynames'][1])[:2]

    return settings


def getdefaultsubtexts():
    return ["a)", "b)", "c)", "d)", "e)", "f)", "g)", "h)", "i)", "j)", "k)", 
                                      "l)", "m)", "n)", "o)", "p)", "q)", "r)"]

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
    elif s == 'genz': return r"GenZ"
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


def newplot(ratio=False, run=False, subplots=1, subplots_X=None, 
                                                            subplots_Y=None):
    fig = plt.figure(figsize=[7, 7])
    #fig.suptitle(opt.title, size='xx-large')
    if subplots is not 1: #Get 4 config numbers: FigXsize, FigYsize, NaxesY, NaxesX
        d = {3:3, 2:2, 7:2}
        if subplots in d:
            x = d [subplots]
            y = int(round(subplots/float(x)))
        elif subplots_Y is not None:
            y = subplots_Y
            x = int(round(subplots/float(y)))
        elif subplots_X is not None:
            x = subplots_X
            y = int(round(subplots/float(x)))
        else:
            y = int(math.sqrt(subplots))
            x = int(round(subplots/float(y)))
        if x * y < subplots:
            x = x+1
        if run:
            a = [14*x, 7*y, y, x]
        else:
            a = [7*x, 7*y, y, x]
        fig = plt.figure(figsize=[a[0], a[1]]) #apply config numbers
        ax = [fig.add_subplot(a[2],a[3],n+1) for n in range(subplots)]
        return fig, ax
    elif ratio: 
        ax = fig.add_subplot(111, position=[0.13, 0.35, 0.83, 0.58])
        ax.number = 1
        ratio = fig.add_subplot(111, position=[0.13, 0.12, 0.83, 0.22], sharex=ax)
        ax.number = 2
        ratio.axhline(1.0, color='black', lw=1)
        return fig, ax, ratio
    elif run:
        fig = plt.figure(figsize=[14, 7])
        ax = fig.add_subplot(111)
        ax.number = 1
        return fig, ax
    else:
        ax = fig.add_subplot(111)
        ax.number = 1
        return fig, ax
    return fig



def setaxislimits(ax, settings):
    """
    Set the axis limits from changes and or options. Default operation mode is:
        1. By default, axis limits are taken from the dictionary
        2. If limits are given in changes, override the dictionary values
        3. If limits are given in opt (command line values), override the values
            from dictionary or changes
    """
    if hasattr(ax, 'number') and (len(settings['x']) >= 2*ax.number):
        ax.set_xlim(settings['x'][2*ax.number-2:][0], 
                                               settings['x'][2*ax.number-2:][1])
    elif not hasattr(ax, 'number'):
        ax.set_xlim(settings['x'][0], settings['x'][1])

    if hasattr(ax, 'number') and (len(settings['y']) >= 2*ax.number):
        ax.set_ylim(settings['y'][2*ax.number-2:][0], 
                                               settings['y'][2*ax.number-2:][1])
    elif not hasattr(ax, 'number'):
        ax.set_ylim(settings['y'][0], settings['y'][1])


def getdefaultfilename(quantity, opt, settings):
    """This function creates a default filename based on quantity, changes and 
       algorithm/correction.
    """
    if 'filename' in settings and settings['filename'] is not None:
        return settings['filename']
    else:
        filename = quantity

    if settings['folder'] is 'allevents':
        filename += "_%s_" % 'allevents'

    if 'algorithm' in opt.user_options:
        filename += "_%s" % settings['algorithm']
    if 'correction' in opt.user_options:
        filename += "_%s" % settings['correction']

    #remove special characters:
    for char in ["*", "/", " "]:
        filename = filename.replace(char,"_")
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



if __name__ == "__main__":
    op = options()
    module_list = [plotresponse, plotfractions, plot2d, plotdatamc, 
                        plot_resolution, plot_mikko, plot_sandbox, plot_tagging]
    print "Number of files:", len(op.files)
    files=[]
    for f in op.files:
        print "Using as file", 1+op.files.index(f) ,":" , f
        files += [getroot.openfile(f, op.verbose)]

    plot(module_list, op.plots, files, op)

