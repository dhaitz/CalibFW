# -*- coding: utf-8 -*-
"""Main plotting file

   Usage: python macros/plot.py file1.root file2.root file3.root ...
   First file should usually be data, 2nd file MC!
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
import plot_sandbox
import plotflavour

def plot():
    """Run all plots with the given settings"""

    op = plotbase.options()
    module_list = [plotresponse, plotfractions, plot2d, plotdatamc, plot_resolution, plot_mikko, plot_sandbox]
    
    print "Number of files:", len(op.files)
    files=[]
    for f in op.files:
        print "Using as file", 1+op.files.index(f) ,":" , f
        files += [getroot.openfile(f, op.verbose)]

    plotbase.plot(module_list, op.plots, files, op)


if __name__ == "__main__":
    plot()
