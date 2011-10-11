import FWCore.ParameterSet.Config as cms
 
class ZJetBase:
  
    @staticmethod
    def getBaseConfig( src_file = "", global_tag = '@GLOBALTAG@', additional_actives = [] ):
	# Basic process setup ----------------------------------------------------------
	process = cms.Process("kappaSkim")
	process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring(
		#'file:///home/piparo/testFiles/Spring10_MinBias_GENSIMRECO_MC_3XY_V25_S09_preproduction-v2.root',
		src_file,
		#'file:///storage/6/zeise/temp/minbias_pr_v9_FE9B4520-7D5B-DF11-B4DA-0019DB2F3F9A.root'
	))
	process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(500) )
	#-------------------------------------------------------------------------------

	# Includes + Global Tag --------------------------------------------------------
	process.load("FWCore.MessageService.MessageLogger_cfi")
	process.load('Configuration/StandardSequences/Services_cff')
	process.load('Configuration.StandardSequences.MagneticField_38T_cff')
	process.load('Configuration.StandardSequences.Geometry_cff')
	process.load('Configuration/StandardSequences/GeometryPilot2_cff')
	process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
	process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi")
	process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi")
	process.load("RecoMuon.DetLayers.muonDetLayerGeometry_cfi")
	#process.load('RecoJets.Configuration.RecoJetAssociations_cff')
	process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
	process.load("Configuration.StandardSequences.Reconstruction_cff")
	process.GlobalTag.globaltag = global_tag
	#-------------------------------------------------------------------------------

	# Reduce amount of messages ----------------------------------------------------
	process.MessageLogger.default = cms.untracked.PSet(ERROR = cms.untracked.PSet(limit = cms.untracked.int32(5)))
	process.MessageLogger.cerr.FwkReport.reportEvery = 40
	#-------------------------------------------------------------------------------

	# Produce PF muon isolation ----------------------------------------------------
	from CommonTools.ParticleFlow.Isolation.tools_cfi import *
	process.pfmuIsoDepositPFCandidates = isoDepositReplace("muons", "particleFlow")
	process.pfMuonIsolCandidates = cms.Path(process.pfmuIsoDepositPFCandidates)
	#-------------------------------------------------------------------------------

	# Produce rho distribution------------------------------------------------------
	process.load('RecoJets.JetProducers.kt4PFJets_cfi')
	process.kt6PFJetsRho = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
	process.kt6PFJetsRho.Rho_EtaMax = cms.double(5.)
	process.JetArea = cms.Path(process.kt6PFJetsRho)
	#-------------------------------------------------------------------------------
	
	
	# enable area calculation for all algos used
	process.ak5PFJets.doAreaFastjet = True
	process.ak7PFJets.doAreaFastjet = True
	process.kt4PFJets.doAreaFastjet = True
	process.kt6PFJets.doAreaFastjet = True

	
	# CHS Jets !
	
	# load the NoPU sequenece 
	process.load('CommonTools.ParticleFlow.pfNoPileUp_cff')
	process.pfCHS = cms.Path( process.pfNoPileUpSequence )
	
	process.ak5PFJetsCHS = process.ak5PFJets.clone( src = cms.InputTag("pfNoPileUp") )
	process.ak7PFJetsCHS = process.ak7PFJets.clone( src = cms.InputTag("pfNoPileUp") )
	process.kt4PFJetsCHS = process.kt4PFJets.clone( src = cms.InputTag("pfNoPileUp") )
	process.kt6PFJetsCHS = process.kt6PFJets.clone( src = cms.InputTag("pfNoPileUp") )
	

	# Path to Redo all Jets
	process.JetsRedo = cms.Path( process.ak5PFJets * process.ak7PFJets * process.kt4PFJets * process.kt6PFJets *
				     process.ak5PFJetsCHS * process.ak7PFJetsCHS * process.kt4PFJetsCHS * process.kt6PFJetsCHS )


	# Require one good muon --------------------------------------------------------
	# and the muon must have Pt > 5 GeV
	process.goodMuons = cms.EDFilter("CandViewSelector",
		src = cms.InputTag("muons"),
		cut = cms.string("pt > 12.0 & abs( eta ) < 8.0 & isGlobalMuon()"),
	)

	process.oneGoodMuon = cms.EDFilter("CandViewCountFilter",
	    src = cms.InputTag("goodMuons"),
	    minNumber = cms.uint32(2),
	)
	#-------------------------------------------------------------------------------

	# Configure tuple generation ---------------------------------------------------
	process.load("Kappa.Producers.KTuple_cff")
	process.kappatuple = cms.EDAnalyzer('KTuple',
		process.kappaTupleDefaultsBlock,
		outputFile = cms.string('skim.root')
	)

	process.kappatuple.verbose = cms.int32(0)
	process.kappatuple.active = cms.vstring(
		'L1Muons', 'Muons', 'TrackSummary', 'TriggerObjects', 'Vertex', 'BeamSpot', 'JetArea', 'PFMET', 'PFJets'
	)
	for ac in additional_actives:
	    process.kappatuple.active.append( ac )
	
	process.kappatuple.Tracks.minPt = cms.double(5.)

	process.kappatuple.Metadata.hltWhitelist = cms.vstring(
		"^HLT_(QuadJet|Jet|DiJetAve)[0-9]+U(_v[[:digit:]]+)?$",
		"^HLT_(L[123])?(Iso|Double)?Mu([0-9]+)(_v[[:digit:]]+)?$",
		"^HLT_Activity.*", ".*(Bias|BSC).*",
	)

	# use the jets created during the kappa skim and not the RECO Jet
	process.kappatuple.PFJets.whitelist = cms.vstring("recoPFJets.*kappaSkim")

	# Process schedule -------------------------------------------------------------
	#process.pathDAT = cms.Path(process.recoJetAssociations+process.kappatuple)
	process.pathDAT = cms.Path(process.goodMuons*process.oneGoodMuon*process.kappatuple)
	process.schedule = cms.Schedule(  
			    process.pfCHS,
			    process.JetArea, process.JetsRedo,  
			    process.pfMuonIsolCandidates, process.pathDAT )
	#-------------------------------------------------------------------------------
	return process

    @staticmethod
    def getOutputModule( process ):
	process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
	splitLevel = cms.untracked.int32(0),
	eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
	    outputCommands = process.RECOSIMEventContent.outputCommands,
		fileName = cms.untracked.string('step3_RELVAL_RAW2DIGI_L1Reco_RECO_VALIDATION_RECO.root'),
		dataset = cms.untracked.PSet(
		    filterName = cms.untracked.string(''), 
		    dataTier = cms.untracked.string('GEN-SIM-RECO')
		)
	    )
	# and add this to the end step
	process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)


	
    