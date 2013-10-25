import ArtusConfigBase as base


def get_template(algo="AK5PFJets"):
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC12_herwig/*.root"
    base.ApplyPUReweighting(conf, 'kappa539_MC12_herwig_190456-208686_8TeV_22Jan2013ReReco')
    base.ApplySampleReweighting(conf)

    algorithms = [algo + "CHSL1L2L3"]
    base_algorithms = [algo, algo + "CHS"]

    conf = base.ExpandConfig(algorithms, conf, expandptbins=False, addResponse=False)
    conf = base.treeconfig(conf)
    return conf
