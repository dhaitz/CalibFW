# -*- coding: utf-8 -*-
"""Main plotting file

   Usage: python macros/plot.py [data.root mc.root]
   For further details: python macros/plot.py -h
   Idea:
     1. every plot is a function, grouped in files plot*.py
        datamc: basic quantities (and cuts)
        response: all responses (with and without extrapolation)
        fractions: jet composition
        (systematic: Z-scale, npu, zeitabhaengig, flavour, jet algo,
         jet size, Calo/PF, Pythia/Herwig, 1-2/more pu, eta (endcap, HF))
    2. plotbase serves to make things easier including formatting
    3. getroot does the interaction with ROOT files
    4. good standard settings:
       settings hierarchy:
        (0) plotBase.options defaults are overwritten by
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


def plot():
    """Run all plots with the given settings"""

    # settings (1):
    op = plotbase.options(
        files=[
            "work/data_2011A/out/closure_data_2011A_only.root",
            "work/mc_DY2011_44A/out/closure_mc_DY2011_Aonly.root",
        ],
        algorithm="AK5PFJetsCHS",
        correction="L1L2L3",
        lumi=2179.0,#4749.9,
        plots=plotdatamc.plots +
              plotresponse.plots +
              plotfractions.plots +
              plot_resolution.plots
        )
    module_list = [plotdatamc, plotresponse, plot_resolution, plotfractions]

    # override commandline (3):
    op.normalize = True

    print "Using Data file", op.files[0]
    print "Using MC file", op.files[1]

    fdata, fmc = [getroot.openfile(f, op.verbose) for f in op.files]
    op.bins = getroot.getbins(fdata,
            [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])

    plotbase.plot(module_list, op.plots, fdata, fmc, op)


if __name__ == "__main__":
    plot()
