#!/usr/bin/env python
# -*- coding: utf-8 -*-
import argparse
import ROOT
import subprocess
import json
import os
"""This is a tool for pile-up reweighting.

   It requires: python, ROOT, CMSSW 5.2 (pileupCalc).
   It calculates the pu distribution in data with the official tool.
   Then it calculates the pu distribution from a skim.
   From these distributions it calculates the weights and saves them to a json
   file. Data and MC distributions are saved to root files.
   Pleas use --help or read the options function for further information.
"""
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError


def main():
    op = options()

    data, datakey = getDataDistribution(op.datainput, op.inputLumiJSON, op.minBiasXsec, op.numPileupBins, op.data_histo, op.dataoutput, op.verbose)
    print "Data:", data
    mc, mckey = getMCDistribution(op.mcinput, op.mc_histo, op.mcoutput, op.verbose)
    print "MC:", mc
    weights = calcWeights(data, mc, op.verbose, not op.no_warning)
    printWeights(weights, op.breaklines)
    if op.check:
        print("Normalization: before ({0:.5f}) and after ({1:.5f}) "
              "reweighting.".format(sum(mc),
                  sum(map(lambda a, b: a * b, mc, weights))))
    if not op.key:
        op.key = mckey[7:-5]+"_"+datakey[7:-5]
    addWeightsToFile(op.key, weights, op.mcXsec, op.output)


def calcWeights(dataDistribution, mcDistribution, verbose=False, warn=True):
    result = []
    print "____________________________________________"
    print "Weight calculation ..."
    # check length and add zeros
    ldiff = len(dataDistribution) - len(mcDistribution)
    dataDistribution += [0.0] * (-ldiff)
    mcDistribution += [0.0] * (ldiff)
    if verbose:
        print "Weighting factors are calculated for up to",
        print len(dataDistribution) - 1, "pile-up interactions from:"
        print "Data:", dataDistribution
        print "MC:  ", mcDistribution
    assert len(dataDistribution) == len(mcDistribution)
    # divide
    for npu in range(len(dataDistribution)):
        if mcDistribution[npu] == 0:
            result.append(0.0)
            if warn:
                print "WARNING: There are no MC events with", npu,
                print "pile-up interactions!"
        else:
            result.append(dataDistribution[npu] / mcDistribution[npu])
    assert len(result) == len(mcDistribution)
    # normalize (i.e. sum_i result[i]*mcDist[i] == 1)
    result = [a / sum(dataDistribution) for a in result]
    return result


def printWeights(weights, nice=False):
    s = "[" + ", ".join("%1.9f" % w for w in weights) + "]"
    if nice:
        s = "conf[\"RecovertWeight\"] = " + s
        while s.rfind(",", 0, 72) > 0:
            i = max(s.rfind(",", 0, 72), s.rfind("]", 0, 72)) + 1
            print "        " + s[0:i]
            s = " " + s[i:]
    else:
        print s


def getDataDistribution(files, lumijson, xsec=69.3, numpu=50, histo="pileup", outfile=None,  verbose=False):
    """Get the true Data PU from the official PU-json

       The "true" (or "observed") PU in data is estimated using this method:
       https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
       1. pileupCalc is run (if json is given)
       2. the distribution is read from a rootfile
    """
    if lumijson is None and files[-5:] == ".root":
        print "Data pile-up distribution from", files
        outfile = files
    elif files[-4:] == ".txt":
        print "Data pile-up distribution is estimated from official files:"
        try:
            fnull = open(os.devnull, 'w')
            subprocess.call("pileupCalc.py", stdout=fnull, stderr=fnull)
            fnull.close()
        except:
            print "pileupCalc.py is not availabe. Probably CMSSW 5.2 is not sourced."
            exit(1)
        if not outfile:
            # take the core part of the json file name
            outfile = files.split('/')[-1]
            if "_v" in outfile:
                outfile = "pileup_" + outfile[5:-25] + outfile[-7:-4] + ".root"
            else:
                outfile = "pileup_" + outfile[5:-22] + ".root"
        cmd=["pileupCalc.py", "-i", files, "--inputLumiJSON", lumijson, "--calcMode", "true", "--minBiasXsec", str(int(xsec*1000)), "--maxPileupBin", str(numpu+1), "--numPileupBins", str(numpu+1), outfile]
        print " ".join([str(s) for s in cmd])
        subprocess.call(cmd)

    result = getDistributionFromFile(outfile, histo)
    for i in range(len(result)):
        if result[i] > 0.0:
            mx=i
    result = result[0:mx+1]
    print "INFO: There are 0.."+str(mx+1), "vertices calculated in Data."
    return result, outfile


def getMCDistribution(source, histo="pu", outfile=None, verbose=False):
    """Get the true MC PU from the MC files

       The "true" PU in MC is determined from either:
       https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
       1. the kappa skim (source="/path/to/skim/*.root")
       2. a root file containing the distribution (source="pilup.root")
       3. the key of a precalculated MC sample (source="madgraphFall11")
    """
    result = []
    # 3 cases for source: skim location, .root file, key from dict below
    if type(source) == list and len(source) >= 1 and "kappa_" in source[0]:
        result, outfile = getDistributionFromSkim(source)
    elif len(source) == 1 and source[0][-5:] == ".root":
        print "____________________________________________"
        print "MC pile-up distribution from", source[0]
        result = getDistributionFromFile(source[0], histo)
        outfile = outfile or source[0]
    else:
        print "This MC distribution could not be found!"
        assert False
    mx=0
    for i in range(len(result)):
        if result[i] > 0.0:
            mx=i
    result = result[0:mx+1]
    s = sum(result)
    print "INFO: There are 0.."+str(mx+1), "vertices calculated in MC."
    result = [w / s for w in result]
    return result, outfile


