#!/usr/bin/env python
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
        algorithm="AK5PFJetsCHS",
        correction="L1L2L3Res",
        lumi=5051.0,
        energy=7,
        plots=plotdatamc.plots +
              plotresponse.plots +
              plotfractions.plots +
              plot_resolution.plots,
        eta=[0, 1.305, 2.411, 5.0],
        )
    module_list = [plotdatamc, plotresponse, plotfractions, plot_resolution]

    # check if GenJet Plot
    if op.gen:
        op.algorithm = "AK5GenJets"
        op.correction = ""
        op.plots = plotdatamc.genplots
        

    # override commandline (3):
    op.normalize = True

    files=[]
    print "Number of files:", len(op.files)
    for f in op.files:
        print "Using as file", 1+op.files.index(f) ,":" , f
        files += [getroot.openfile(f, op.verbose)]

    op.bins = getroot.getbins(files[0],
            [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])

    plotbase.plot(module_list, op.plots, files, op)


if __name__ == "__main__":
    plot()
