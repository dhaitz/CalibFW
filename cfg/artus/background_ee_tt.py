import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee', rundepMC=True)
    conf["InputFiles"] = base.setInputFiles(
        ekppath="/storage/9/dhaitz/skims/2014_08_04_ee_mc_TTJets/*.root",
        nafpath='/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_08_04_ee_mc_TTJets/',
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 21675970
    conf['XSection'] = 245.8  # NNLO, https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
    conf['electrons'] = 'correlectrons'
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
