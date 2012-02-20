# -*- coding: utf-8 -*-
"""Plot quantities from data and MC.

   This module is dedicated to the simple comparison of plots from data and MC.
   The plots must be already present in the closure output files.
"""
import numpy
import math

import getroot
import plotbase

def datamcplot(quantity, fdata, fmc, opt, legloc='center right',
               change={}, log=False, rebin=5, file_name = ""):
    """Template for all data/MC comparison plots for basic quantities."""
    # read the values
    if opt.verbose:
        print quantity

    change = plotbase.createchanges(opt, change)
    if quantity in ['numpu', 'numputruth']:
        hdata = getPUindata(quantity)
    else:
        hdata = getroot.getplotfromnick(quantity, fdata, change, rebin)
    hmc = getroot.getplotfromnick(quantity, fmc, change, rebin)
    print hdata.ysum()
    if opt.normalize and 'cut_' not in quantity:
        hmc.scale(hdata.ysum() / hmc.ysum())
    elif 'cut_' not in quantity:
        hmc.scale(opt.lumi)

    # create the plot
    fig, ax = plotbase.newplot()
    ax.bar(hmc.x, hmc.y, (hmc.x[2] - hmc.x[1]),
           bottom=numpy.ones(len(hmc.x)) * 1e-6, fill=True,
           facecolor=opt.mc_color, edgecolor=opt.mc_color)
    ax.errorbar(hmc.xc, hmc.y, hmc.yerr, drawstyle='steps-mid',
        color=opt.mc_color, fmt='-', capsize=0, label=opt.mc_label)
    ax.errorbar(hdata.xc, hdata.y, hdata.yerr, drawstyle='steps-mid',
        color=opt.data_color, fmt='o', capsize=0, label=opt.data_label)
    plotbase.labels(ax, opt, legloc=legloc, frame=True)
    ax.set_ylim(top=hmc.ymax() * 1.2)
    if 'cut_' in quantity and '_npv' in quantity:
        ax = plotbase.axislabels(ax, 'npv', quantity)
    elif 'cut_' in quantity and '_zpt' in quantity:
        ax = plotbase.axislabels(ax, 'z_pt', quantity)
    else:
        ax = plotbase.axislabels(ax, quantity)

    if not file_name:
        if 'incut' in change and change['incut'] == 'allevents':
            file_name = quantity + "_nocuts"
        elif 'bin' in change:
            file_name = change['bin'] + "_" + quantity
            ranges = change['bin'][2:].split('to')
            plotbase.binlabel(ax, 'ptz', int(ranges[0]), int(ranges[1]))
        else:
            file_name = quantity

    # save it
    plotbase.Save(fig, file_name, opt)
    if log:
        ax.set_ylim(bottom=1.0)
        ax.set_yscale('log')
        plotbase.Save(fig, quantity + '_log', opt)


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

# NPV
def npv(fdata, fmc, opt):
    datamcplot('npv', fdata, fmc, opt, 'center right', rebin = 1)


def npv_nocuts(fdata, fmc, opt):
    datamcplot('npv', fdata, fmc, opt, 'center right', {'incut': 'allevents'}, rebin = 1)


def numpu(fdata, fmc, opt):
    datamcplot('numpu', fdata, fmc, opt, 'center right', rebin = 1)


def numpu_nocuts(fdata, fmc, opt):
    datamcplot('numpu', fdata, fmc, opt, 'center right', {'incut': 'allevents'}, rebin = 1)


def numputruth(fdata, fmc, opt):
    datamcplot('numputruth', fdata, fmc, opt, 'center right', rebin = 1)


def numputruth_nocuts(fdata, fmc, opt):
    datamcplot('numputruth', fdata, fmc, opt, 'center right', {'incut': 'allevents'}, rebin = 1)


# Z boson
def zpt(fdata, fmc, opt):
    datamcplot('z_pt', fdata, fmc, opt, 'center right', log=True)


def zpt_nocuts(fdata, fmc, opt):
    datamcplot('z_pt', fdata, fmc, opt, 'center right', {'incut': 'allevents'})


def zeta(fdata, fmc, opt):
    datamcplot('z_eta', fdata, fmc, opt, 'lower center')


def zeta_nocuts(fdata, fmc, opt):
    datamcplot('z_eta', fdata, fmc, opt, 'lower center', {'incut': 'allevents'})


def zphi(fdata, fmc, opt):
    datamcplot('z_phi', fdata, fmc, opt, 'lower center')


def zmass(fdata, fmc, opt):
    datamcplot('z_mass', fdata, fmc, opt, rebin=2)


def zmass_nocuts(fdata, fmc, opt):
    datamcplot('z_mass', fdata, fmc, opt, 'center right', {'incut': 'allevents'},
               rebin=2, log=True)


# Leading jet
def jetpt(fdata, fmc, opt):
    datamcplot('jet1_pt', fdata, fmc, opt, 'center right',
               log=True)


def jetpt_nocuts(fdata, fmc, opt):
    datamcplot('jet1_pt', fdata, fmc, opt, 'center right', {'incut': 'allevents'},
               log=True)


def jeteta(fdata, fmc, opt):
    datamcplot('jet1_eta', fdata, fmc, opt, 'lower center')


