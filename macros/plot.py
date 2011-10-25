# -*- coding: utf-8 -*-

import time
import argparse
import plotBase
import getROOT
import plotDataMC
import plot_fractions
import plot_extrapolation2
#Idea: 
# 1. every plot is a function, grouped in files plotDataMC incl. Response? plotExtrapolation plotSystematic: Z-scale, npu, zeitabhaengig, flavour, jet algo, jet size, Calo/PF,#   Pythia/Herwig1-2/more pu, eta (endcap, HF),
#   pf_fractions; cuts, cuteff; crosschecks: (if available)
# different plots: data/mc (p4, response); extrapolation; special: Z-scale, npu, zeitabhaengig, flavour,
#   pf_fractions; cuts, cuteff; crosschecks: 1-2/more pu, eta (endcap, HF), jet algo, jet size, Calo/PF,
#   Pythia/Herwig
# 2. plotBase serves to make things easier,
#    plotFormat: here is the formatting help: captions labels, axes, ...
# 3. getROOT does the interaction with ROOT
# 4. good standard settings
# 5. files can _only_ be set by commandline intentionally

# settings hierarchy: 
#	(0) plotBase.commandlineOptions defaults are overwritten by
#	(1) arguments in main are overwritten by
#	(2) commandline arguments are overwritten by 
#	(3) changes afterwards in main

def plot():
	# list of plots to do (function names), leave empty if you want the standard selection
	
	# settings (1):
	op = plotBase.commandlineOptions("ak5PFJets", "L1L2L3CHS",
		lumi = 2179.0,
		files = [
			"../../data/data_Oct19.root", 
			"../../data/powheg_Oct19.root",
		],
		plots =  plotDataMC.plots + plot_extrapolation2.plots + plot_fractions.plots,
		) 
	# override commandline (3):
	op.normalize = True

	fdata, fmc = getROOT.openFiles([op.data], [op.mc], op.verbose)
	op.bins = plotBase.guessBins(fdata, [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]) #binning must be after file open. plots do this later: if bins[0] == 0 bins.pop(0)
	plotDataMC.initDataMC(op,fdata,fmc)
	plotBase.plot(plotDataMC, op.plots, fdata, fmc, op)
	plotBase.plot(plot_fractions, op.plots, fdata, fmc, op)
	plotBase.plot(plot_extrapolation2, op.plots, fdata, fmc, op)

if __name__ == "__main__":
	plot()
