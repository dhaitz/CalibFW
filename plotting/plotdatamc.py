# -*- coding: utf-8 -*-
"""Plot quantities from data and MC.

   This module is dedicated to the simple comparison of plots from data and MC.
"""
import numpy
import math
import getroot
import plotbase
import ROOT
import sys
import os
import plotresponse
import copy


def datamcplot(quantity, files, opt, fig_axes=(), changes=None, settings=None):
    """Keep this function only for backward compatibility. """
    #TODO remove at some point
    plot1d(quantity, files, opt, fig_axes, changes, settings)


def plot1d(quantity, files, opt, fig_axes=(), changes=None, settings=None):
    """Template for all 1D data/MC comparison plots for basic quantities."""

    # if no settings are given, create:
    settings = plotbase.getsettings(opt, changes, settings, quantity)
    print "A %s plot is created with the following selection: %s" % (quantity,
                                                          settings['selection'])

    if 'flavour' in settings['xynames'][0]:
        settings['nbins'] = 25
    # create list with histograms from a ttree/tntuple
    datamc, rootobjects = [], []
    settings['events'] = []
    for f in files:
        rootobjects += [getroot.histofromfile(quantity, f, settings)]
        datamc += [getroot.root2histo(rootobjects[-1], f.GetName(), 1)]
        settings['events'] += [datamc[-1].ysum()]
        if 'flavour' in settings['xynames'][0]:
            datamc[-1].x = [5.5 if x == 20.5 else 6.5 if x == -0.5 else x for x in datamc[-1].x]
            datamc[-1].xc = [6 if x == 21 else 7 if x == 0 else x for x in datamc[-1].xc]

    if settings['subtract']:
        rootobjects[0].Add(rootobjects[1], -1)
        rootobjects = [rootobjects[0]]
        datamc = [getroot.root2histo(rootobjects[0], files[0].GetName(), 1)]

    # if true, create a ratio plot:
    if settings['ratio'] and len(datamc) == 2:
        rootobject = getroot.rootdivision(rootobjects, settings['normalize'])
        datamc = [getroot.root2histo(rootobject, files[0].GetName(), 1)]
    else:
        rootobject = None

    # use the argument-given fig/axis or create new one:
    if settings['subplot'] == True:
        fig, ax = fig_axes
    else:
        fig, ax = plotbase.newplot(run=settings['run'])

    settings['filename'] = plotbase.getdefaultfilename(quantity, opt, settings)

    # create an additional ratio subplot at the bottom:
    #TODO this is only kept for backwards compatibility! remove at some point
    if settings['ratiosubplot'] and not settings['subplot']:
        ratiosubplot(quantity, files, opt, settings)
        return

    # if runplot_diff, get the mean from mc:
    if settings['run'] == 'diff':
        datamc, ax, offset = runplot_diff(files, datamc, ax, settings, quantity)
    else:
        offset = 0

    # if true, save as root file:
    if settings['root'] is not False:
        getroot.saveasroot(rootobjects, opt, settings)
        return

    bottom = []
    #loop over histograms: scale and plot
    for f, l, c, s, rootfile, rootobj, index in reversed(zip(datamc, settings['labels'],
                  settings['colors'], settings['markers'], files, rootobjects, range(len(files))[::-1])):
        scalefactor = 1
        if 'Profile' in f.classname:
            scalefactor = 1
            #s = s.replace('f','o')
        elif settings['normalize']:
            if 'scalefactor' in settings:
                f.scale(settings['scalefactor'])
            elif (f.ysum() != 0 and datamc[0].ysum() != 0):
                scalefactor = datamc[0].ysum() / f.ysum()
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

            if settings['stacked'] and index > 0:
                if len(bottom) > 0:
                    bottom = [b + d for b, d in zip(bottom, datamc[len(datamc) - index].y)]
                else:
                    bottom = datamc[len(datamc) - index].y
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
        ax.text(0.03, 0.95 - (len(datamc) / 20.), r"$\mathrm{Ratio:\hspace{1.5}} R = %1.3f\pm%1.3f$" % (ratio, ratioerr),
               va='top', ha='left', transform=ax.transAxes, color='black')

    formatting(ax, settings, opt, datamc, rootobjects=None)

    # save it
    if settings['subplot']:
        del rootobjects
        return
    else:
        plotbase.Save(fig, settings)


def formatting(ax, settings, opt, datamc, rootobjects=None):
    """This function takes an axis object and formats it according to settings."""

    # set the axis labels and limits
    plotbase.labels(ax, opt, settings, settings['subplot'])
    plotbase.axislabels(ax, settings['xynames'][0], settings['xynames'][1],
                                                            settings=settings)
    plotbase.setaxislimits(ax, settings)
    if settings['xynames'][1] == 'events' and 'y' not in opt.user_options:
        ax.set_ylim(top=max(d.ymax() for d in datamc) * 1.2)

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
        if 'y' not in opt.user_options and 'y' not in changes:
            ax.set_ylim(bottom=1.0, top=max(d.ymax() for d in datamc) * 2)
        ax.set_yscale('log')


try:
    datamcplot = profile(datamcplot)
except NameError:
    pass  # not running with profiler, that's ok.


def runplot_diff(files, datamc, ax, settings, quantity):

    settings2 = copy.deepcopy(settings)
    if 'components' in settings['xynames'][1]:
        settings2['x'] = [0, 1.5]
    else:
        settings2['x'] = plotbase.getaxislabels_list(settings['xynames'][1])[:2]

    mc = getroot.histofromfile(quantity.split("_")[0], files[1], settings2)
    offset = mc.GetMean()

    new_y = []
    for x_elem, y_elem, yerr_elem in zip(datamc[0].xc, datamc[0].y,
                                                                datamc[0].yerr):
        if y_elem == 0.0:
            datamc[0].xc.remove(x_elem)
            datamc[0].y.remove(y_elem)
            datamc[0].yerr.remove(yerr_elem)
        else:
            new_y.append(y_elem - offset)

    datamc[0].y = new_y
    return datamc, ax, offset


def plot1dratiosubplot(quantity, files, opt, changes=None, settings=None):
    """ Basically the same as the plot1d function,
        but creates an additional ratiosubplot at the bottom.
    """
    #TODO Currently the histograms are created twice, once normally and once
    #     for the ratio plot. Improve this

    settings = plotbase.getsettings(opt, changes, settings, quantity)

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


def ratiosubplot(quantity, files, opt, settings):
    """ratiosubplot. This function os kept for backwards compatibility. Remove at some point."""
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

if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    #TODO do we really need this?
    import sys
    if len(sys.argv) < 2:
        print "Usage: python plotting/plotdatamc.py data_file.root mc_file.root"
        exit(0)
    datamc[0] = getroot.openfile(sys.argv[1])
    fmc = getroot.openfile(sys.argv[2])
    bins = getroot.getbins(datamc[0], [])
    zpt(datamc, opt=plotbase.options(bins=bins))
    jeteta(datamc, opt=plotbase.options(bins=bins))
    cut_all_npv(datamc, opt=plotbase.options(bins=bins))