def jetphi(fdata, fmc, opt):
    datamcplot('jet1_phi', fdata, fmc, opt, 'lower center')


# Second leading jet
def jet2pt(fdata, fmc, opt):
    datamcplot('jet2_pt', fdata, fmc, opt)


def jet2pt_nocuts(fdata, fmc, opt):
    datamcplot('jet2_pt', fdata, fmc, opt, 'center right',
               {'incut': 'allevents'}, log=True)


def jet2eta(fdata, fmc, opt):
    datamcplot('jet2_eta', fdata, fmc, opt, 'lower center')


def jet2phi(fdata, fmc, opt):
    datamcplot('jet2_phi', fdata, fmc, opt, 'lower center')


# cut efficiencies
def cut_all_npv(fdata, fmc, opt):
    datamcplot('cut_all_npv', fdata, fmc, opt, rebin=1)


def cut_backtoback_npv(fdata, fmc, opt):
    datamcplot('cut_backtoback_npv', fdata, fmc, opt)


def cut_jet2pt_npv(fdata, fmc, opt):
    datamcplot('cut_jet2pt_npv', fdata, fmc, opt)


def cut_muon_npv(fdata, fmc, opt):
    datamcplot('cut_muonpt_npv', fdata, fmc, opt)


def cut_zmass_npv(fdata, fmc, opt):
    datamcplot('cut_zmass_npv', fdata, fmc, opt)


def cut_backtoback_zpt(fdata, fmc, opt):
    datamcplot('cut_backtoback_zpt', fdata, fmc, opt)


def cut_jet2pt_zpt(fdata, fmc, opt):
    datamcplot('cut_jet2pt_zpt', fdata, fmc, opt)


def cut_muon_npv(fdata, fmc, opt):
    datamcplot('cut_muonpt_zpt', fdata, fmc, opt)


def cut_zmass_npv(fdata, fmc, opt):
    datamcplot('cut_zmass_zpt', fdata, fmc, opt)


def balresp(fdata, fmc, opt):
    datamcplot('balresp', fdata, fmc, opt)


def mpfresp(fdata, fmc, opt):
    datamcplot('mpfresp', fdata, fmc, opt)


def balresp_bins(fdata, fmc, opt):
    for bin in getroot.binstrings(opt.bins):
        datamcplot('balresp', fdata, fmc, opt, 'upper right', {'bin': bin})

def balresp_fornpv(fdata, fmc, opt):
    for npv in getroot.npvstrings(opt.npv):
        datamcplot('balresp', fdata, fmc, opt, 'upper right', {'var': npv}, file_name=npv + "_balresp")

def mpfresp_bins(fdata, fmc, opt):
    for bin in getroot.binstrings(opt.bins):
        datamcplot('mpfresp', fdata, fmc, opt, 'upper right', {'bin': bin})


def plotany(x, y, fdata, fmc, opt):
    fig, ax = plotbase.newplot() #ratio=True?
    plot = getroot.getgraph(x, y, fdata, opt, root=False)
    ax.errorbar(plot.x, plot.y, plot.yerr,
        color=opt.data_color, fmt='o', capsize=0, label=opt.data_label)
    plot = getroot.getgraph(x, y, fmc, opt, root=False)
    ax.errorbar(plot.x, plot.y, plot.yerr,
        color='FireBrick', fmt='s', capsize=0, label=opt.mc_label)

    plotbase.labels(ax, opt, jet=True)
    plotbase.axislabels(ax, x, y)
    plotbase.Save(fig, "_".join(['plot', x, y, opt.algorithm]) + opt.correction, opt)


#plotanys
def basic_npv(fdata, fmc, opt):
    for y in ['z_mass', 'z_pt', 'z_eta', 'jet1_pt', 'jet1_eta',
              'jet1_neutral_had_fraction', 'jet1_charged_had_fraction',
              'jet1_neutral_em_fraction', 'jet1_charged_em_fraction']:
        plotany('npv', y, fdata, fmc, opt)


def basic_zpt(fdata, fmc, opt):
    for y in ['z_mass', 'z_eta', 'jet1_pt', 'jet1_eta']:
        plotany('z_pt', y, fdata, fmc, opt)



plots = [
    'npv', 'npv_nocuts',
    'zpt', 'zeta', 'zphi', 'zmass',
    'jetpt', 'jeteta', 'jetphi',
    'jet2pt',  'jet2eta', 'jet2phi', 'jet2pt_nocuts',
    'cut_all_npv',
    'balresp', 'mpfresp',
    'basic_npv', 'basic_zpt',
    ]


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    import sys
    if len(sys.argv) < 2:
        print "Usage: python macros/plotfractions.py data_file.root mc_file.root"
        exit(0)
    fdata = getroot.openfile(sys.argv[1])
    fmc = getroot.openfile(sys.argv[2])
    bins = getroot.getbins(fdata, [])
    zpt(fdata, fmc, opt=plotbase.options(bins=bins))
    jeteta(fdata, fmc, opt=plotbase.options(bins=bins))
    cut_all_npv(fdata, fmc, opt=plotbase.options(bins=bins))
