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
from dictionaries import d_plots
import plotresponse
import copy


def datamcplot(quantity, files, opt, fig_axes=(), changes=None, settings=None):
    """Template for all data/MC comparison plots for basic quantities."""

    # if no settings are given, create:
    settings = plotbase.getsettings(opt, changes, settings, quantity)
    print "A %s plot is created with the following selection: %s" % (quantity,
                                                          settings['selection'])

    if 'flavour' in settings['xynames'][0]:
        settings['rebin'] = 4
    # create list with histograms from a ttree/tntuple
    datamc, rootobjects = [], []
    settings['events'] = []
    for f in files:
        rootobjects += [getroot.getobjectfromtree(quantity, f, settings)]
        if settings['special_binning'] is False:
            rootobjects[-1].Rebin(settings['rebin'])
        datamc += [getroot.root2histo(rootobjects[-1], f.GetName(), 1)]
        settings['events'] += [datamc[-1].ysum()]
        if 'flavour' in settings['xynames'][0]:
            datamc[-1].x = [5.5 if x == 20.5 else 6.5 if x == -0.5 else x for x in datamc[-1].x]
            datamc[-1].xc = [6 if x == 21 else 7 if x == 0 else x for x in datamc[-1].xc]


    # if true, create a ratio plot:
    if settings['ratio'] and len(datamc) == 2:
        rootobject = getroot.rootdivision(rootobjects)
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

    #loop over histograms: scale and plot
    for f, l, c, s, rootfile, rootobj in reversed(zip(datamc, settings['labels'],
                  settings['colors'], settings['markers'], files, rootobjects)):
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
            elif settings['lumi'] != None:
                f.scale(opt.lumi)

        if s == 'f':
            if settings['special_binning']:
                widths = [(a - b) for a, b in zip(f.x[1:], f.x[:-1])]
                widths += [0]
            else:
                widths = (f.x[2] - f.x[1])
            ax.bar(f.x, f.y, widths, bottom=numpy.ones(len(f.x))
              * 1e-6, yerr=f.yerr, ecolor=c, label=l, fill=True, facecolor=c, edgecolor=c)
        else:
            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c,
                                                    fmt=s, capsize=0, label=l)

        if settings['fit'] is not None and ("MC" not in l or settings['run'] is
                                                                    not "diff"):
            plotbase.fit(ax, quantity, rootobj, settings, c, l,
                                           datamc.index(f), scalefactor, offset)

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

    if settings['log']:
        ax.set_ylim(bottom=1.0, top=max(d.ymax() for d in datamc) * 2)
        ax.set_yscale('log')

    # save it
    if settings['subplot']:
        del rootobjects
        return
    else:
        plotbase.Save(fig, settings['filename'], opt, settings=settings)

try:
    datamcplot = profile(datamcplot)
except NameError:
    pass  # not running with profiler, that's ok.


def getPUindata(version=''):
    """Estimated pile-up distributions in data

       This function returns the pile-up distributions officially estimated in
       data, i.e. the estimated distribution of npv and of the poisson mean
       (pile-up truth distribution).
    """
    result = getroot.Histo()

    if version == 'numpu':
        result.y = [14270649.239810746, 58990748.352223635, 142150149.40766457, 245256691.15595514, 338436389.97566742, 397985695.68997073, 416213978.05408591, 399465500.10306287, 360392674.57897019, 311175771.31574732, 260361488.5309844, 212678650.06446162, 170173269.53691545, 133433048.3509787, 102403443.7904025, 76780777.957231909, 56143916.895207874, 39980742.414969571, 27700580.582106292, 18664152.486487065, 12228440.899056926, 7792471.5964409728, 4831946.5122446436, 2917355.3677396402, 1716360.6078244832, 984800.02188280306, 551568.04700530577, 301831.7033449692, 161529.35198793598, 84617.549058685574, 43429.244216440435, 21856.99816211635, 10795.351639796547, 5236.5632377837173, 4645.721078383227]
    elif version == 'numputruth':
        result.y = [0.0, 279840.46209498082, 5774196.5336854951, 50638611.730361067, 268771849.74966705, 515883259.2782445, 555096689.70183992, 469327380.55458814, 379750064.11310208, 330774376.95963043, 269479045.19981587, 223542407.91147482, 188292685.89233157, 146804692.82040361, 94443749.898273259, 46031697.288334906, 16923096.857849758, 5181606.4255731292, 1428052.4246575104, 437008.14233305631, 102694.0511659878, 6516.1959370700497]
    else:
        print "There are no PU distributions for", version, "defined in plotdatamc."
        exit(0)
    result.yerr = [math.sqrt(i) for i in result.y]
    result.x = range(len(result))
    result.xc = result.x
    result.normalize()
    assert len(result) == len(result.yerr)
    assert len(result) > 10
    return result


