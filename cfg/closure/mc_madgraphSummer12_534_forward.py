import sys
import ClosureConfigBase as cbase

def get_template(algo="AK5PFJets"):
    conf = cbase.BaseConfig('mc', '2012')
    conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa534_MC12_madgraph/*.root", sys.argv)
    conf["OutputPath"] = "closure_madgraphSummer12_534"

    algorithms = [#algo, algo+"L1",
         algo+"L1L2L3"]
    cbase.addCHS(algorithms)
    base_algorithms = [algo, algo+"CHS"]

    conf = cbase.ExpandConfig(algorithms, conf, expandptbins=True, alletaFolder=True, zcutsFolder=True)

    # create various variations ...
    variations = []
    variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.3, 0.35], onlyBasicQuantities=False, correction="") ]
    #variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
    variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=True, onlyOnIncut=False) ]

    #cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
    #cbase.AddCutConsumer(conf, algorithms)

    for v in variations:
        conf["Pipelines"].update(v)

    algo_list = [algo+"L1L2L3", algo+"CHSL1L2L3"]

    cbase.AddQuantityPlots(conf, algo_list, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

    #cbase.Add2DHistograms(conf, algo_list, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)
    #cbase.Add2DProfiles(conf, algo_list, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

    return conf


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""

    conf = get_template()
    cbase.Run(conf, sys.argv)
