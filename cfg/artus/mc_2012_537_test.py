import sys
import ArtusConfigBase as base


def get_template(algo="AK5PFJets"):
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = base.CreateFileList("/storage/6/berger/zpj/kappa534_MC12_madgraph/*.root", sys.argv)
    conf["OutputPath"] = "artus_mc_tree"

    algorithms = [algo + "CHSL1L2L3"]
    #base.addCHS(algorithms)
    base_algorithms = [algo, algo + "CHS"]

    conf = base.ExpandConfig(algorithms, conf, expandptbins=False, alletaFolder=True,
                              zcutsFolder=False, addResponse=False, nocutsFolder=True)

    # create various variations ...
    variations = []
    variations += [base.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.35], onlyBasicQuantities=False, correction="")]
    """variations += [ base.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
    variations += [ base.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=False, onlyOnIncut=False) ]

    base.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
    base.AddCutConsumer(conf, algorithms)"""

    for v in variations:
        conf["Pipelines"].update(v)

    """algo_list = [algo+"L1L2L3", algo+"CHSL1L2L3"]

    base.AddQuantityPlots(conf, algorithms, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

    base.Add2DHistograms(conf, algo_list, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)
    base.Add2DProfiles(conf, algo_list, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)"""

    for p, pval in conf["Pipelines"].items():
        base.RemoveConsumer(pval, "quantities_all")
        base.AddConsumerNoConfig(pval, "tree")
    return conf


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""

    conf = get_template()
    base.Run(conf, sys.argv)
