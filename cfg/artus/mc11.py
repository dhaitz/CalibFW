import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2011')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC11/*.root"
    conf['Tagged'] = False
    conf = base.expand(conf, ['all', 'zcuts'])
    conf['Jec'] = base.getPath() + "/data/jec/Summer13_V5_NEW_MOD_MC"
    conf['checkKappa'] = "47c52f"
    conf['checkArtus'] = "abb490"
    return conf

