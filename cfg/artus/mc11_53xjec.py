import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2011', tagged=False)
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC11/*.root"
    conf['Jec'] = base.GetBasePath() + "data/jec/Summer13_V5_MC"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
