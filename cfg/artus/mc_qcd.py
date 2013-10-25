import ArtusConfigBase as base


def get_template():
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa534_MC12_background/kappa_QCD*.root"

    algorithms = ["AK5PFJetsL1L2L3"]
    base.addCHS(algorithms)
    base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

    conf = base.ExpandConfig(algorithms, conf, expandptbins=False, alletaFolder=True, zcutsFolder=True)
