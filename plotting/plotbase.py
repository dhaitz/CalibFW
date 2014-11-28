# -*- coding: utf-8 -*-
"""Basic plot formatting and helper module.

This module contains all the often used plotting tools

"""

import os
import os.path
import sys
import copy
import matplotlib.pyplot as plt
from time import clock
import inspect
import math
import matplotlib
import pkgutil
from matplotlib.backends.backend_pdf import PdfPages
PIL_available = False
try:
	import PIL
	PIL_available = True
except:
	print "Could not import PIL: No image metadata."

from ROOT import gROOT, PyConfig
PyConfig.IgnoreCommandLineOptions = True  # prevents Root from reading argv
gROOT.SetBatch(True)

import plot1d
import plot2d
import modules

import getroot
import plotrc
from labels import *
from fit import *


def plot(op):
    """Search for plots in the module and run them."""
    # dont display any graphics
    gROOT.SetBatch(True)
    whichfunctions = []
    plots = op.plots

    # get the list of all the modules in the 'modules' folder
    module_list = [(module.find_module(name).load_module(name)) for module, name, is_pkg in pkgutil.walk_packages(modules.__path__)]

    if op.verbose:
        showoptions(op)

    files, op = openRootFiles(op.files, op)

    startop = copy.deepcopy(op)

    if op.list:
        printfunctions(module_list)
        sys.exit()

    if op.quantities:
        printquantities(files, op)
        sys.exit()

    remaining_plots = copy.deepcopy(plots)
    # 1. Look if you can direcly call functions in the modules
    if not plots:
        print "Nothing to do. Please list the plots you want!"
        plots = []
    for p in plots:
        for module in module_list:
            if hasattr(module, p):  # plot directly as a function
                print "Doing %s in %s" % (p, module.__name__)
                getattr(module, p)(files, op)
                remaining_plots.remove(p)
        if op != startop:
            whichfunctions += [p + " in " + module.__name__]
    # 2. remaining plots are given to the functionSelector
    if len(remaining_plots) > 0:
        print "Doing remaining plots via function selector..."
        for plot in remaining_plots:
            functionSelector(plot, files, op)

    # check whether the options have changed and warn
    # TODO remove the focus on the opt object, rather check the settings?
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


def functionSelector(plot, datamc, opt):
    """ The functionSelector takes a list of plots and assigns them to a funtion,
        according to naming conventions.
    """
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


def openRootFiles(filenames, opt=None):
    """
        Open the root files and read out the meta information
    """
    if len(filenames) == None:
        return [], opt

    print "Number of files:", len(filenames)
    files = []
    for f in filenames:
        print "Using as file", 1 + filenames.index(f), ":", f
        files += [getroot.openfile(f, opt.verbose)]

    opt = readMetaInfosFromRootFiles(files, opt)
    return files, opt


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
        print '\033[96m%s' % module.__name__
        if module.__doc__ != None:
            print "  \033[92m  ", module.__doc__
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
            if len(changes['selection']) == 1 and len(settings['selection']) == 1:
                selection = [" && ".join([changes['selection'][0], settings['selection'][0]])]
            elif len(changes['selection']) == 1:
                selection = ["%s && %s" % (x, changes['selection'][0]) for x in settings['selection']]
            elif len(settings['selection']) == 1:
                selection = ["%s && %s" % (x, settings['selection'][0]) for x in changes['selection']]
            else:
                print "please implement the code to combine selection-lists of different length!"
                sys.exit()
            nsettings.update(changes)
            nsettings['selection'] = selection
        else:
            nsettings.update(changes)
    return nsettings


def getSettings(opt, changes=None, settings=None, quantity=None):
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
    if not settings.get('xquantities', None):
        settings['xquantities'] = []
    if not settings.get('yquantities', None):
        settings['yquantities'] = []
    args = ['xquantities', 'yquantities', 'selection', 'files']
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


