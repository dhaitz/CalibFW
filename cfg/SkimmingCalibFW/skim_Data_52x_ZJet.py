import Kappa.Producers.skim_Base_52x_ZJet as base

globaltag = 'GR_R_52_V9'

process = base.getBaseConfig(
    globaltag,
    'file:/storage/6/berger/testfiles/data_2012A_AOD.root',
    ['DataMetadata', 'TriggerObjects'],
    maxevents=2000,
    residual=True
)
