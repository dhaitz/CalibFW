import ArtusConfigBase as base
import mc_ee_raw


def config():
    conf = mc_ee_raw.config()
    conf['electrons'] = 'correlectrons'
    conf['ElectronID'] = 'none'
    conf['GlobalProducer'].remove('electron_sf_producer')
    return conf

