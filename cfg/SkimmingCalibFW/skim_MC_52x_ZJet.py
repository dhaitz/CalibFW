import Kappa.Producers.skim_Base_52x_ZJet as base

globaltag = 'START52_V11'

process = base.getBaseConfig(
    globaltag,
    'file:/storage/6/berger/testfiles/mc_madgraphV9_AOD.root',
    ['GenMetadata', 'Partons'],
    maxevents=500
)
    
