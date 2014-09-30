import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2011')
    conf["InputFiles"] = "/storage/a/berger/zpj/kappa5313_Data11_Oct12ReReco/*.root"
    conf['Jec'] = base.getPath() + "/data/jec/2011Legacy_NEW/Legacy11_V1_DATA"
    conf = base.expand(conf, ['all', 'zcuts', 'incut'], algorithms=['AK5PFJetsL1L2L3', 'AK5PFJetsL1L2L3Res'])
    return conf
