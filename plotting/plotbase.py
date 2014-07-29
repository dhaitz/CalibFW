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
PyConfig.IgnoreCommandLineOptions = True  # prevents Root from reading argv
gROOT.SetBatch(True)

# use ls and imp to read all of them
import plotrc
import plot1d
import plotfractions
import plotresponse
import plot_resolution
import plot_mikko
import plot2d
import plot_sandbox
import plotcollections
import plot_tagging

import getroot
from labels import *
from fit import *


def plot(op):
    """Search for plots in the module and run them."""
    # dont display any graphics
    gROOT.SetBatch(True)
    startop = copy.deepcopy(op)
    whichfunctions = []
    plots = op.plots

    modules = [plotresponse, plotfractions, plot2d, plot1d, plotcollections,
                        plot_resolution, plot_mikko, plot_sandbox, plot_tagging]

    if op.verbose:
        showoptions(op)

    print "Number of files:", len(op.files)
    files = []
    for f in op.files:
        print "Using as file", 1 + op.files.index(f), ":", f
        files += [getroot.openfile(f, op.verbose)]

    if op.list:
        printfunctions(modules)
        sys.exit()

    if op.quantities:
        printquantities(files, op)
        sys.exit()

    remaining_plots = copy.deepcopy(plots)
    # 1. Look if you can direcly call functions in the modules
    for module in modules:
        if op.verbose:
            print "%1.2f | Start plotting" % clock()
        if not plots:
            print "Nothing to do. Please list the plots you want!"
            plots = []
        for p in plots:
            if hasattr(module, p):  # plot directly as a function
                print "Doing %s in %s" % (p, module.__name__)
                getattr(module, p)(files, op)
                remaining_plots.remove(p)
        if op != startop:
            whichfunctions += [p + " in " + module.__name__]
        if op.verbose:
            print "%1.2f | End" % clock()
    # 2. remaining plots are given to the functionSelector
    if len(remaining_plots) > 0:
        print "Doing remaining plots via function selector..."
        functionSelector(remaining_plots, files, op)

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


try:
    getobjectfromtree = profile(getobjectfromtree)
except NameError:
    pass  # not running with profiler


def functionSelector(plots, datamc, opt):
    """ The functionSelector takes a list of plots and assigns them to a funtion,
        according to naming conventions.
    """
    for plot in plots:
        if '2D' in plot:
            plot2d.twoD(plot[3:], datamc, opt)
        elif "_all" in plot:
            plot1d.datamc_all(plot[:-4], datamc, opt)

        # for responseratio-plots
        elif 'responseratio' in plot and len(plot.split('_')) > 2:
            plotresponse.responseratio(datamc, opt,
                            types=plot.split('_responseratio_')[0].split('_'),
                            over=plot.split('_responseratio_')[1])
        elif 'response' in plot and len(plot.split('_')) > 2:
            plot = plot.replace('bal', 'balresp')
            plotresponse.responseplot(datamc, opt,
                            types=plot.split('_response_')[0].split('_'),
                            over=plot.split('_response_')[1])
        elif 'ratio' in plot and len(plot.split('_')) > 2:
            plot = plot.replace('bal', 'balresp')
            plotresponse.ratioplot(datamc, opt,
                            types=plot.split('_ratio_')[0].split('_'),
                            over=plot.split('_ratio_')[1])
        elif opt.ratiosubplot is True:
            plot1d.plot1dratiosubplot(plot, datamc, opt)
        else:  # simple 1D plot
            plot1d.plot1d(plot, datamc, opt)


def debug(string):
    """Print a string and the line number + file of function call."""
    frame, filename, line_number, function_name, lines, index =\
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


def printfunctions(module_list):
    for module in module_list:
        print '\033[92m%s' % module.__name__
        for elem in inspect.getmembers(module, inspect.isfunction):
            if (inspect.getargspec(elem[1])[0][:2] == ['files', 'opt']):
                print "\033[93m  %s \033[0m" % elem[0]
                if (elem[1].__doc__ is not None):
                    print "     ", elem[1].__doc__


def printquantities(files, opt):
    quantities = {}
    treename = "_".join([opt.folder, opt.algorithm + opt.correction])

    for f, name in zip(files, opt.labels):
        quantities[name] = []

        # Get the ntuple
        ntuple = f.Get(treename)
        if not ntuple and "Res" in treename:
            ntuple = f.Get(treename.replace("Res", ""))

        # Get the list of quantities from the ntuple
        for branch in ntuple.GetListOfBranches():
            quantities[name] += [branch.GetTitle()]
        quantities[name] = set(quantities[name])

    # Print the list of quantities present in ALL Ntuples
    common_set = quantities[quantities.keys()[0]]
    for name in quantities.keys()[1:]:
        common_set = common_set.intersection(quantities[name])
    print '\033[92m%s\033[0m' % "Quantities in ALL files:"
    for q in sorted(common_set, key=lambda v: (v.upper(), v[0].islower())):
        print "  %s" % q

    # Print the list of quantities that are present only in specific files
    for name in quantities.keys():
        quantities[name] = quantities[name].difference(common_set)
        if len(quantities[name]) > 0:
            print '\033[92m%s\033[0m' % "Quantities only in '%s' file:" % name
            for q in sorted(quantities[name], key=lambda v: (v.upper(), v[0].islower())):
                print "  %s" % q


