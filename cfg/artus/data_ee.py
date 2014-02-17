import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012', analysis='zee')
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_02_14_zee_data/*.root"
    #conf['GlobalProducer'].remove('hlt_selector')
    #conf['Pipelines']['default']['Filter'].remove('hlt')
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
