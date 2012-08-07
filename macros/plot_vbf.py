#!/usr/bin/env python
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


def plot():
    """Run all plots with the given settings"""

    # settings (1):
    op = plotbase.options(
        files=[
            "/storage/6/berger/zpj/CalibFW/work/data_2012_vbf/out/closure.root",
            "/storage/6/berger/zpj/CalibFW/work/mc_madgraphSummer12_vbf/out/closure.root",
        ],
        algorithm="AK5PFJetsCHS",
        correction="L1L2L3Res",
        labels=["2012 data", "Madgraph"],
        colors=['black', '#19D175'],
        style=["o","f"],
        lumi=5000.0,#4749.9,
        author="Joram Berger",
        date='today',
        out="out_vbf",
        npv=[(0, 4), (5, 8), (9, 15), (16, 21), (22, 100)],
        plots=plotdatamc.plots +
              plotresponse.plots +
              plotfractions.plots #+
              #plot_resolution.plots
        )
    module_list = [plotdatamc, plotresponse, plot_resolution, plotfractions]

    # override commandline (3):
    op.normalize = True

    print "Using Data file " + op.files[0]
    print "Using MC file " + op.files[1]

    files = [getroot.openfile(f, op.verbose) for f in op.files]
    op.bins = getroot.getbins(files[0],
            [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])

    plotbase.plot(module_list, op.plots, files, op)


if __name__ == "__main__":
    plot()
