import ArtusConfigBase as base
import data


def config():
    conf = data.config()
    for pipeline in conf['Pipelines']:
        conf['Pipelines'][pipeline]['Consumer'] = [
            "muonntuple",
            "jetntuple",
        ]
    return conf
