import FWCore.ParameterSet.Config as cms
from Kappa.Producers.skim_Base_42x_ZJet import ZJetBase

gtag = '@GLOBALTAG@'
gtag = 'GR_R_42_V19::All'

process = ZJetBase.getBaseConfig( src_file = 'file:///storage/5/hauth/zpj/test_data/data_DoubleMu_Run2011A_ReReco_May10_AOD.root',
				  global_tag = gtag,
				  additional_actives = ['DataMetadata', 'TriggerObjects'])


#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 1 ) )
#process.kappatuple.verbose = cms.int32(30)

