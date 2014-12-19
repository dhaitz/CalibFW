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
    if opt.filename is None:
        opt.filename = "plots_mikko.root"
    f = ROOT.TFile(opt.out + "/" + opt.filename, "RECREATE")
    mikko_oneset(files, opt, ['balresp', 'mpfresp', 'mpf-rawresp',
         'recogen'])
    f.Close()


def doPlot(resp, dtype, opt, cut, eta, cu_str, et_str, file1, file2=None):
    d = {
        'balresp': 'PtBal',
        'mpfresp': 'MPF',
        'mpf-rawresp': 'MPF-notypeI',
        'recogen': 'RecoToGen',
    }

    changes = {'allalpha': True,
               'alleta': True,
               'selection': ['(%s) && (%s)' % (cut, eta)]
               }
    for corr in ["L1L2L3", "L1L2L3Res"]:
        changes['correction'] = corr

        settings = plotbase.getSettings(opt, changes=changes, quantity="%s_zpt" % resp)

        rgraph = plotresponse.getresponse(resp, 'zpt', opt, settings, file1, file2)
        string = "_".join(filter(None, [dtype, d[resp], opt.algorithm.replace("AK5PFJets", ""), cu_str, et_str, corr]))
        rgraph.SetTitle(string)
        rgraph.SetName(string)
        rgraph.Write()
        rgraph.Delete()


def mikko_oneset(files, opt, types=['bal', 'mpf', 'mpf-raw']):
    et_strings = [
         "eta00_13", "eta00_08", "eta08_13", "eta13_19",
         "eta19_25", "eta25_30", "eta30_32", "eta32_52"
    ]
    a_strings = ["a10", "a15", "a20", "a30", "a40"]

    et_cuts = ["1"] + getroot.etacuts(opt.eta)
    a_cuts = getroot.alphacuts([0.1, 0.15, 0.2, 0.3, 0.4])

    for et, et_str in zip(et_cuts, et_strings):
        for a, a_str in zip(a_cuts, a_strings):
            for t in types:
                if 'gen' not in t:
                    doPlot(t, opt.labels[0], opt, a, et, a_str, et_str, files[0])
                    doPlot(t, "ratio", opt, a, et, a_str, et_str, files[0], files[1])
                doPlot(t, opt.labels[1], opt, a, et, a_str, et_str, files[1])

    print "Output written to:", opt.out + "/" + opt.filename
