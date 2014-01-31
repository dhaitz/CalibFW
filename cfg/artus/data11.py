import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2011')
    conf["InputFiles"] = "/storage/a/berger/zpj/kappa5313_Data11_Oct12ReReco/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
