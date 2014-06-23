import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', rundepMC=True)
    conf["InputFiles"] = "/storage/a/berger/zpj/kappa5313_MC12_madgraph_rundep-2/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    conf['Jec'] = base.getPath() + "/data/jec/Winter14/Winter14_V1_MC"
    conf['PileupWeights'] = base.getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_kappa5313_MC12_madgraph_rundep-2.root"
    conf['checkKappa'] = "newtagging"
    conf['RC'] = True
    conf['VetoPileupJets'] = True
    #conf['checkArtus'] = "65a2c8c2"
    return conf

