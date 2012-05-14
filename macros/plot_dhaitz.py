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


def plot():
    """Run all plots with the given settings"""

    # settings (1):
    op = plotbase.options(
        files=[     # First file must be data, other files MC
            "work/data_2012_52/out/closure_data_2011_52.root",
            "work/data_2011A/out/closure_data_2011A_only.root",
            #"work/mc_DY2012_52/out/closure_mc_DY2011_52.root",
            #"work/mc_DY2012_50/out/closure_mc_DY2011_50.root",
            #"work/mc_DY2011/out/closure_mc_DY2011_one.root",
            #"work/mc_DY2011_madgraph/out/closure_mc_DY2011_madgraph_one.root"
        ],
        #specify output path
        out="out/8_TeV_vs_7_TeV/",

        #extend colors, labels, styles:
        labels=["data 8 TeV", "data 7 TeV", "MC 5.0" , "MC 7 TeV (powheg)" , "MC 7 TeV (madgraph)"],
        colors=['#800000', 'blue','#800000', 'blue', '#00FFFF'],
        style=["o","o","-","-","-"],
        
        #energy=8,
        #lumi=264.5

        algorithm="AK5PFJetsCHS",
        correction="L1L2L3",

        plots= plotdatamc.plots 
               +plotresponse.plots
               +plotfractions.plots
               +plot_resolution.plots
        )
    module_list = [plotdatamc, plotresponse, plotfractions, plot_resolution]

    # check if GenJet Plot
    if op.gen:
        op.algorithm = "AK5GenJets"
        op.correction = ""
        op.plots = plotdatamc.genplots
    

    # override commandline (3):
    op.normalize = True

    #print info about used data/mc files:
    print "\n","Using Data file " + op.files[0]
    print "Number of additional files:", len(op.files)-1
    for f in op.files:
        if op.files.index(f) > 0:
            print "Using as file", 1+op.files.index(f) ,":" , f
    
    files = []
    #fill files with data and MC files:
    for f in op.files:
        files.append(getroot.openfile(f, op.verbose))
    op.bins = getroot.getbins(files[0],
            [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])

    plotbase.plot(module_list, op.plots, files, op)


if __name__ == "__main__":
    plot()
