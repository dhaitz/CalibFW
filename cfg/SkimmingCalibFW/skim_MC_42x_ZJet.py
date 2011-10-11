import FWCore.ParameterSet.Config as cms
import copy

import FWCore.ParameterSet.Config as cms
from skim_Base_42x_ZJet import ZJetBase

process = ZJetBase.getBaseConfig( src_file = 'file:/scratch/hh/lustre/cms/user/hauth/ZPJ2011/examples/mc_DYToMuMu_M-20_TuneZ2_7TeV-pythia6_AOD.root',
				  global_tag = 'MC42_V13::All',
				  additional_actives = ['GenMetadata', 'Partons'] )