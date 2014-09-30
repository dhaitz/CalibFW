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
    conf['XSection'] = 26.75
    # ttbar(NNLO ) * BR(W->l nu)^2
    # https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
    # W decay leptonic BR = 0.324
    conf['electrons'] = 'correlectrons'
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
