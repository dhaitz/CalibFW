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
import plot_mikko


def plot():
    """Run all plots with the given settings"""

    # settings (1):
    op = plotbase.options(
        files=[     # First file must be data, other files MC
            "/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root",
        ],
        #specify output path
        out="out/8_TeV_data_mc/",

        #extend colors, labels, styles:
        labels=["data", "MC", "MC 5.0" , "MC 7 TeV (powheg)" , "MC 7 TeV (madgraph)"],
        colors=['black', '#CBDBF9','#800000', 'blue', '#00FFFF'],
        style=["o","f","-","-","-"],
        
        energy=8,
        lumi=5100,

        algorithm="AK5PFJetsCHS",
        correction="L1L2L3Res",

        plots= plotdatamc.plots
               +plotresponse.plots
               +plotfractions.plots
               #+plot_resolution.plots
               #+plot_mikko.plots
        )
    module_list = [plotdatamc, plotresponse, plotfractions, plot_resolution, plot_mikko]

    print "Number of files:", len(op.files)
    files=[]
    for f in op.files:
        print "Using as file", 1+op.files.index(f) ,":" , f
        files += [getroot.openfile(f, op.verbose)]

    #op.bins = getroot.getbins(files[0], op.bins)
    #op.eta = getroot.getetabins(files[0], op.eta)
    #op.npv = getroot.getnpvbins(files[0], op.npv)
    #op.cut = getroot.getcutbins(files[0], op.cut)

    plotbase.plot(module_list, op.plots, files, op)


if __name__ == "__main__":
    plot()
