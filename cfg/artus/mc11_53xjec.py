import ArtusConfigBase as base


def get_template(algo="AK5PFJets"):
    conf = base.BaseConfig('mc', '2011')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC11/*.root"
    conf['Tagged'] = False
    conf['Jec'] = base.GetBasePath() + "data/jec/Summer13_V5_MC"

    algorithms = [algo + "CHSL1L2L3"]
    base_algorithms = [algo, algo + "CHS"]

    conf = base.ExpandConfig(algorithms, conf, expandptbins=False, addResponse=False)
    conf = base.treeconfig(conf)
    return conf
