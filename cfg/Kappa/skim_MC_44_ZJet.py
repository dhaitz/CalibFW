import skim_base as base

process = base.getBaseConfig(
    'START44_V12',
    'file:/storage/6/berger/testfiles/mc_Fall11_DY_M-20_TuneZ2_pythia_AODSIM.root',
    ['GenMetadata', 'Partons'],
    maxevents=100)
    
