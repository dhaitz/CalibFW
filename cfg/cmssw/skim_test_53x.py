import Kappa.Producers.skim_Base_53x_ZJet as base

process = base.getBaseConfig(
    '@GLOBALTAG@',
    maxevents=300,
    datatype='data',
)
