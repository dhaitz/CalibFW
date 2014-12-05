import ArtusConfigBase as base

def config():
    cfg = base.BaseConfig('mc', '2012', rundepMC=True, flavourCorrections=True)
    cfg["InputFiles"] = base.setInputFiles(
        ekppath="",
        nafpath="/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_11_25_mm_mc_herwig"
    )
    base.ApplySampleReweighting(cfg, sample = 'herwigRD')
    cfg['RC'] = True
    cfg['EnableLumiReweighting'] = False
    cfg['EnableTriggerReweighting'] = False
    cfg = base.expand(cfg, ['all', 'zcuts', 'incut'])
    return cfg
