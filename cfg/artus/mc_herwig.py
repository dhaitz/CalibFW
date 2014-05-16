import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC12_herwig/*.root"
    #base.ApplyPUReweighting(conf, 'kappa539_MC12_herwig_190456-208686_8TeV_22Jan2013ReReco')
    base.ApplySampleReweighting(conf)
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
