import Kappa.Producers.skim_Base_53x_ZJet as base

process = base.getBaseConfig(
    'START52_V11',
    'file:/storage/6/berger/testfiles/mc_madgraphV9_AOD.root',
    ['GenMetadata', 'Partons'],
    maxevents=500
)

