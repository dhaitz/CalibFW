import ArtusConfigBase as base
import socket


def config():
    conf = base.BaseConfig('data', '2012', analysis='zee')
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_06_23_zee_data/*.root"
    #conf['GlobalProducer'].remove('hlt_selector')
    #conf['Pipelines']['default']['Filter'].remove('hlt')
    conf = base.expand(conf, ['all', 'zcuts'])
    if socket.gethostname().startswith('naf'):
	conf['InputFiles'] = "/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_06_23_ee-data"
    return conf
