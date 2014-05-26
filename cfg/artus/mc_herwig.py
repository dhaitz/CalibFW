import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_05_12_Herwig/*.root"
    #base.ApplyPUReweighting(conf, 'kappa539_MC12_herwig_190456-208686_8TeV_22Jan2013ReReco')
    #conf['PileupWeights'] = base.getPath() + 
    base.ApplySampleReweighting(conf)
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
