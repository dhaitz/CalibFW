import Kappa.Producers.skim_Base_44x_ZJet as base

globaltag = '@GLOBALTAG@'
#globaltag = 'GR_R_44_V13'

process = base.getBaseConfig(
    globaltag,
    'file:/storage/6/berger/testfiles/data_DoubleMu_ReReco_AOD.root',
    ['DataMetadata', 'TriggerObjects'],
    maxevents=100
)
