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
    corr = ["L1L2L3","L1L2L3Res"]
    alg = ["AK5PFJets","AK5PFJetsCHS"
               ]
    for a in alg:
        opt.algorithm = a
        for c in corr:
            opt.correction = c
            responseplot_mikko(files, opt, ['bal', 'mpf', 'mpf-notypeI'
                    ], drawextrapolation=False, binborders=True)
    opt.algorithm = al
    opt.correction = co

def ratio_mikko(files, opt):
    al = opt.algorithm
    co = opt.correction
    corr = ["L1L2L3", "L1L2L3Res"]
    alg = ["AK5PFJets", "AK5PFJetsCHS"
          ]
    for a in alg:
        opt.algorithm = a
        for c in corr:
            opt.correction = c
            ratioplot_mikko(files, opt, ['bal', 'mpf', 'mpf-notypeI'
                ], drawextrapolation=False, binborders=True)
    opt.algorithm = al
    opt.correction = co




def responseplot_mikko(files, opt, types, labels=None,
                 colors=["FireBrick", 'blue', 'green', 'red']*7,
                 markers=['o', '*', 's']*8,
                 over='zpt',
                 binborders=False,
                 drawextrapolation=False):
    
    if labels is None:
        labels = types

    et_strings = ["_eta00_13", "_eta00_08","_eta08_13", "_eta13_19", "_eta19_25", "_eta25_30", "_eta30_32", "_eta32_52"
                  ]
    cu_strings = ["_a10", "_a15", "_a20", "_a30"
                   ]
    print types, labels

    for et, et_str in zip(([""]+getroot.etastrings(opt.eta)),et_strings):
        for cu, cu_str in zip(getroot.cutstrings(opt.cut), cu_strings):
            for t, l, m, c, in zip(types, labels, markers, colors):
                extrapolation = False
                if t == 'RecoGen':
                    t = 'RecoToGen_bal'
                #elif len(t) > 3:
                #    extrapolation = t[3:]
                #    t = t[:3]
                print t
                if extrapolation in ['ex', 'data', 'mc', 'datamc']:        
                    extrapolation = 'data'
                if 'Gen' not in t:
                    rgraph = plotresponse.getresponse(t+'resp', over, opt, files[0], None, {'var': cu+"_"+et}, extrapolation)
                    plot = getroot.root2histo(rgraph)
                    print t

                    if t == 'bal':
                        t1 = 'PtBal'
                    elif t == 'mpf':
                        t1 = 'MPF'
                    elif t == 'mpf-notypeI':
                        t1 = 'MPF-notypeI'
                    string = "data_"+t1+"_"+opt.algorithm+"_"+cu_str+et_str+"_"+opt.correction
                    string = string.replace("AK5PFJetsCHS_", "CHS")
                    string = string.replace("_AK5PFJets_", "")
                    rgraph.SetTitle(string)
                    rgraph.SetName(string)  
  
                    f = ROOT.TFile("/home/dhaitz/git/CalibFW/rootfiles/"+string+".root", "RECREATE")
                    rgraph.Write()
                    f.Close()
                if extrapolation == 'data':
                    extrapolation = 'mc'
                rgraph = plotresponse.getresponse(t+'resp', over, opt, files[1], None, {'var': cu+"_"+et}, extrapolation)
                plot = getroot.root2histo(rgraph)

                if t == 'bal':
                    t = 'PtBal'
                elif t == 'mpf':
                    t = 'MPF'
                elif t == 'mpf-notypeI':
                    t = 'MPF-notypeI'
                string = "mc_"+str(t)+"_"+opt.algorithm+"_"+cu_str+et_str+"_"+opt.correction
                string = string.replace("AK5PFJetsCHS_", "CHS")
                string = string.replace("_AK5PFJets_", "")
                rgraph.SetTitle(string)
                rgraph.SetName(string)

                f = ROOT.TFile("/home/dhaitz/git/CalibFW/rootfiles/"+string+".root", "RECREATE")
                rgraph.Write()
                f.Close()

def ratioplot_mikko(files, opt, types, labels=None,
                 colors=["FireBrick", 'blue', 'green', 'red']*7,
                 markers=['o', '*', 's']*8,
                 over='zpt',
                 binborders=False,
                 drawextrapolation=False,
                 fit=True):
    #type: bal|mpf[ratio|seperate]
    if binborders:
        for x in opt.bins:
            pass #ax.axvline(x, color='gray')

    if labels is None:
        labels = types

    et_strings = ["_eta00_13", "_eta00_08","_eta08_13", "_eta13_19", "_eta19_25", "_eta25_30", "_eta30_32", "_eta32_52"
                       ]
    cu_strings = ["_a10", "_a15", "_a20", "_a30"
                   ]
    print et_strings
    print "labels ", labels
    print ([""]+getroot.etastrings(opt.eta))

    for et, et_str in zip(([""]+getroot.etastrings(opt.eta)),et_strings):
        for cu, cu_str in zip(getroot.cutstrings(opt.cut), cu_strings):
            for t, l, m, c in zip(types, labels, markers, colors):
                rgraph = plotresponse.getresponse(t[:3]+'resp', over, opt, files[0], files[1], {'var': cu+"_"+et})
                if fit:
                    line, err, chi2, ndf = getroot.fitline(rgraph)

                plot = getroot.root2histo(rgraph)
                if t == 'bal':
                    t = 'PtBal'
                elif t == 'mpf':
                    t = 'MPF'
                elif t == 'mpf_notypeI':
                    t = 'MPF_notypeI'
                string = "ratio_"+str(t)+"_"+opt.algorithm+"_"+cu_str+et_str+"_"+opt.correction
                string = string.replace("AK5PFJetsCHS_", "CHS")
                string = string.replace("_AK5PFJets_", "")
                rgraph.SetTitle(string)
                rgraph.SetName(string)

                f = ROOT.TFile("/home/dhaitz/git/CalibFW/rootfiles/"+string+".root", "RECREATE")
                rgraph.Write()
                f.Close()


            if over == 'jet1eta':
                pre = "abs_"
            else:
                pre = ""
