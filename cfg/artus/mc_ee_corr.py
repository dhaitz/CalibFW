import ArtusConfigBase as base
import mc_ee_raw


def config():
    conf = mc_ee_raw.config()
    conf['electrons'] = 'correlectrons'
    return conf

