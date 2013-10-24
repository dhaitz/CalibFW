import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile
import ROOT

import plotbase
import getroot
import plotresponse

"""This is the macro to produce Mikko's root files"""

plots = ['mikko_all']

def mikko_all(files, opt):
    al = opt.algorithm
    co = opt.correction
    corr = ["L1L2L3", "L1L2L3Res"]
    alg = ["AK5PFJets", "AK5PFJetsCHS"]

    f = ROOT.TFile(opt.out + "/plots_mikko.root", "RECREATE")
    for a in alg:
        opt.algorithm = a
        for c in corr:
            opt.correction = c
            mikko_oneset(files, opt, ['bal', 'mpf', 'mpf-raw'])
    f.Close()
    opt.algorithm = al
    opt.correction = co


def doPlot(resp, dtype, opt, cut, eta, cu_str, et_str, file1, file2=None):
    d = {
        'bal': 'PtBal',
        'mpf': 'MPF',
        'mpf-raw': 'MPF-notypeI',
        'RecoGen': 'RecoToGen_bal',
    }
    rgraph = plotresponse.getresponse(resp+'resp', 'zpt', opt, file1, file2, {'var': cut+"_"+eta})
    string = "_".join(filter(None, [dtype, d[resp], opt.algorithm.replace("AK5PFJets", ""), cu_str, et_str, opt.correction]))
    rgraph.SetTitle(string)
    rgraph.SetName(string)
    rgraph.Write()


def mikko_oneset(files, opt, types=['bal', 'mpf', 'mpf-raw']):
    et_strings = ["eta00_13", "eta00_08","eta08_13", "eta13_19", "eta19_25", "eta25_30", "eta30_32", "eta32_52"]
    cu_strings = ["a10", "a15", "a20", "a30"]


    for et, et_str in zip(([""]+getroot.etastrings(opt.eta)), et_strings):
        for cu, cu_str in zip(getroot.cutstrings(opt.cut), cu_strings):
            for t in types:
                if 'Gen' not in t:
                    doPlot(t, "data", opt, cu, et, cu_str, et_str, files[0])
                    doPlot(t, "ratio", opt, cu, et, cu_str, et_str, files[0], files[1])
                doPlot(t, "mc", opt, cu, et, cu_str, et_str, files[1])
    print "Output written to:", opt.out + "/plots_mikko.root"


