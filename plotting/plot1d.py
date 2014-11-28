# -*- coding: utf-8 -*-
"""Create a 1D histogram / profile plot.

   This module contains several functions to create a 1D plot from opt and files.
"""
import numpy
import math
import copy

import getroot
import plotbase


def datamcplot(quantity, files, opt, fig_axes=(), changes=None, settings=None):
    """Keep this function only for backward compatibility. """
    #TODO remove at some point
    plot1d(quantity, files, opt, fig_axes, changes, settings)


def plot1dFromFilenames(quantity, filenames, opt, fig_axes=(), changes=None, settings=None):
    """ Same as plot1d, but open files before."""
    files, opt = plotbase.openRootFiles(filenames, opt)
    plot1d(quantity, files, opt, fig_axes=fig_axes, changes=changes, settings=settings)


def plot1d(quantity, files, opt, fig_axes=(), changes=None, settings=None):
    """Main function for all 1D  plots."""

    # if no settings are given, create:
    settings = plotbase.getSettings(opt, changes, settings, quantity)
    print "A %s plot is created with the following selection: %s" % (quantity,
                                                          settings['selection'])

    mplhistos, rootobjects = getHistos(quantity, files, settings)

    # if true, save as root file:
    if settings['root'] is not False:
        getroot.saveasroot(rootobjects, opt, settings)
        return

    plot1dFromMplHistos(rootobjects, mplhistos, opt, settings, quantity=quantity, fig_axes=fig_axes)


def plot1dFromMplHistos(rootobjects, mplhistos, opt, settings=None, quantity=None, changes=None, fig_axes=None):
    settings = plotbase.getSettings(opt, changes, settings, quantity)

    fig, ax = plotMpl(rootobjects, mplhistos, opt, settings, quantity, fig_axes)

    formatting(ax, settings, opt, mplhistos, rootobjects)

    # save it
    if settings['subplot']:
        del rootobjects
        return
    else:
        settings['filename'] = plotbase.getDefaultFilename(quantity, opt, settings)
        plotbase.Save(fig, settings)


def getHistos(quantity, files, settings):
    """
        Creates and returns lists of the root and MPL histograms
    """
    # create list with histograms from a ttree/tntuple
    mplhistos, rootobjects = [], []
    settings['events'] = []
    for i, f in enumerate(files):
        rootobjects += [getroot.histofromfile(quantity, f, settings, index=i)]
        mplhistos += [getroot.root2histo(rootobjects[-1], f.GetName(), 1)]
        settings['events'] += [mplhistos[-1].ysum()]
        if 'flavour' in settings['xynames'][0]:
            mplhistos[-1].x = [5.5 if x == 20.5 else 6.5 if x == -0.5 else x for x in mplhistos[-1].x]
            mplhistos[-1].xc = [6 if x == 21 else 7 if x == 0 else x for x in mplhistos[-1].xc]

    if settings['subtract']:
        rootobjects[0].Add(rootobjects[1], -1)
        rootobjects = [rootobjects[0]]
        mplhistos = [getroot.root2histo(rootobjects[0], files[0].GetName(), 1)]

    # if true, create a ratio plot:
    if settings['ratio']:
        if len(mplhistos) == 2:
            rootobject = getroot.rootdivision(rootobjects, settings['normalize'])
            mplhistos = [getroot.root2histo(rootobject, files[0].GetName(), 1)]
        elif settings['stacked']:
            # Get the sum of the stacked histos
            stacked = []
            for rootobject, typ in zip(rootobjects, settings['types']):
                if typ == 'mc':
                    stacked.append(rootobject)
            for i in stacked[1:]:
                stacked[0].Add(i)
            rootobject = getroot.rootdivision([rootobjects[0], stacked[0]], settings['normalize'])
            mplhistos = [getroot.root2histo(rootobject, files[0].GetName(), 1)]
        else:  # more than 2 files
            newobjects, mplhistos = [], []
            for obj in rootobjects[1:]:
                copy = rootobjects[0].Clone()
                newobjects += [getroot.rootdivision([copy, obj], settings['normalize'])]
                mplhistos += [getroot.root2histo(newobjects[-1], files[0].GetName(), 1)]
            rootobjects = newobjects
    return mplhistos, rootobjects