def applyChanges(settings, changes):
    """This function updates the settings dictionary with the changes function
       in a way that the selection is not overwritten but combined.
    """
    nsettings = copy.deepcopy(settings)
    if changes is not None:
        if 'selection' in changes and settings['selection'] is not None:
            selection = " && ".join([changes['selection'], settings['selection']])
            nsettings.update(changes)
            nsettings['selection'] = selection
        else:
            nsettings.update(changes)
    return nsettings


def getsettings(opt, changes=None, settings=None, quantity=None):
    """Create the local settings dictionary.

       The customizable parameters can be accessed via the settings directory
       that is created from the global 'opt' module and optional other arguments.
    """
    # if a setting dictionary is given, apply changes (if any)
    if settings is not None:
        settings = applyChanges(settings, changes)
        return settings

    # 1. create a local copy of the default_settings
    settings = copy.deepcopy(opt.default_options)

    # 2. overwrite the default_settings with settings(function argument, e.g. from dictionary):
    if changes is not None:
        settings = applyChanges(settings, changes)

    # 3. overwrite the default_settings with user-settings (=command line arguments):
    if opt.user_options is not {}:
        settings = applyChanges(settings, opt.user_options)

    # 4. create the local settings for quantites and axes:
    if quantity is not None and settings['xynames'] is None:
        settings['xynames'] = quantity.split("_")[::-1]

        if len(settings['xynames']) < 2:
            settings['xynames'] += ['events']

    if settings['xname'] is not None:
        settings['xynames'][0] = settings['xname']
    if settings['yname'] is not None:
        settings['xynames'][1] = settings['yname']
    if settings['x'] is None:
        settings['x'] = plotbase.getaxislabels_list(settings['xynames'][0])[:2]

    if settings['y'] is None:
        settings['y'] = plotbase.getaxislabels_list(settings['xynames'][1])[:2]

    # consistency check: the length of xaxis, yaxis, selection, files should be
    # either 1 or n
    # TODO explain this better ...
    # TODO Improve the hacky code below
    # TODO automatize labelling
    if not settings.get('xaxis', None):
        settings['xaxis'] = []
    if not settings.get('yaxis', None):
        settings['yaxis'] = []
    args = ['xaxis', 'yaxis', 'selection', 'files']
    n = max([len(settings.get(a, []) or []) for a in args])
    if n > 1:
        for a in args:
            if type(settings[a]) is list:
                if len(settings[a]) == 1:
                    settings[a] = settings[a] * n
                elif len(settings.get(a)) != n and len(settings.get(a)) > 0:
                    print "\033[93mWARNING!! '%s' has length %s! it should have length %s\033[0m" % (a, len(settings.get(a)), n)

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
    if "AK7" in algorithm:
        algorithms = ['AK7PFJets', 'AK7PFJetsCHS']
    else:
        algorithms = ['AK5PFJets', 'AK5PFJetsCHS']
    return algorithms


def getgenname(opt):
    if "AK7" in opt.algorithm:
        gen = 'AK7GenJets'
    else:
        gen = 'AK5GenJets'
    return gen


