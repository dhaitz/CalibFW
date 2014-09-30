import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = "/home/dhaitz/CMSSW_5_3_9/skim_mc.root"
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf

