# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms

# Meta configuration |----------------------------------------------------------

#production_id = "Spring10"

# Configuration |---------------------------------------------------------------

process = cms.Process("DATA")
p = process # shortcut!

#__FILE_NAMES__=["dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010B/Mu/RECO/PromptReco-v2/000/146/898/202CDD2B-5DCB-DF11-810A-003048F1C58C.root"]
#"dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010A/Mu/RECO/v4/000/140/387/14E55AAA-C992-DF11-9A3C-001D09F24259.root",
#"dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010A/Mu/RECO/v4/000/140/387/02E68AB1-CB92-DF11-83CA-003048D373AE.root"]
#__FILE_NAMES__=["dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010A/Mu/RECO/v4/000/140/124/5674E64F-398F-DF11-9600-0019B9F730D2.root"]

#__FILE_NAMES__=["dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010B/Mu/RECO/PromptReco-v2/000/146/721/AE8A955E-31CA-DF11-B735-0030487CD6DA.root"]
#__SKIP_EVENTS__ = 0 
#__MAX_EVENTS__ = 60000
p.source = cms.Source("PoolSource",
#                      skipBadFiles = cms.untracked.bool(True),
                       skipEvents = cms.untracked.uint32(__SKIP_EVENTS__),
                       fileNames = cms.untracked.vstring(__FILE_NAMES__),
                      )

# The max events number to process
p.maxEvents=cms.untracked.PSet(input = cms.untracked.int32(__MAX_EVENTS__))

# Begin of TreeMaker-config ====================================================
# Message logger and count of processed events 

#p.MessageLogger = cms.Service("MessageLogger",
#	destinations   = cms.untracked.vstring('cout'),
#	cout           = cms.untracked.PSet(
#                       threshold = cms.untracked.string('DEBUG')),
        # enable log debug output for all Modules
#        debugModules  = cms.untracked.vstring('*')
#)

#import Zplusjet.ZplusjetTreeMaker.ZplusJetConfigHelper as ZplusJetConfigHelper

p.processedEvents = cms.EDProducer('EventCounter',
	 index_label = cms.untracked.string("processedEvents"),
	 )
	 
p.processedEventsPath = cms.Path(process.processedEvents)

# The Zs  and the Z+jet-analyzer
import  Zplusjet.ZplusjetTreeMaker.Z_collections_noprecuts_cfi as Zs 
p.extend(Zs)


# The test histos --------------------------------------------------------------
#import  Zplusjet.ZplusjetTreeMaker.Z_test_histos_cfi as test_histos 
#p.extend(test_histos)


p.flatTreeMaker = cms.EDAnalyzer('ZplusjetTreeMaker',
	reco_zs_name = cms.string('goodZToMuMuReco'),
	gen_zs_name = cms.string(''),
	zJetPhiBalancing = cms.double(10.0),			# Z plus Jets
	jetMaxEta = cms.double(8.0),
	zMuMinDr = cms.double(0.1),	 			# Is muon a seed for the genjet?
	algo_whitelist_vstring = cms.untracked.vstring(''),
	algo_blacklist_vstring = cms.untracked.vstring(''),
	secondJetZptRatio = cms.double(100.0),
	output_filename = cms.untracked.string('out.root'),#'ftm_@PTMIN@to@PTMAX@_7TeV.root'),
)

p.flatTreeMakerPath = cms.Path(p.flatTreeMaker)

p.out = cms.OutputModule("PoolOutputModule",
		fileName = cms.untracked.string("test.root")
		)
p.outpath=cms.Path(p.out)


# The gen jets matching --------------------------------------------------------

calo_jets_names=("iterativeCone5CaloJets",
                 "kt4CaloJets",
                 "kt6CaloJets",
                 "sisCone5CaloJets",
                 "sisCone7CaloJets",
                 "ak5CaloJets")

gen_jets_names=("iterativeCone5GenJets",
                "kt4GenJets",
                "kt6GenJets",
                "sisCone5GenJets",
                "sisCone7GenJets",
                "ak5GenJets")

jets_names_dict=zip(gen_jets_names,calo_jets_names)

#-------------------------------------------------------------------------------
# Add the flavour matcher of B tagging group - Physics definition

#jetnames=["iterativeCone5CaloJets",
#          "kt4CaloJets",
#          "kt6CaloJets",
#          "sisCone5CaloJets",
#          "sisCone7CaloJets",
#          "L2CorJetIC5Calo",
#          "L2CorJetSC5Calo",
#          "L2CorJetSC7Calo",
#          "L2CorJetKT4Calo",
#          "L2CorJetKT6Calo",
#          "L2L3CorJetIC5Calo",
#          "L2L3CorJetSC5Calo",
#          "L2L3CorJetSC7Calo",
#          "L2L3CorJetKT4Calo",
#          "L2L3CorJetKT6Calo"]


#p.myPartons = cms.EDFilter("PartonSelector",
#    withLeptons = cms.bool(False)
#)

#p.flavourmatchingpath=cms.Path(p.myPartons)

#for jetname in jetnames:
#    setattr(p,"flavourByRef%s"%jetname,cms.EDFilter("JetPartonMatcher",
#                          jets = cms.InputTag(jetname),
#                          coneSizeToAssociate = cms.double(0.3),
#                          partons = cms.InputTag("myPartons")))

#    exec("p.flavourmatchingpath+=p.flavourByRef%s"%jetname)



# Schedule
process.schedule = cms.Schedule(
	p.processedEventsPath,		# TreeMaker events
	p.goodZToMuMuRecoPath,
	p.flatTreeMakerPath		# TreeMaker flat root-file
#	p.outpath,
)



# 
#  Customizations for Grid Control follow here
#

def customise_for_gc(process):
	import FWCore.ParameterSet.Config as cms
	from IOMC.RandomEngine.RandomServiceHelper import RandomNumberServiceHelper

	try:
		maxevents = __MAX_EVENTS__
		process.maxEvents = cms.untracked.PSet(
			input = cms.untracked.int32(max(-1, maxevents))
		)
	except:
		pass

	# Dataset related setup
	try:
		tmp = __SKIP_EVENTS__
		process.source = cms.Source("PoolSource",
			skipEvents = cms.untracked.uint32(__SKIP_EVENTS__),
			fileNames = cms.untracked.vstring(__FILE_NAMES__)
		)
		try:
			secondary = __FILE_NAMES2__
			process.source.secondaryFileNames = cms.untracked.vstring(secondary)
		except:
			pass
		try:
			lumirange = [__LUMI_RANGE__]
			process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange(lumirange)
		except:
			pass
	except:
		pass

	if hasattr(process, "RandomNumberGeneratorService"):
		randSvc = RandomNumberServiceHelper(process.RandomNumberGeneratorService)
		randSvc.populate()

	process.AdaptorConfig = cms.Service("AdaptorConfig",
		enable = cms.untracked.bool(True),
		stats = cms.untracked.bool(True),
	)

	# Generator related setup
	try:
		if hasattr(process, "generator"):
			process.source.firstLuminosityBlock = cms.untracked.uint32(1+__MY_JOBID__)
			print "Generator random seed:", process.RandomNumberGeneratorService.generator.initialSeed
	except:
		pass

	return (process)

process = customise_for_gc(process)

# grid-control: https://ekptrac.physik.uni-karlsruhe.de/trac/grid-control