def nicetext(s):
    if "run" in s:
        return r"Time dependence for " + nicetext(s[:-4])
    elif s in ['z_pt', 'zpt']:
        return r"$p_\mathrm{T}^\mathrm{Z}$"
    elif s in ['jet1_pt', 'jet1pt']:
        return r"$p_\mathrm{T}^\mathrm{Jet 1}$"
    elif s in ['jet2_pt', 'jet2pt']:
        return r"$p_\mathrm{T}^\mathrm{Jet 2}$"
    elif s in ['jet1eta', 'eta']:
        return r"$\eta^\mathrm{Jet1}$"
    elif s in ['jets_valid', 'jetsvalid']:
        return r"Number of valid jets"
    elif s == 'npv':
        return 'NPV'
    elif s == 'alpha':
        return r"$\alpha$"
    elif s == 'balresp':
        return r"$p_\mathrm{T}$ balance"
    elif s == 'baltwojet':
        return r"two-jet balance"
    elif s == 'mpfresp':
        return "MPF response"
    elif s == 'sumEt':
        return r"$\sum E^\mathrm{T}$"
    elif s == 'METsumEt':
        return r"Total transverse energy $\sum E^\mathrm{T}$"
    elif s == 'METpt':
        return r"$p_\mathrm{T}^\mathrm{MET}$"
    elif s == 'METphi':
        return r"$\phi^\mathrm{MET}$"
    elif s == 'met':
        return r"E_\mathrm{T}^\mathrm{miss}"
    elif s == 'MET':
        return r"E_\mathrm{T}^\mathrm{miss}"
    elif s == 'muonsvalid':
        return "Number of valid muons"
    elif s == 'muonsinvalid':
        return "Number of invalid muons"
    elif s == 'muplus':
        return "mu plus"
    elif s == 'muminus':
        return "mu minus"
    elif s == 'leadingjet':
        return r"Leading \/Jet"
    elif s == 'secondjet':
        return r"Second \/Jet"
    elif s == 'leadingjetsecondjet':
        return r"Leading\/ Jet,\/ Second\/ Jet"
    elif s == 'jet1':
        return r"Leading Jet"
    elif s == 'jet2':
        return r"Second \/Jet"
    elif s == 'z':
        return r"Z"
    elif s == 'genz':
        return r"GenZ"
    elif s == 'leadingjetMET':
        return r"Leading\/ Jet,\/ MET"
    elif s == 'jet1MET':
        return r"Leading\/ Jet,\/ MET"
    elif s == 'zMET':
        return r"Z, MET"
    elif s == 'jet2toZpt':
        return r"2nd Jet Cut"
    return s


def newplot(ratio=False, run=False, subplots=1, subplots_X=None,
                                                            subplots_Y=None):
    fig = plt.figure(figsize=[7, 7])
    #fig.suptitle(opt.title, size='xx-large')
    if subplots is not 1:  # Get 4 config numbers: FigXsize, FigYsize, NaxesY, NaxesX
        d = {3: 3, 2: 2, 7: 2}
        if subplots in d:
            x = d[subplots]
            y = int(round(subplots / float(x)))
        elif subplots_Y is not None:
            y = subplots_Y
            x = int(round(subplots / float(y)))
        elif subplots_X is not None:
            x = subplots_X
            y = int(round(subplots / float(x)))
        else:
            y = int(math.sqrt(subplots))
            x = int(round(subplots / float(y)))
        if x * y < subplots:
            x = x + 1
        if run:
            a = [14 * x, 7 * y, y, x]
        else:
            a = [7 * x, 7 * y, y, x]
        fig = plt.figure(figsize=[a[0], a[1]])  # apply config numbers
        ax = [fig.add_subplot(a[2], a[3], n + 1) for n in range(subplots)]
        return fig, ax
    elif ratio:
        ax = fig.add_subplot(111, position=[0.13, 0.35, 0.83, 0.58])
        ax.number = 1
        ax.set_xlabel([])
        ratio = fig.add_subplot(111, position=[0.13, 0.12, 0.83, 0.22], sharex=ax)
        ratio.number = 2
        ratio.axhline(1.0, color='gray', lw=1)
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
    if len(settings['x']) >= 2:
        ax.set_xlim(settings['x'][0], settings['x'][1])

    if hasattr(ax, 'number') and (len(settings['y']) >= 2 * ax.number):
        ax.set_ylim(settings['y'][2 * ax.number - 2:][0],
                    settings['y'][2 * ax.number - 2:][1])
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
        filename = filename.replace(char, "_")
    return filename


def selfsave(settings):
    pass


def Save(figure, settings=None, crop=True, pad=None):
    """Save this figure in all listed data formats.

    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    """
    if not settings:
        print "Please use mpl savefig if no settings are given"
        figure.savefig("plot.png")
        return

    if not os.path.exists(settings['out']):
        os.makedirs(settings['out'])

    name = settings['out'] + '/' + settings['filename']
    name = name.replace("PFJets", "PF")
    print ' -> Saving as',
    if settings is not None and settings['title'] is not "":
        title = figure.suptitle(settings['title'], size='large')
    # this helps not to crop labels
    elif crop:
        title = figure.suptitle("I", color='white')
    first = True
    for f in settings['formats']:
        if f in ['pdf', 'png', 'ps', 'eps', 'svg']:
            if not first:
                print ",",
            else:
                first = False
            print name + '.' + f
            if crop:
                if pad is not None:
                    figure.savefig(name + '.' + f, bbox_inches='tight', bbox_extra_artists=[title], pad_inches=pad)
                else:
                    figure.savefig(name + '.' + f, bbox_inches='tight', bbox_extra_artists=[title])
            else:
                figure.savefig(name + '.' + f)
            plt.close(figure)

        else:
            print f, "failed. Output type is unknown or not supported."
