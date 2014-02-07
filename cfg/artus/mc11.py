import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2011')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC11/*.root"
    conf['Jec'] = base.getPath() + "/data/jec/Summer13_V5_NEW_MOD_MC"
    conf['Tagged'] = False
    conf = base.expand(conf, ['all', 'zcuts'], algorithms=['AK5PFJetsL1L2L3'])
    conf['checkKappa'] = "47c52f"
    conf['checkArtus'] = "abb490"
    return conf