def plotMpl(rootobjects, mplhistos, opt, settings, quantity, fig_axes=None):
    """
        Creates the figure and axis objects and plots the histogram lists
    """

    # use the argument-given fig/axis or create new one:
    if settings['subplot'] == True:
        fig, ax = fig_axes
    else:
        fig, ax = plotbase.newPlot(run=settings['run'])

    # if runplot_diff, get the mean from mc:
    if settings['run'] == 'diff':
        mplhistos, ax, offset = runplot_diff(files, mplhistos, ax, settings, quantity)
    else:
        offset = 0

    bottom = []
    #loop over histograms: scale and plot
    for f, l, c, s, rootobj, index in reversed(zip(mplhistos, settings['labels'],
                  settings['colors'], settings['markers'], rootobjects, range(len(rootobjects))[::-1])):
        scalefactor = 1
        if 'Profile' in f.classname or settings['ratio']:
            scalefactor = 1
            #s = s.replace('f','o')
        elif settings['normalize']:
            if 'scalefactor' in settings:
                f.scale(settings['scalefactor'])
            elif (f.ysum() != 0 and mplhistos[0].ysum() != 0):
                scalefactor = mplhistos[0].ysum() / f.ysum()
                f.scale(scalefactor)

        if settings['verbose']:
            print "\n\033[92mHistogram for '%s'\033[0m\n" % l, f

        if s == 'f':
            if settings['special_binning']:
                widths = [(a - b) for a, b in zip(f.x[1:], f.x[:-1])]
                widths += [0]
            else:
                if settings['nbins'] > 1:
                    widths = (f.x[2] - f.x[1])
                else:
                    widths = settings['x'][1] - settings['x'][0]

            if settings['stacked'] and settings['types'][index] == 'mc':
                if len(bottom) > 0:
                    bottom = [b + d for b, d in zip(bottom, mplhistos[len(mplhistos) - index].y)]
                elif index > 0:
                    bottom = mplhistos[len(mplhistos) - index].y
                else:
                    bottom = [0] * len(mplhistos[0].y)
            else:
                bottom = numpy.ones(len(f.x)) * 1e-6

            ax.bar(f.x, f.y, widths, bottom=bottom,
              yerr=f.yerr, ecolor=c, label=l, fill=True, facecolor=c, edgecolor=c)
        else:
            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c,
                                                    fmt=s, capsize=0, label=l)

        if settings['fit'] is not None and ("MC" not in l or settings['run'] is
                                                                    not "diff"):
            plotbase.fit(ax, quantity, rootobj, settings, c, l,
                                           index, scalefactor, offset)
    if len(settings.get('fitvalues', [])) == 2:
        ratio = settings['fitvalues'][1][0] / settings['fitvalues'][0][0]
        ratioerr = math.sqrt(settings['fitvalues'][1][1] ** 2 + settings['fitvalues'][0][1] ** 2)
        ax.text(0.03, 0.95 - (len(mplhistos) / 20.), r"$\mathrm{Ratio:\hspace{1.5}} R = %1.3f\pm%1.3f$" % (ratio, ratioerr),
               va='top', ha='left', transform=ax.transAxes, color='black')
    return fig, ax


def formatting(ax, settings, opt, mplhistos, rootobjects=None):
    """This function takes an axis object and formats it according to settings."""

    # determine axis limits if not automatically determined
    # TODO is there a better place for this?
    if rootobjects is not None:
        if settings['x'] == [0, 1]:
            settings['x'][0] = min([histo.GetXaxis().GetXmin() for histo in rootobjects])
            settings['x'][1] = max([histo.GetXaxis().GetXmax() for histo in rootobjects])
            print "determine x axis borders automatically:", ",".join([str(x) for x in settings['x']])
        if 'TProfile' in [histo.ClassName() for histo in rootobjects] and settings['y'] == [0, 1] and 'y' not in opt.user_options:
            settings['y'][0] = min([histo.GetMinimum() for histo in rootobjects])
            settings['y'][1] = max([histo.GetMaximum() for histo in rootobjects]) * 1.02
            print "determine y axis borders automatically: %s, %s" % (settings['y'][0], settings['y'][1])

    # set the axis labels and limits
    plotbase.labels(ax, opt, settings, settings['subplot'])
    plotbase.axislabels(ax, settings['xynames'][0], settings['xynames'][1],
                                                            settings=settings)
    plotbase.setAxisLimits(ax, settings)
    if 'events' in settings['xynames'][1].lower() and 'y' not in opt.user_options:
        ax.set_ylim(top=max(d.ymax() for d in mplhistos) * 1.2)

    #plot a vertical line at 1.0 for certain y-axis quantities:
    if ((settings['xynames'][1] in ['response', 'balresp', 'mpfresp', 'recogen',
                          'ptbalance', 'L1', 'L2', 'L3', 'mpf', 'mpfresp']) or
                          'cut' in settings['xynames'][1] or settings['ratio']):
        ax.axhline(1.0, color='black', linestyle=':')

    if settings['grid']:
        ax.grid(True)

    if 'flavour' in settings['xynames'][0]:
        ax.set_xlim(0.5, 7.5)
        ax.set_xticks([1, 2, 3, 4, 5, 6, 7])
        ax.set_xticklabels(['d', 'u', 's', 'c', 'b', 'g', 'undef.'])
        ax.axvline(6.5, color='black', linestyle=':')

    if settings['log'] and ax.number == 1:
        if 'y' not in opt.user_options:
            ax.set_ylim(bottom=1.0, top=max(d.ymax() for d in mplhistos) * 2)
        ax.set_yscale('log')


try:
    datamcplot = profile(datamcplot)
except NameError:
    pass  # not running with profiler, that's ok.


