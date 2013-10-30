import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2011')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC11/*.root"
    conf['Tagged'] = False
    conf = base.expand(conf, ['all', 'zcuts'])
    #base.pipelinediff(conf)
    return conf

