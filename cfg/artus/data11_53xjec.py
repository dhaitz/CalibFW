import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2011')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_Data11/*.root"
    conf['Jec'] = base.GetBasePath() + "data/jec/Summer13_V5_DATA/Summer13_V5_DATA"
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    #base.pipelinediff(conf)
    return conf