def getDistributionFromSkim(filelist, outputfile=None, numpu=50, save=True, step=1.0, histo="pileup"):
    print "____________________________________________"
    print "MC pile-up distribution from skim ("+str(len(filelist))+" file(s)):", filelist[0], "etc."
    chain = ROOT.TChain("Events");
    for f in filelist:
        chain.Add(f)

    npu = ROOT.TH1D(histo, "Number of Pile-Up", int(step*numpu)+1, 0.0, numpu+1)
    chain.Draw("KEventMetadata.numPUInteractionsTruth >> "+histo)
    result = [npu.GetBinContent(i) for i in range(1, npu.GetNbinsX())]
    print result
    if not outputfile:
        outputfile = "pileup_" + filelist[0].split("/")[-2] + ".root"
    if save:
        print "Distribution is written to:", outputfile
        f = ROOT.TFile.Open(outputfile, "RECREATE")
        npu.Write()
        f.Write()
        f.Close()
    return result, outputfile


def getDistributionFromFile(filename, histoname="npu"):
    """Convention according to official files: bin 1 has LowEdge at 0"""
    rootfile = ROOT.TFile(filename)
    if not rootfile.IsOpen():
        print "The file", filename, "could not be opened."
        exit(0)
    histo = rootfile.Get(histoname)
    if not histo:
        print "The histogram", histoname, "could not be found in", filename + "."
        exit(0)
    return [histo.GetBinContent(i) for i in range(1, ROOT.TH1D(histo).GetNbinsX())]


def addWeightsToFile(key, weights, xsec, filename, warnOnOverwrite=False):
    assert type(key) == str
    assert type(weights) == list
    d = os.path.dirname(filename)
    if not os.path.exists(d):
        print "Directory", d, "is created"
        os.makedirs(d)
    try:
        f = open(filename, 'r')
        dic = json.load(f)
        f.close()
    except:
        print filename, "does not exist and will be created."
        dic = {}
    if warnOnOverwrite and key in dic:
        print "There are already weights for", key+". Do you wan to overwrite these [Y/n]:"
        print dic[key]
        print "with"
        print {"weights": weights, "xsection": xsec}
        if "n" not in raw_input():
            dic[key] = {"weights": weights, "xsection": xsec, "step": 1}
    else:
        dic[key] = {"weights": weights, "xsection": xsec, "step": 1}
    f = open(filename, 'w')
    json.dump(dic, f, sort_keys=True, indent=2)
    f.close()
    print "Weights written to", filename, "as", key


def options():
    parser = argparse.ArgumentParser(
        description="%(prog)s calculates the weights for MC reweighting "
            "according to the number of pile-up interactions. Use cases: "
            "%(prog)s /path/to/skim/*.root Cert_JSON.txt pileup_JSON.txt or"
            "%(prog)s mcdist.root datadist.root")
    parser.add_argument('datainput', metavar='datainput', type=str,
        help="root file containing the estimated true number of pile-up "
            "interactions in data or the used json file and the official "
            "pile-up json. The name of the contained histogram is "
            "specified with -D.")
    parser.add_argument('mcinput', metavar='mcinput', type=str, nargs='+',
        help="either a skim location or a rootfile with the MC distribution "
            "of pile-up. The name of the contained histogram is "
            "specified with -M.")

    parser.add_argument('-M', '--mc-histo', type=str, default="pileup",
        help="histogram name of the pile-up distribution in the MC file")
    parser.add_argument('-D', '--data-histo', type=str, default="pileup",
        help="histogram name of the pile-up distribution in the data file")
    parser.add_argument('-d', '--dataoutput', type=str, default="",
        help="Output file for the data distribution. Determined automatically "
            "if not specified.")
    parser.add_argument('-m', '--mcoutput', type=str, default="",
        help="Output file for the MC distribution. Determined automatically "
            "if not specified.")
    parser.add_argument('-o', '--output', type=str, default="data/pileup/puweights.json",
        help="Output file for the calculated weights.")
    parser.add_argument('-k', '--key', type=str, default=None,
        help="Name for this set of weights in the output file. Determined "
            "automatically if not specified.")

    parser.add_argument('-l', '--inputLumiJSON', type=str, default=None,
        help="Input Lumi JSON for pileupCalc.")
    parser.add_argument('-x', '--minBiasXsec', type=float, default=69.3,
        help="Minimum bias cross section in mb (NB: pileupCalc takes µb!)")
    parser.add_argument('-n', '--numPileupBins', type=int, default=50,
        help="Maximum number of pile-up bins (default: 50).")
    parser.add_argument('-X', '--mcXsec', type=float, default=1614.0,
        help="Cross section of the MC sample to be saved alongside the weights.")

    parser.add_argument('-v', '--verbose', action='store_true',
        help="verbosity")
    parser.add_argument('-b', '--breaklines', action='store_true',
        help="linebreak the output")
    parser.add_argument('-c', '--check', action='store_true',
        help="check whether the weights are correctly normalized. If no MC "
            "events are availabe for some numbers of pile-up interactions, "
            "the distribution can not be correctly normalized and the weights "
            "do not exactly add up to unity.")
    parser.add_argument('-q', '--no-warning', action='store_true',
        help="Do not print warnings if the Monte Carlo sample does not "
            "contain events for all numbers of pile-up interactions.")
    return parser.parse_args()


if __name__ == "__main__":
    main()
