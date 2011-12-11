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

#import plot_extrapolation


def plot():
    """Run all plots with the given settings"""

    print "plot 1.0"
    # settings (1):
    op = plotbase.options(
        files=[
            "work/data_2011A_B/out/closure_data_2011A_B.root",
            "work/mc_DY2011_for_2011_full/out/closure_mc_DY2011_2011full.root",
        ],
        algorithm="AK5PFJetsCHS",
        #algorithm="AK5PFJets",
        correction="L1L2L3",
        lumi=4700.0,
        plots= #plot_resolution.plots +
	       #plotdatamc.plots + 
	       plotextrapolation_mc_data_ratio.plots
              #plotfractions.plots,
        )
    # override commandline (3):
    op.normalize = True
    
    op.out = "out/data_mc_2011"

    plotbase.EnsurePathExists( op.out )
    
    print "Using Data file " + op.data
    print "Using MC file " + op.mc

    fdata = getroot.openfile(op.data, op.verbose)
    fmc = getroot.openfile(op.mc, op.verbose)
    op.bins = getroot.getbins(fdata,
            [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])
    #op.npv =  [ (0,2), (3,5), (6,11 ) ]
    op.verbose = True
    
    plotbase.plot( [plotfractions, plotdatamc, plotextrapolation_mc_data_ratio], 
		    op.plots, fdata, fmc, op)

    op.algorithm = "AK5PFJets"
    op.npv = [ (0,2), (3,5), (6,11 ) ]
    plotbase.plot( [plotfractions, plotdatamc, plotextrapolation_mc_data_ratio], 
		    op.plots, fdata, fmc, op)
		    
if __name__ == "__main__":
    plot()
