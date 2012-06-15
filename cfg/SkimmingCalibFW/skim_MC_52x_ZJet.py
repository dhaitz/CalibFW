import Kappa.Producers.skim_Base_52x_ZJet as base

globaltag = 'START52_V11'

process = base.getBaseConfig(
    globaltag,
    'file:/storage/6/berger/testfiles/mc_Fall11_DY_M-20_TuneZ2_pythia_AODSIM.root',
    ['GenMetadata', 'Partons'],
    maxevents=100
)
    