def runplot_diff(files, datamc, ax, settings, quantity):

    settings2 = copy.deepcopy(settings)
    if 'components' in settings['xynames'][1]:
        settings2['x'] = [0, 1.5]
    else:
        settings2['x'] = plotbase.getaxislabels_list(settings['xynames'][1])[:2]

    mc = getroot.getobjectfromtree(quantity.split("_")[0], files[1], settings2)
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
    #ax.axhspan(mc.GetMeanError(),-mc.GetMeanError(),
    #                                    color=settings2['colors'][0], alpha=0.4)
    return datamc, ax, offset


#Some additional submodules ...
def datamc_all(quantity, files, opt, fig_axes=(), subplot=False,
                                             changes=None, settings=None):
    """Plot subplots of one quantity in bins of different variation.
       Loop over the different variations and the different alpha cut values.

       plotbase.getvariationlist gets a list of 'changes' dictionaries, one for each variation bin:
         ch_list = [{'var': 'var_JetEta_0to0_783'}, {'var': 'var_JetEta_0_783to1_305'}, ...]

       fig_axes = plotbase.newplot(subplots=n) creates a plot figure (fig_axes[0]) with a list fig_axes[1]
         of n 'axes' elements (=subplots), where fig_axes[1][n] is the n-th subplot
    """
    if 'run' in quantity:
        run = True
        variations = ['eta', 'zpt']
        rebin = 500
        files = [d for d, name in zip(files, opt.files) if 'data' in name]
    else:
        variations = ['npv', 'jet1eta', 'zpt', 'alpha']

    subtexts = plotbase.getdefaultsubtexts()

    # change this
    run = False

    if quantity in variations:
        variations.remove(quantity)
    for variation in variations:
        for cut, cut_string in zip(opt.cut, getroot.cutstrings(opt.cut)):
            ch_list = plotbase.getvariationlist(variation, opt)
            fig_axes = plotbase.newplot(subplots=len(ch_list), run=run)
            for ch, ax, subtext in zip(ch_list, fig_axes[1], subtexts):
                if variation == 'zpt':
                    ch['var'] = cut_string
                elif variation is not 'alpha':
                    ch['var'] = cut_string + "_" + ch['var']
                if run:
                    runplot(quantity, files, opt, changes=ch, fig_axes=(fig_axes[0], ax), subplot=True, log=log, subtext=subtext, rebin=rebin, legloc=legloc, fit=fit)
                else:
                    datamcplot(quantity, files, opt, changes=ch, subplot=True,
                                   fig_axes=(fig_axes[0], ax), settings=settings)

            if variation == 'alpha':
                text = " for different " + plotbase.nicetext(variation) + " values "
            else:
                text = " in " + plotbase.nicetext(variation) + " bins for " + r"$\alpha$ " + str(cut) + "  "
            title = plotbase.nicetext(quantity) + text + opt.algorithm + " " + opt.correction
            fig_axes[0].suptitle(title, size='x-large')

            if variation == 'alpha':
                text = "_bins__"
            else:
                text = "_bins__alpha_" + str(cut).replace('.', '_') + "__"
            filename = quantity + "/" + quantity + "_in_" + variation + text + opt.algorithm + opt.correction
            plotbase.EnsurePathExists(opt.out + "/" + quantity)
            plotbase.Save(fig_axes[0], filename, opt)
            if variation == 'alpha':
                break


def ratiosubplot(quantity, files, opt, settings):

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
    changes['lumi'] = None
    changes['ratio'] = True
    changes['legloc'] = False
    changes['y'] = [0.5, 1.5]
    changes['labels'] = ['Ratio']
    changes['xynames'] = [settings['xynames'][0], 'datamcratio']

    datamcplot(quantity, files, opt, fig_axes=(fig, ax2), changes=changes, settings=settings)

    fig.subplots_adjust(hspace=0.05)
    ax1.set_xticks([])
    ax1.set_xlabel("")

    plotbase.Save(fig, settings['filename'], opt)


def plotfromdict(datamc, opt, name, blacklist=[]):
    if len(d_plots[name]) == 0:  # emptylist
        datamcplot(name, datamc, opt)

    elif len(d_plots[name]) == 1:  # list contains only arguments
        eval("datamcplot('" + name + "', datamc, opt, " + d_plots[name][0] + ")")

    elif len(d_plots[name]) == 2:  # list contains arguments+function
        eval(d_plots[name][1] + "('" + name + "', datamc, opt, " + d_plots[name][0] + ")")

    elif len(d_plots[name]) == 3:  # list contains arguments+function+name
        eval(d_plots[name][1] + "('" + d_plots[name][2] + "', datamc, opt, " + d_plots[name][0] + ")")


plots = []

if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
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
