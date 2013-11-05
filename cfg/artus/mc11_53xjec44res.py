import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2011', tagged=False)
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC11/*.root"
    conf['Jec'] = base.getPath() + "/data/jec/Summer13_V5_NEW_MC"
    conf = base.expand(conf, ['all', 'zcuts'])
    conf['EnableLumiReweighting'] = False
    return conf
