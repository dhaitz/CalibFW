import FWCore.ParameterSet.Config as cms
import copy

import FWCore.ParameterSet.Config as cms
from Kappa.Producers.skim_Base_42x_ZJet import ZJetBase

gtag = '@GLOBALTAG@'
#gtag =  'START42_V13::All'

process = ZJetBase.getBaseConfig( src_file = 'file:///storage/5/hauth/zpj/test_data/mc_DYToMuMu_M-20_TuneZ2_7TeV-pythia6_AOD.root',
				  global_tag = gtag,
				  additional_actives = ['GenMetadata', 'Partons'] )
