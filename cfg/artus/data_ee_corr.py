import ArtusConfigBase as base
import data_ee_raw


def config():
    conf = data_ee_raw.config()
    conf['electrons'] = 'correlectrons'
    return conf
