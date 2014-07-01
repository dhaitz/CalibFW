import ArtusConfigBase as base
import mc_rundep


def config():
    conf = mc_rundep.config()
    for pipeline in conf['Pipelines']:
        conf['Pipelines'][pipeline]['Consumer'] = [
            "muonntuple",
            "jetntuple",
	        "genparticlentuple"
        ]
    return conf
