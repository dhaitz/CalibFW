import ArtusConfigBase as base
import mc_ee_corr


def config():
    conf = mc_ee_corr.config()
    for pipeline in conf['Pipelines']:
        conf['Pipelines'][pipeline]['Consumer'] = [
            "electronntuple",
            "jetntuple",
        ]
    return conf
