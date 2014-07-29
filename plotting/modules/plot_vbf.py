#!/usr/bin/env python
# -*- coding: utf-8 -*-
import plotbase
import getroot

import plot1d
import plotfractions
import plotresponse
import plot_resolution
import plot_mikko
import plot2d

def plot(cluster=False):

    op = plotbase.options(
        algorithm="AK5PFJetsCHS",
        correction="L1L2L3Res",
        out="out/vbf/",
        labels=["data", "MC", "MC2"],
        colors=['black', '#CBDBF9','#800000', 'blue', '#00FFFF'],
        style=["o", "f", "-", "-", "-"],

        lumi=14712.0,
        energy=8,
        plots=plotresponse.plots
            + plotfractions.plots
            + plot2d.plots
            + plot1d.plots
            #+ plot_resolution.plots
            #+ plot_mikko.plots
        )
    module_list = [plotresponse, plotfractions, plot2d, plot1d, plot_resolution, plot_mikko]

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
