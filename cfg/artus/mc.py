import ArtusConfigBase as base
import copy

def config():
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = "/storage/a/berger/zpj/kappa539_MC12_madgraph_tags/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    for pipeline in conf['Pipelines'].keys():
        if pipeline.startswith('incut_') and pipeline.endswith('L1L2L3'):
            newpipeline = pipeline.replace('L2L3', '')
            conf['Pipelines'][newpipeline] = copy.deepcopy(conf['Pipelines'][pipeline])
            conf['Pipelines'][newpipeline]['JetAlgorithm'] = conf['Pipelines'][pipeline]['JetAlgorithm'].replace('L2L3', '')

            newpipeline = pipeline.replace('L1L2L3', '')
            conf['Pipelines'][newpipeline] = copy.deepcopy(conf['Pipelines'][pipeline])
            conf['Pipelines'][newpipeline]['JetAlgorithm'] = conf['Pipelines'][pipeline]['JetAlgorithm'].replace('L1L2L3', '')
    return conf

