import ArtusConfigBase as base
import data


def config():
    conf = data.config()
    conf['VetoPileupJets'] = True
    return conf

