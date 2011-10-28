# -*- coding: utf-8 -*-
"""

"""
import numpy

import getroot
import plotbase


def datamcplot(quantity, q, obj, fdata, fmc, opt, legloc='center right',
               change = {}, log=False, rebin=5):
    """Template for all data/MC comparison plots for basic quantities."""
    # read the values
    if opt.verbose: print q, "of the", obj
    hdata = getroot.gethisto(quantity, fdata, change, rebin)
    hmc = getroot.gethisto(quantity, fmc, change, rebin)
    if opt.normalize:
        hmc.scale(hdata.ysum/hmc.ysum)
    else:
        hmc.scale(opt.lumi)

    # create the plot
    fig, ax = plotbase.newplot()
    ax.bar(hmc.x, hmc.y,(hmc.x[2]-hmc.x[1]),bottom=numpy.ones(len(hmc.x))*1e-6,
        fill=True, facecolor=opt.mc_color, edgecolor=opt.mc_color)
    ax.errorbar(hmc.xc, hmc.y, hmc.yerr, drawstyle='steps-mid',
        color=opt.mc_color, fmt='-', capsize=0, label=opt.mc_label)
    ax.errorbar(hdata.xc, hdata.y, hdata.yerr, drawstyle='steps-mid',
        color=opt.data_color, fmt='o', capsize=0, label=opt.data_label)
    plotbase.labels(ax, opt, legloc=legloc)
    ax.set_ylim(top=hmc.ymax * 1.2)
    ax = plotbase.axislabel(ax, q, obj)

    # save it
    plotbase.Save(fig, quantity, opt)
    if log:
        ax.set_ylim(bottom=1.0)
        ax.set_yscale('log')
        plotbase.Save(fig, quantity+'_log', opt)


# Z boson
def zpt(fdata, fmc, opt):
    datamcplot('z_pt', 'pt', 'Z', fdata, fmc, opt, 'center right', log=True)

def zeta(fdata, fmc, opt):
    datamcplot('z_eta', 'eta', 'Z', fdata, fmc, opt, 'lower center')

def zphi(fdata, fmc, opt):
    datamcplot('z_phi', 'phi', 'Z', fdata, fmc, opt, 'lower center')

def zmass(fdata, fmc, opt):
    datamcplot('zmass', 'mass', 'Z', fdata, fmc, opt)

def zmass_qualitycuts(fdata, fmc, opt):
    datamcplot('zmass', 'mass', 'Z', fdata, fmc, opt, 'center right',
               {'incut':'qualitycuts'}, log=True)


# Leading jet
def jetpt(fdata, fmc, opt):
    datamcplot('jet1_pt', 'pt', 'jet', fdata, fmc, opt, 'center right', log=True)

def jeteta(fdata, fmc, opt):
    datamcplot('jet1_eta', 'eta', 'jet', fdata, fmc, opt, 'lower center')

def jetphi(fdata, fmc, opt):
    datamcplot('jet1_phi', 'phi', 'jet', fdata, fmc, opt, 'lower center')


# Second leading jet
def jet2pt(fdata, fmc, opt):
    datamcplot('jet2_pt', 'pt', 'jet2', fdata, fmc, opt)

def jet2pt_qualitycuts(fdata, fmc, opt):
    datamcplot('jet2_pt', 'pt', 'jet2', fdata, fmc, opt, 'center right',
               {'incut':'qualitycuts'}, log=True)

def jet2eta(fdata, fmc, opt):
    datamcplot('jet2_eta', 'eta', 'jet2', fdata, fmc, opt, 'lower center')

def jet2phi(fdata, fmc, opt):
    datamcplot('jet2_phi', 'phi', 'jet2', fdata, fmc, opt, 'lower center')


def responseplot(method, fdata, fmc, opt, legloc='lower right', change = {}):
    """Template for the data/MC comparison of the response."""
    if opt.verbose:
        print "Response:", method[3:]
    hdata = getroot.gethisto(method, fdata, change, True)
    hmc = getroot.gethisto(method, fmc, change)

    fig, ax = plotbase.newplot()
    ax.errorbar(hmc.x, hmc.y, hmc.yerr,
        color=opt.mc_color, fmt='-', capsize=0, label=opt.mc_label)
    ax.errorbar(hdata.xc, hdata.y, hdata.yerr,
        color=opt.data_color, fmt='o', capsize=0, label=opt.data_label)
    ax = plotbase.labels(ax, opt, legloc=legloc)
    ax.set_ylim(top=hmc.ymax * 1.2)
    ax = plotbase.axislabel(ax, method[3:])
    plotbase.Save(fig, method[3:], opt)


def balance(fdata, fmc, opt):
    responseplot('../jetresp', fdata, fmc, opt)

def mpf(fdata, fmc, opt):
    responseplot('../mpfresp', fdata, fmc, opt)


plots = ['zpt', 'zeta', 'zphi', 'zmass', 'zmass_qualitycuts',
    'jetpt', 'jeteta', 'jetphi',
    'jet2pt', 'jet2pt_qualitycuts', 'jet2eta', 'jet2phi',
    'balance', 'mpf' ]


if __name__ == "__main__":
    fdata = getROOT.openFile(plotbase.GetPath() + "data_Oct19.root")
    fmc = getROOT.openFile(plotbase.GetPath() + "pythia_Oct19.root")
    bins = plotbase.guessBins(fdata, [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])
    balance(fdata, fmc, opt=plotbase.commandlineOptions(bins=bins))
    mpf(fdata, fmc, opt=plotbase.commandlineOptions(bins=bins))
