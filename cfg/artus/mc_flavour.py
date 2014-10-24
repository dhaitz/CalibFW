import ArtusConfigBase as base
import mc


def config():
    cfg = mc.config()
    cfg['FlavourCorrections'] = True
    for i in cfg['Pipelines']:
        cfg['Pipelines'][i]['QuantitiesVector'] += ["algol5pt", "physl5pt"]
        cfg['Pipelines'][i]['QuantitiesString'] += ":algol5pt:physl5pt"
    return cfg
