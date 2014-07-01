import ArtusConfigBase as base
import mc_rundep


def config():
    conf = mc_rundep.conf()
    conf['VetoPileupJets'] = True
    return conf

