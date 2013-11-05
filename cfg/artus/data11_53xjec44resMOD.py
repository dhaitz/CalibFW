import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2011')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_Data11/*.root"
    conf['Jec'] = base.getPath() + "/data/jec/Summer13_V5_NEW_MOD_DATA"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
