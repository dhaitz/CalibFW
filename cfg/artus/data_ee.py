import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012', analysis='zee', tagged=False)
    conf["InputFiles"] = "/storage/a/dhaitz/zee/*.root"
    conf = base.expand(conf, ['all'])
    return conf
