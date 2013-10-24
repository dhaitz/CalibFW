import sys
import ArtusConfigBase as base

def get_template(algo="AK5PFJets"):
    conf = base.BaseConfig('data', '2012')
    conf["InputFiles"] = base.CreateFileList("/storage/8/dhaitz/Winter13ReReco_2/*.root", sys.argv)
    conf["OutputPath"] = "artus_data2012_tree"
    conf["JsonFile"] = base.GetBasePath() + "data/json/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"

    #algorithms = [algo, algo+"L1", algo+"L1L2L3", algo+"L1L2L3Res"]
    algorithms = [algo+"L1L2L3Res"]
    base.addCHS(algorithms)
    base_algorithms = [algo, algo+"CHS"]

    conf = base.ExpandConfig(algorithms, conf, expandptbins=False, alletaFolder=True,
             zcutsFolder=False, addResponse=False)

    # create various variations ...
    variations = []
    variations += [ base.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.35], onlyBasicQuantities=False, correction="") ]
    #variations += [ base.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
    #variations += [ base.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=False, onlyOnIncut=False) ]

    #base.AddCorrectionPlots(conf, base_algorithms, l3residual = True)
    #base.AddCutConsumer(conf, algorithms)
    #base.AddHltConsumer(conf, base_algorithms, ["HLT_Mu17_Mu8", "HLT_Mu13_Mu8", "HLT_DoubleMu7", "HLT_DoubleMu5"])

    for v in variations:
        conf["Pipelines"].update(v)

    #algo_list = [algo+"L1L2L3", algo+"CHSL1L2L3", algo+"L1L2L3Res", algo+"CHSL1L2L3Res"]

    #base.AddLumiConsumer(conf, algo_list, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=True)
    #base.AddQuantityPlots(conf, algorithms, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

    #base.Add2DHistograms(conf, algo_list, forIncut=True, forAllevents=True, forIncutVariations=False, forAlleventsVariations=True)
    #base.Add2DProfiles(conf, algo_list, forIncut=True, forAllevents=True, forIncutVariations=False, forAlleventsVariations=True)

    for p, pval in conf["Pipelines"].items():
        base.RemoveConsumer(pval, "quantities_all")
        base.AddConsumerNoConfig(pval, "tree")

    return conf


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""

    conf = get_template()
    base.Run(conf, sys.argv)
