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
import plotresponse
import plot_resolution

#import plot_extrapolation


def plot():
    """Run all plots with the given settings"""

    print "plot 1.0"

    module_list = [plotfractions, plotdatamc, plotresponse, plot_resolution]
    # settings (1):
    op = plotbase.options(
        files=[
            "work/data_2011A/out/closure_data_2011A.root",
            "work/mc_DY2011/out/closure_mc_DY2011.root",
        ],
        algorithm="AK5PFJetsCHS",
        correction="L1L2L3",
        lumi=2200.0,
        plots= plot_resolution.plots #+
           #plotdatamc.plots #+
           #plotresponse.plots
           #plotfractions.plots,
        )
    # override commandline (3):
    op.normalize = True

    op.out = "out/data_mc_2011A"

    plotbase.EnsurePathExists( op.out )

    print "Using Data file " + op.files[0]
    print "Using MC file " + op.files[1]

    fdata = getroot.openfile(op.files[0], op.verbose)
    fmc = getroot.openfile(op.files[1], op.verbose)
    op.bins = getroot.getbins(fdata,
            [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])
    op.npv = [(0, 2), (3, 5), (6, 11)]
    op.verbose = True

    plotbase.plot( module_list, op.plots, fdata, fmc, op)

    op.algorithm = "AK5PFJets"
    op.npv = [(0, 2), (3, 5), (6, 11)]
    plotbase.plot( module_list, op.plots, fdata, fmc, op)

if __name__ == "__main__":
    plot()
