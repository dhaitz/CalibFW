import ArtusConfigBase as base
import mc


def config():
    conf = mc.conf()
    conf['VetoPileupJets'] = True
    return conf

