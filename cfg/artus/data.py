import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012')
    #conf["InputFiles"] = "/storage/8/berger/zpj/kappa539_Data12_tags/*.root"
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_03_25_data12/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    conf['Jec'] = base.getPath() + "/data/jec/Winter14/Winter14_V1_DATA"
    conf['RC'] = True
    return conf
