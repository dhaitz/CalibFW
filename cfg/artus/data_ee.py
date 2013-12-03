import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012', analysis='zee', tagged=False)
    conf["InputFiles"] = "/storage/a/dhaitz/zee/*.root"
    conf["GlobalProducer"].remove("hlt_selector")
    conf = base.expand(conf, ['all'])
    for p in conf['Pipelines'].keys():
        conf['Pipelines'][p]['Filter'].remove('hlt')
    return conf
