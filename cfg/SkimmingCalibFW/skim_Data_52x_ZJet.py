import Kappa.Producers.skim_Base_52x_ZJet as base

process = base.getBaseConfig(
    'GR_R_52_V9',
    'file:/storage/6/berger/testfiles/data_2012A_AOD.root',
    ['DataMetadata', 'TriggerObjects'],
    maxevents=20000,
    residual=True
)
