#!/usr/bin/env python
# -*- coding: utf-8 -*-
import plotbase
import getroot

import plotdatamc
import plotfractions
import plotresponse
import plot_resolution


def plot():
    op = plotbase.options(
        files=[
            "/home/berger/CalibFW/work/data_2012_vbf/out/closure_data_2012_vbf.root",
            "/home/berger/CalibFW/work/mc_madgraphSummer12_vbf/out/closure_mc_madgraphSummer12_vbf.root",
        ],
        algorithm="AK5PFJetsCHS",
        correction="L1L2L3Res",
        labels=["2012 data", "Madgraph"],
        colors=['black', '#19D175'],
        style=["o","f"],
        lumi=9882.0,
        author="Joram Berger",
        date='today',
        out="out_vbf",
        plots=plotdatamc.plots +
              plotresponse.plots +
              plotfractions.plots #+
              #plot_resolution.plots
        )
    module_list = [plotdatamc, plotresponse, plot_resolution, plotfractions]

    op.normalize = True

    print "Using Data file " + op.files[0]
    print "Using MC file " + op.files[1]

    files = [getroot.openfile(f, op.verbose) for f in op.files]
    op.bins = getroot.getbins(files[0],
            [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000])

    plotbase.plot(module_list, op.plots, files, op)


if __name__ == "__main__":
    plot()
