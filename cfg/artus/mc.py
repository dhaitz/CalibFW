import ArtusConfigBase as base


def config():
    cfg = base.BaseConfig('mc', '2012', rundepMC=True)
    cfg["InputFiles"] = base.setInputFiles(
        ekppath="/storage/9/dhaitz/skims/2014_07_18_ee-mm-mcRD/*.root",
        nafpath="/nfs/dust/cms/user/dhaitz/skims/2014_07_18_ee-mm-mcRD/*.root"
    )
    cfg["InputFiles"] = "/storage/a/dhaitz/skims/2014_07_18_mc_ee_mm/*.root"
    cfg = base.expand(cfg, ['all', 'zcuts', 'incut'])
    cfg['Jec'] = base.getPath() + "/data/jec/Winter14/Winter14_V1_MC"
    cfg['PileupWeights'] = base.getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_kappa5313_MC12_madgraph_rundep-2.root"
    cfg['RC'] = True
    cfg['EnableLumiReweighting'] = True
    cfg['EnableTriggerReweighting'] = True
    cfg['NEvents'] = 30459503
    cfg['XSection'] = 3503.71
    return cfg