def newPlot(ratio=False, run=False, subplots=1, subplots_X=None,
                                                            subplots_Y=None):
    fig = plt.figure(figsize=[3.1, 3.1])
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
        fig = plt.figure(figsize=[3.1, 4.5])
        ax1 = plotbase.plt.subplot2grid((3, 1), (0, 0), rowspan=2)
        ax1.number = 1
        ax2 = plotbase.plt.subplot2grid((3, 1), (2, 0))
        ax2.number = 2
        fig.add_axes(ax1)
        fig.add_axes(ax2)
        return fig, ax1, ax2
        
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


def setAxisLimits(ax, settings):
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
        ax.set_ylim(float(settings['y'][2 * ax.number - 2:][0]),
                    float(settings['y'][2 * ax.number - 2:][1]))
    elif not hasattr(ax, 'number'):
        ax.set_ylim(settings['y'][0], settings['y'][1])


def getDefaultFilename(quantity, opt, settings):
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


def readMetaInfosFromRootFiles(files, opt,
                            metainfos=['Year', 'Channel', 'Type']):
    """This function reads certain meta data (that must be present as
        strings in the root files) and adds them to a copy of opt. """
    # Create variables of the same name as the metainfos
    for m in metainfos:
        exec('%s = []' % m)
    # Iterate over files, append metadata:
    for f in files:
        for m in metainfos:
            eval(m).append(f.Get(m))
    # Append to copy of opt
    opt2 = copy.deepcopy(opt)
    for m in metainfos:
        setattr(opt2, m.lower() + 's', eval(m))
        opt2.default_options[m.lower() + 's'] = eval(m)
    return opt2


def getDefaultFiles():
    """ Returns the default data and MC files, if they exist. """
    files = []
    for name in ['DATA', 'MC']:
        if os.environ.get(name, False):
            files.append(os.environ[name])
    return files


def Save(figure, settings=None, crop=True, pad=0.1):
    """Save this figure in all listed data formats.

    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    For pdf, the settings are written into metadata.
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
    # this hack helps not to crop labels
    elif crop:
        title = figure.suptitle("I", color='white')
    first = True
    for f in settings['formats']:
        if not first:
            print ",",
        else:
            first = False
        filename = name + '.' + f
        print filename
        if f in ['png', 'ps', 'eps', 'svg']:
            if crop:
                figure.savefig(filename, bbox_inches='tight', bbox_extra_artists=[title], pad_inches=pad)
            else:
                figure.savefig(filename)
        elif f == 'pdf':
                pdffig = PdfPages(filename)
                pdffig.savefig(figure, bbox_inches='tight', bbox_extra_artists=[title], pad_inches=pad)

                # write complete settings into PDF metadata:
                metadata = pdffig.infodict()
                metadata['Author'] = os.environ['USER']
                metadata['Keywords'] = getDictionaryString(settings)
                pdffig.close()
        else:
            print f, "failed. Output type is unknown or not supported."
        # for PNG, we can only add the metadata afterwards
        if f == 'png' and PIL_available:
            im = PIL.Image.open(filename)
            meta = PIL.PngImagePlugin.PngInfo()
            for x in settings:
                if str(settings[x]):
                    meta.add_text(x, str(settings[x]))
                elif type(settings[x]) == list and all(str(item) for item in settings[x]):
                    meta.add_text(x, ",".join([str(item) for item in settings[x]]))
            im.save(filename, "png", pnginfo=meta)

    plt.close(figure)


def getDictionaryString(dictio):
    "This function returns one string containing all string key-value pairs from a given dictionary."""
    string = ""
    for key in dictio.keys():
        if str(dictio[key]):
            string += "%s: %s; \n" % (key, str(dictio[key]))
        elif type(dictio[key] == list) and all(str(item) for item in dictio[key]):
            string += "%s: %s; \n" % (key, ",".join(str(dictio[key])))
    return string
