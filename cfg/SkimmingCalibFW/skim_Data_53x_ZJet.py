import Kappa.Producers.skim_Base_53x_ZJet as base

process = base.getBaseConfig(
    'GR_R_53_V8',
    'file:/storage/6/berger/testfiles/data_2012C_AOD.root',
    ['DataMetadata', 'TriggerObjects'],
    maxevents=300,
    residual=True
)
