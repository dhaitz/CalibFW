# coding: utf-8
"""This is only a template for quick and dirty plots
"""
import numpy
import getroot
import plotbase
import plotresponse
import copy

def varrow(ax0, x, y1, y2, **kwargs):
    ax0.arrow(x, y1, 0, y2 - y1, head_width=0.12, head_length=0.002, length_includes_head=True, **kwargs)

def completeHisto(histo, xerr=0.4, yerr=0.0, shift=0.0):
    """Make incomplete histogram usable
    
    Fill x, xerr and yerr of a histogram with default values, in case these are
    not specified.
    """
    if len(histo.xerr) == 0:
        histo.x = [i+shift for i in range(len(histo))]
    if len(histo.xerr) == 0:
        histo.xerr = [xerr]*len(histo)
    if len(histo.yerr) == 0:
        histo.yerr = [yerr]*len(histo)

def fastplot(objectname, filenames, op=plotbase.options()):
    """
    
    data file is the first one (can be ignored with "nodata")
    the rest are MC files
    """
    ch_ext = {'var': "var_CutSecondLeadingToZPt_0_3"}
    ch_raw = {}
    quadratic = False
    if len(filenames) < 2:
        print "I need at least 2 files (1 data, 1 mc)!"
        exit(0)
    files = [getroot.openfile(f, op.verbose) if f != "nodata" else None for f in filenames ]
    bal = {}
    err = {}
    #print files
    
    for r in ['zresp', 'parton', 'balparton', 'genbal-toparton', 'genbal-tobalparton',
            'genbal', 'recogen', 'balresp', 'mpfresp', 'muresp', 'genmpf']:
        obj = getroot.getobject("NoBinning_incut/" + r + "_AK5PFJetsCHSL1L2L3", files[-1], ch_raw)
        bal[r] = obj.GetMean()
        err[r] = obj.GetMeanError()
    #print bal
        
    # fill plots
    mresp = getroot.Histo()  # MC response
    mresp.y = [
        1.0/bal['muresp'],
        1.0,
        1.0/bal['zresp'],
        bal['genbal']/bal['zresp'],
        bal['genbal']*bal['recogen']/bal['zresp'],
        bal['mpfresp'],
        #bal['genmpf'],
    ]
    mresp.yerr = [
        1.0/bal['muresp']/bal['muresp']*err['muresp'],
        0.0,
        1.0/bal['zresp']/bal['zresp']*err['zresp'],
        err['genbal']/bal['zresp'],
        err['genbal']*bal['recogen']/bal['zresp'],
        err['mpfresp'],
        #err['genmpf'],
    ]
    completeHisto(mresp)
    
    mextr = getroot.Histo()  # MC extrapolated
    mextr.y = [0]*3 + [
        plotresponse.getextrapolated('genbalance', files[-1], ch_ext)[0],
        plotresponse.getextrapolated('ptbalance', files[-1], ch_ext)[0],
        plotresponse.getextrapolated('mpf', files[-1],ch_ext)[0],
    ]
    completeHisto(mextr)
    
    if files[0]:
        dresp = getroot.Histo()  # data response
        dresp.y = [
            0, 1, 0, 0,
            getroot.getobjectfromnick('balresp', files[0], ch_raw, quadratic).GetMean(),
            getroot.getobjectfromnick('mpfresp', files[0], ch_raw, quadratic).GetMean(),
        ]
        
        completeHisto(dresp, shift=-0.07)

        dextr = getroot.Histo()  # data extrapolated
        dextr.y = [
            0, 0, 0, 0,
            plotresponse.getextrapolated('ptbalance', files[0], ch_ext, quadratic)[0],
            plotresponse.getextrapolated('mpf', files[0], ch_ext, quadratic)[0],
        ]
        completeHisto(dextr, shift=-0.07)
    else:
        print "No data file."
        dresp = dextr = None

    # create the plot
    fig, ax = plotbase.newPlot()
    ax.axhline(1.00, color = '#CCCCCC', zorder = -5)
    ax.axvline(0.52, color = '#000000', zorder = -5)
    ax.axvline(0.48, color = '#000000', zorder = -5)
    ax.axvline(4.50, color = '#AAAAAA', zorder = -5)

    # lines
    for p, c, l in zip([mresp, mextr, dresp, dextr], ['red', 'green', 'grey', 'black', 'green', 'blue', 'black' , 'black', 'purple'], ["MC Response", "MC Extrapolated", "Data Response", "Data Extrapolated"]):
        if p is None:
            continue
        if len(p.x) != len(p.y):
            print p.x, p.y
            print "Different"
            exit(0)

        #print p.xerr
        #print p.y
        ax.errorbar(p.x, p.y, p.yerr, xerr=p.xerr, drawstyle=None, color=c, fmt='o', capsize=0, label=l)

    # Extrapolation arrows
    for i in range(len(mextr)):
        if mextr.y[i] > 0:
            varrow(ax, mresp.x[i], mresp.y[i], mextr.y[i], color = 'orange')
            ax.text(mresp.x[i]-0.08, 0.5*(mresp.y[i]+mextr.y[i]),
                 "Extrapolation" if abs(mresp.y[i]-mextr.y[i]) > 0.015 else "Extr."
                                 if abs(mresp.y[i]-mextr.y[i]) > 0.005 else "",
                va='center', ha='right',rotation=90, fontsize=9)

    for i in range(len(dextr)):
        if dextr.y[i] > 0:
            varrow(ax, dresp.x[i], dresp.y[i], dextr.y[i], color = 'orange')


    # labels
    plotbase.labels(ax, op, legloc='lower left', frame=True)
    ax2 = ax.twinx()
    ax2.set_ylabel("Response to reconstructed Z")

    ax.set_xlabel("Balanced object")
    ax.set_ylim(0.95, 1.04)
    ax2.set_ylim(0.95, 1.04)
    ax.set_xticklabels([r"", r"gen $\mu$", r"reco Z", r"gen Z", r"GenJet", r"PFJet", r"MPF"]) #, r"gen MPF"])
    ax.set_ylabel("Response to reconstructed muon")
    plotbase.Save(fig, "balances", op)


    
    
fastplot("NoBinning_allevents/jet2_pt_AK5PFJetsCHSL1L2L3", ["/storage/6/berger/closure/work/data_2012_534/out/closure.root", "/storage/6/berger/closure/work/mc_madgraphSummer12_534/out/closure.root"])
