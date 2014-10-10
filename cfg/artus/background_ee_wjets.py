import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='ee')
    conf["InputFiles"] = base.setInputFiles(
        ekppath="/storage/9/dhaitz/skims/2014_07_29_wjets/*.root",
        nafpath="/nfs/dust/cms/user/dhaitz/skims/2014_07_29_ee-mm-wjets/*.root"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 57709905
    conf['XSection'] = 37509.8
    conf['electrons'] = 'correlectrons'
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
