import ArtusConfigBase as base
import mc_ee_corr_noid


def config():
    conf = mc_ee_corr_noid.config()
    for pipeline in conf['Pipelines']:
        conf['Pipelines'][pipeline]['Consumer'] = [
            "electronntuple",
            "jetntuple",
        ]
    return conf
