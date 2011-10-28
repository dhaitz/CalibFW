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
import plotextrapolation


def plot():
    """Run all plots with the given settings"""
    # settings (1):
    op = plotbase.options(
        files = [
            "../../data/data_Oct19.root",
            "../../data/powheg_Oct19.root",
        ],
        algorithm = "ak5PFJets",
        correction = "L1L2L3CHS",
        lumi = 2179.0,
        plots =  plotdatamc.plots +
                 plotextrapolation.plots +
                 plotfractions.plots,
        )
    # override commandline (3):
    op.normalize = True

    fdata = getroot.openfile(op.data, op.verbose)
    fmc = getroot.openfile(op.mc, op.verbose)
    op.bins = getroot.getbins(fdata, [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])
    plotbase.plot(plotdatamc, op.plots, fdata, fmc, op)
    plotbase.plot(plotfractions, op.plots, fdata, fmc, op)
    plotbase.plot(plotextrapolation, op.plots, fdata, fmc, op)


if __name__ == "__main__":
    plot()
