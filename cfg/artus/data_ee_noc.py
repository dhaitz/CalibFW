import ArtusConfigBase as base
import data_ee_corr


def config():
    conf = data_ee_corr.config()
    l = []
    for pipeline in conf['Pipelines']:
        if not pipeline.startswith('all'):
            l.append(pipeline)
        elif pipeline.endswith('L3'):
            l.append(pipeline)
    for pipeline in l:
            del conf['Pipelines'][pipeline]
    for pipeline in conf['Pipelines']:
        conf['Pipelines'][pipeline]['Consumer'] = [
            "electronntuple",
            "jetntuple",
        ]
    return conf
