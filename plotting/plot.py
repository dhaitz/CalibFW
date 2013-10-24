#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Main plotting file

   Usage: python plotting/plot.py file1.root file2.root file3.root ...
   First file should usually be data, 2nd file MC!
   For further details: python plotting/plot.py -h
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
import plot_mikko
import plot2d

def plot(cluster=False):
    """Run all plots with the given settings"""

    # settings (1):
    op = plotbase.options(
        algorithm="AK5PFJetsCHS",
        correction="L1L2L3Res",
        out="out/8_TeV_data_mc/",
        labels=["data", "MC", "MC2"],
        colors=['black', '#CBDBF9','#800000', 'blue', '#00FFFF'],
        style=["o", "f", "-", "-", "-"],

        lumi=18258.0,
        energy=8,
        plots=plotresponse.plots
            + plotfractions.plots
            + plot2d.plots
            + plotdatamc.plots
            #+ plot_resolution.plots
            #+ plot_mikko.plots
        )
    module_list = [plotresponse, plotfractions, plot2d, plotdatamc, plot_resolution, plot_mikko]

    # override commandline (3):
    op.normalize = True

    print "Number of files:", len(op.files)
    files=[]
    for f in op.files:
        print "Using as file %d: %s" % (1 + op.files.index(f), f)
        files += [getroot.openfile(f, op.verbose)]

    if cluster: return op, files

    op.bins = getroot.getbins(files[0], op.bins)
    op.eta = getroot.getetabins(files[0], op.eta)
    op.npv = getroot.getnpvbins(files[0], op.npv)
    op.cut = getroot.getcutbins(files[0], op.cut)

    plotbase.plot(module_list, op.plots, files, op)


if __name__ == "__main__":
    plot()
