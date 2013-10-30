import ArtusConfigBase as base


def get_template(algo="AK5PFJets"):
    conf = base.BaseConfig('data', '2011')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_Data11/*.root"
    conf['Jec'] = base.GetBasePath() + "data/jec/Summer13_V5_DATA"

    algorithms = [algo + "CHSL1L2L3", algo + "CHSL1L2L3Res"]
    base_algorithms = [algo, algo + "CHS"]

    conf = base.ExpandConfig(algorithms, conf, expandptbins=False, addResponse=False)
    conf = base.treeconfig(conf)
    return conf
