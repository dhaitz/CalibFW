import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee', tagged=False)
    conf["InputFiles"] = "/storage/a/dhaitz/zee_mc/*.root"
    conf = base.expand(conf, ['all'])
    conf["PileupWeights"] = "/portal/ekpcms5/home/berger/excalibur/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_68_5mb_kappa539_MC12_madgraph_tags.root"
    return conf