def runplot_diff(files, mplhistos, ax, settings, quantity):

    settings2 = copy.deepcopy(settings)
    if 'components' in settings['xynames'][1]:
        settings2['x'] = [0, 1.5]
    else:
        settings2['x'] = plotbase.getaxislabels_list(settings['xynames'][1])[:2]

    mc = getroot.histofromfile(quantity.split("_")[0], files[1], settings2)
    offset = mc.GetMean()

    new_y = []
    for x_elem, y_elem, yerr_elem in zip(mplhistos[0].xc, mplhistos[0].y,
                                                                mplhistos[0].yerr):
        if y_elem == 0.0:
            mplhistos[0].xc.remove(x_elem)
            mplhistos[0].y.remove(y_elem)
            mplhistos[0].yerr.remove(yerr_elem)
        else:
            new_y.append(y_elem - offset)

    mplhistos[0].y = new_y
    return mplhistos, ax, offset


def plot1dratiosubplotFromFilenames(quantity, filenames, opt, changes=None, settings=None):
    """ Same as plot1dratiosubplot, but open files before."""
    files, opt2 = plotbase.openRootFiles(filenames, opt)
    plot1dratiosubplot(quantity, files, opt2, changes=changes, settings=settings)


def plot1dratiosubplot(quantity, files, opt, changes=None, settings=None):
    """ Basically the same as the plot1d function,
        but creates an additional ratiosubplot at the bottom.
    """
    #TODO Currently the histograms are created twice, once 'normally' and once
    #     for the ratio plot. Improve this

    settings = plotbase.getSettings(opt, changes, settings, quantity)

    fig = plotbase.plt.figure(figsize=[7, 10])
    ax1 = plotbase.plt.subplot2grid((3, 1), (0, 0), rowspan=2)
    ax1.number = 1
    ax2 = plotbase.plt.subplot2grid((3, 1), (2, 0))
    ax2.number = 2
    fig.add_axes(ax1)
    fig.add_axes(ax2)

    # primary ploy
    plot1d(quantity, files, opt, fig_axes=(fig, ax1), changes={'subplot': True}, settings=settings)

    # prepare ratiosubplot
    changes = {'subplot': True,
        'ratio': True,
        'y': [None, None] + settings.get('ratiosubploty', [0.5, 1.5]),
        'fit': settings.get('ratiosubplotfit', None),
        'legloc': False,
    }
    if len(files) == 2:
        changes.update({
            'labels': ['Ratio'],
            'markers': ['o'],
            'xynames': [settings['xynames'][0], " / ".join(settings['labels'][:2])],
        })
    else:
        changes.update({
            #'legloc': settings.get('ratiosubplotlegloc', 'lower right'),
            'labels': settings['labels'][1:],
            'colors': settings['colors'][1:],
            'markers': settings['markers'][1:],
            'xynames': [settings['xynames'][0], "Data/MC ratio"],
        })

    if settings['stacked']:
        changes['xynames'] = [settings['xynames'][0], " / ".join([settings['labels'][0], 'mc'])]

    for i, marker in enumerate(changes['markers']):
        if marker == 'f':
            changes['markers'][i] = 'o'
    plot1d(quantity, files, opt, fig_axes=(fig, ax2), changes=changes, settings=settings)

    fig.subplots_adjust(hspace=0.1)
    ax1.set_xticklabels([])
    ax1.set_xlabel("")

    settings['filename'] = plotbase.getDefaultFilename(quantity, opt, settings)
    plotbase.Save(fig, settings)


def ratiosubplot(quantity, files, opt, settings, changes):
    """WARNING!! ratiosubplot. This function is kept for backwards compatibility. Remove at some point."""
    fig = plotbase.plt.figure(figsize=[7, 10])
    ax1 = plotbase.plt.subplot2grid((3, 1), (0, 0), rowspan=2)
    ax1.number = 1
    ax2 = plotbase.plt.subplot2grid((3, 1), (2, 0))
    ax2.number = 2
    fig.add_axes(ax1)
    fig.add_axes(ax2)

    changes = {'subplot': True}

    datamcplot(quantity, files, opt, fig_axes=(fig, ax1), changes=changes, settings=settings)

    changes['energy'] = None
    #changes['lumi'] = None
    changes['ratio'] = True
    changes['legloc'] = False
    if 'ratiosubploty' in settings:
        settings['ratiosubploty'] = [float(x) for x in settings['ratiosubploty']]
    changes['y'] = [None, None] + settings.get('ratiosubploty', [0.5, 1.5])
    changes['labels'] = ['Ratio']
    changes['xynames'] = [settings['xynames'][0], 'datamcratio']
    changes['fit'] = settings.get('ratiosubplotfit', None)
    datamcplot(quantity, files, opt, fig_axes=(fig, ax2), changes=changes, settings=settings)

    fig.subplots_adjust(hspace=0.05)
    ax1.set_xticks([])
    ax1.set_xlabel("")

    plotbase.Save(fig, settings)

