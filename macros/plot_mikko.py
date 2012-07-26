import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile
import ROOT

import plotbase
import getroot
import plotresponse

"""This is the macro to produce Mikko's root files"""

plots = ['response_mikko','ratio_mikko']

def response_mikko(files, opt):
    al = opt.algorithm
    co = opt.correction
    corr = ["L1L2L3"]
    alg = ["AK5PFJets","AK5PFJetsCHS"
               ]
    for a in alg:
        opt.algorithm = a
        for c in corr:
            opt.correction = c
            responseplot_mikko(files, opt, ['bal', 'mpf'
                    ], drawextrapolation=False, binborders=True)
    opt.algorithm = al
    opt.correction = co

def ratio_mikko(files, opt):
    al = opt.algorithm
    co = opt.correction
    corr = ["L1L2L3"]
    alg = ["AK5PFJets", "AK5PFJetsCHS"
          ]
    for a in alg:
        opt.algorithm = a
        for c in corr:
            opt.correction = c
            ratioplot_mikko(files, opt, ['bal', 'mpf'
                ], drawextrapolation=False, binborders=True)
    opt.algorithm = al
    opt.correction = co




def responseplot_mikko(files, opt, types, labels=None,
                 colors=["FireBrick", 'blue', 'green', 'red']*7,
                 markers=['o', '*', 's']*8,
                 over='z_pt',
                 binborders=False,
                 drawextrapolation=False):
    
    fig, ax = plotbase.newplot()
    if labels is None:
        labels = types

    et_strings = ["_eta00_13", "_eta00_08","_eta08_13", "_eta13_19", "_eta19_25", "_eta25_30", "_eta30_52"
                  ]
    cu_strings = ["_a10", "_a15", "_a20", "_a30"
                   ]

    for et, et_str in zip(([""]+getroot.etastrings(opt.eta)),et_strings):
        for cu, cu_str in zip(getroot.cutstrings(opt.cut), cu_strings):
            for t, l, m, c, in zip(types, labels, markers, colors):
                extrapolation = False
                if t == 'RecoGen':
                    t = 'RecoToGen_bal'
                elif len(t) > 3:
                    extrapolation = t[3:]
                    t = t[:3]
                if extrapolation in ['ex', 'data', 'mc', 'datamc']:        
                    extrapolation = 'data'
                if 'Gen' not in t:
                    rgraph = plotresponse.getresponse(t+'resp', over, opt, files[0], None, {'var': cu+"_"+et}, extrapolation)
                    plot = getroot.root2histo(rgraph)

                    if t == 'bal':
                        t1 = 'PtBal'
                    elif t == 'mpf':
                        t1 = 'MPF'
                    string = "data_"+t1+"_"+opt.algorithm+"_"+cu_str+et_str
                    string = string.replace("AK5PFJetsCHS_", "CHS")
                    string = string.replace("_AK5PFJets_", "")
                    rgraph.SetTitle(string)
                    rgraph.SetName(string)  
  
                    f = ROOT.TFile("/home/dhaitz/git/CalibFW/rootfiles/"+string+".root", "RECREATE")
                    rgraph.Write()
                    f.Close()
                    #ax.errorbar(plot.x, plot.y, plot.yerr, color='black', fmt=m, label=l+' (data)')
                if extrapolation == 'data':
                    extrapolation = 'mc'
                rgraph = plotresponse.getresponse(t+'resp', over, opt, files[1], None, {'var': cu+"_"+et}, extrapolation)
                plot = getroot.root2histo(rgraph)

                if t == 'bal':
                    t = 'PtBal'
                elif t == 'mpf':
                    t = 'MPF'
                string = "mc_"+str(t)+"_"+opt.algorithm+"_"+cu_str+et_str
                string = string.replace("AK5PFJetsCHS_", "CHS")
                string = string.replace("_AK5PFJets_", "")
                rgraph.SetTitle(string)
                rgraph.SetName(string)

                f = ROOT.TFile("/home/dhaitz/git/CalibFW/rootfiles/"+string+".root", "RECREATE")
                rgraph.Write()
                f.Close()
                #ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l+' (MC)')

def ratioplot_mikko(files, opt, types, labels=None,
                 colors=["FireBrick", 'blue', 'green', 'red']*7,
                 markers=['o', '*', 's']*8,
                 over='z_pt',
                 binborders=False,
                 drawextrapolation=False,
                 fit=True):
    #type: bal|mpf[ratio|seperate]
    
    fig, ax = plotbase.newplot()
    #if labels is None:
    #    labels = [labelformat(t) for t in types]
    ax.axhline(1.0, color="black", linestyle='--')
    if binborders:
        for x in opt.bins:
            pass #ax.axvline(x, color='gray')

    if labels is None:
        labels = types

    et_strings = ["_eta00_13", "_eta00_08","_eta08_13", "_eta13_19", "_eta19_25", "_eta25_30", "_eta30_52"
                       ]
    cu_strings = ["_a10", "_a15", "_a20", "_a30"
                   ]
    print et_strings
    print "labels ", labels
    print ([""]+getroot.etastrings(opt.eta))

    for et, et_str in zip(([""]+getroot.etastrings(opt.eta)),et_strings):
        for cu, cu_str in zip(getroot.cutstrings(opt.cut), cu_strings):
            for t, l, m, c in zip(types, labels, markers, colors):
                rgraph = plotresponse.getresponse(t[:3]+'resp', over, opt, files[0], files[1], {'var': cu+"_"+et}, extrapol=t[3:])
                if fit:
                    line, err, chi2, ndf = getroot.fitline(rgraph)
                    ax.text(0.95, 0.65+0.07*colors.index(c), r"$R = {0:.3f} \pm {1:.3f}$ ".format(line, err),
                        va='bottom', ha='right', color=c, transform=ax.transAxes, fontsize=16)
                    ax.text(0.95, 0.2+0.07*colors.index(c), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
                        va='bottom', ha='right', color=c, transform=ax.transAxes, fontsize=16)
                    ax.axhline(line, color=c)
                    ax.axhspan(line-err, line+err, color=c, alpha=0.2)

                plot = getroot.root2histo(rgraph)
                if t == 'bal':
                    t = 'PtBal'
                elif t == 'mpf':
                    t = 'MPF'
                string = "ratio_"+str(t)+"_"+opt.algorithm+"_"+cu_str+et_str
                string = string.replace("AK5PFJetsCHS_", "CHS")
                string = string.replace("_AK5PFJets_", "")
                rgraph.SetTitle(string)
                rgraph.SetName(string)

                f = ROOT.TFile("/home/dhaitz/git/CalibFW/rootfiles/"+string+".root", "RECREATE")
                rgraph.Write()
                f.Close()

                #ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l)

                # format plot
            if over == 'jet1_eta':
                pre = "abs_"
            else:
                pre = ""
