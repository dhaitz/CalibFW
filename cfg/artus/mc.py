import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC12_madgraph_tags/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    #base.pipelinediff(conf)
    return conf

