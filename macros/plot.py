# -*- coding: utf-8 -*-
"""Main plotting file

   Usage: python macros/plot.py data.root mc.root
   For further details: python macros/plot.py -h
   Idea:
     1. every plot is a function, grouped in files plot*.py
        datamc: basic quantities (and cuts)
        extrapolation: (all extrapolation versions)
        fractions
        (systematic: Z-scale, npu, zeitabhaengig, flavour, jet algo,
         jet size, Calo/PF, Pythia/Herwig, 1-2/more pu, eta (endcap, HF))
    2. plotbase serves to make things easier including formatting
    3. getroot does the interaction with ROOT files
    4. good standard settings:
       settings hierarchy:
        (0) plotBase.commandlineOptions defaults are overwritten by
        (1) arguments in main are overwritten by
        (2) commandline arguments are overwritten by
        (3) changes afterwards in main

"""
import plotbase
import getroot

import plotdatamc
import plotfractions
import plotextrapolation_mc_data_ratio
import plot_resolution
#import plotextrapolation


def plot():
    """Run all plots with the given settings"""

    # settings (1):
    op = plotbase.options(
        files=[
            "work/data_2011AB/out/closure_data_2011AB_one.root",
            "work/mc_DY2011/out/closure_mc_DY2011_one.root",
        ],
        algorithm="AK5PFJetsCHS",
        correction="L1L2L3",
        lumi=4749.9,
        plots=plotdatamc.plots +
              plotextrapolation_mc_data_ratio.plots +
              plotfractions.plots +
              plot_resolution.plots
        )
    module_list = [plotdatamc, plotextrapolation_mc_data_ratio, plot_resolution, plotfractions]

    # override commandline (3):
    op.normalize = True

    print "Using Data file " + op.data
    print "Using MC file " + op.mc

    fdata = getroot.openfile(op.data, op.verbose)
    fmc = getroot.openfile(op.mc, op.verbose)
    op.bins = getroot.getbins(fdata,
            [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])
    op.npv = [(0,2), (3,5), (6,11)]

    plotbase.plot(module_list, op.plots, fdata, fmc, op)
#    plotbase.plot([plotfractions], op.plots, fdata, fmc, op)


if __name__ == "__main__":
    plot()
