import ArtusConfigBase as base
import mc_ee_raw


def config():
    conf = mc_ee_raw.config()
    conf['electrons'] = 'correlectrons'
    conf['HltPaths'] = ["HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v%d" % v for v in range(15, 20)]
    conf['GlobalProducer'] += ['hlt_selector']
    for i in conf['Pipelines']:
        conf['Pipelines'][i]['Filter'].append('hlt')
    return conf

