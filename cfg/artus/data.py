import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012')
    conf["InputFiles"] = base.setInputFiles(
        ekppath = "/storage/a/dhaitz/skims/2014_03_25_data12/*.root",
        nafpath = "/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_03_25_data12"
    )
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    conf['Jec'] = base.getPath() + "/data/jec/Winter14_V5/Winter14_V5_DATA"
    conf['RC'] = True
    return conf
