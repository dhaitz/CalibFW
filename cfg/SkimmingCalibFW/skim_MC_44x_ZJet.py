import Kappa.Producers.skim_Base_44x_ZJet as base

globaltag = '@GLOBALTAG@'
#globaltag = 'START44_V12'

process = base.getBaseConfig(
    globaltag,
    'file:/storage/6/berger/testfiles/mc_Fall11_DY_M-20_TuneZ2_pythia_AODSIM.root',
    ['GenMetadata', 'Partons'],
    maxevents=100
)
    
