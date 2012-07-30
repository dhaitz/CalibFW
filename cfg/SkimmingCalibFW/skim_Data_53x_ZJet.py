import Kappa.Producers.skim_Base_53x_ZJet as base

process = base.getBaseConfig(
    'GR_R_52_V9',
    'file:/storage/6/berger/testfiles/data_2012A_AOD.root',
    ['DataMetadata', 'TriggerObjects'],
    maxevents=2000,
    residual=True
)
