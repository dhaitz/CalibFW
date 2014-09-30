import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012', tagged=False)
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_Data12/*.root"
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
