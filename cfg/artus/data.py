import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012')
    conf["InputFiles"] = "/storage/8/berger/zpj/kappa539_Data12_tags/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    conf['Jec'] = base.getPath() + "/data/jec/Spring14_V1_DATA"
    conf['checkKappa'] = "master"
    return conf
