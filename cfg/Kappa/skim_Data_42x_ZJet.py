import FWCore.ParameterSet.Config as cms

# Basic process setup ----------------------------------------------------------
process = cms.Process("kappaSkim")
process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring(
	#'file:///home/piparo/testFiles/Spring10_MinBias_GENSIMRECO_MC_3XY_V25_S09_preproduction-v2.root',
	'file:///afs/naf.desy.de/user/h/hauth/lustre/ZPJ2011/examples/data_DoubleMu_Run2011A_ReReco_May10_AOD.root',
	#'file:///storage/6/zeise/temp/minbias_pr_v9_FE9B4520-7D5B-DF11-B4DA-0019DB2F3F9A.root'
))
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
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
process.GlobalTag.globaltag = '@GLOBALTAG@' #''GR_R_42_V19::All''
#-------------------------------------------------------------------------------

# Reduce amount of messages ----------------------------------------------------
process.MessageLogger.default = cms.untracked.PSet(ERROR = cms.untracked.PSet(limit = cms.untracked.int32(5)))
process.MessageLogger.cerr.FwkReport.reportEvery = 42
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

# JEC corrected Jet Collections
process.load("JetMETCorrections.Configuration.DefaultJEC_cff")
process.load("JetMETCorrections.Configuration.JetCorrectionProducers_cff")

process.kt6PFJets.doRhoFastjet = True
process.pathJEC = cms.Path(process.kt6PFJets *
         process.ak5PFJetsL1FastL2L3 *
         process.ak7PFJetsL1FastL2L3 *
	 process.ak5PFJetsL1FastL2L3Residual *
	 process.ak7PFJetsL1FastL2L3Residual  )



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
	outputFile = cms.string('skim.root'),
	CaloJets = cms.PSet(
			process.kappaNoCut,
			process.kappaNoRegEx,
			srcNoiseHCAL = cms.InputTag(""),
			AK5CaloJets = cms.PSet(
				src = cms.InputTag("ak5CaloJets"),
				srcExtender = cms.InputTag("ak5JetExtender"),
				srcJetID = cms.InputTag("ak5JetID"),
			),
			AK7CaloJets = cms.PSet(
				src = cms.InputTag("ak7CaloJets"),
				srcExtender = cms.InputTag("ak7JetExtender"),
				srcJetID = cms.InputTag("ak7JetID"),
			),
			#IC5CaloJets = cms.PSet(
			#	src = cms.InputTag("iterativeCone5CaloJets"),
			#	srcExtender = cms.InputTag("iterativeCone5JetExtender"),
			#	srcJetID = cms.InputTag("ic5JetID"),
			#),
			KT4CaloJets = cms.PSet(
				src = cms.InputTag("kt4CaloJets"),
				srcExtender = cms.InputTag("kt4JetExtender"),
				srcJetID = cms.InputTag("kt4JetID")
			),
			KT6CaloJets = cms.PSet(
				src = cms.InputTag("kt6CaloJets"),
				srcExtender = cms.InputTag(""),
				srcJetID = cms.InputTag("kt6JetID")
			)
	)
)
process.kappatuple.verbose = cms.int32(0)
process.kappatuple.active = cms.vstring(
	'L1Muons', 'Muons', 'TrackSummary', 'TriggerObjects', 'Vertex', 'BeamSpot', 'JetArea', 'PFMET', 'CaloJets', 'PFJets'
)
process.kappatuple.Tracks.minPt = cms.double(5.)

process.kappatuple.Metadata.hltWhitelist = cms.vstring(
	"^HLT_(QuadJet|Jet|DiJetAve)[0-9]+U(_v[[:digit:]]+)?$",
	"^HLT_(L[123])?(Iso|Double)?Mu([0-9]+)(_v[[:digit:]]+)?$",
	"^HLT_Activity.*", ".*(Bias|BSC).*",
)
#-------------------------------------------------------------------------------

process.kappatuple.PFJets.whitelist.append("...PFJetsL1.*")


# Process schedule -------------------------------------------------------------
#process.pathDAT = cms.Path(process.recoJetAssociations+process.kappatuple)
process.pathDAT = cms.Path(process.goodMuons*process.oneGoodMuon*process.kappatuple)
process.schedule = cms.Schedule( process.JetArea, process.pathJEC, process.pfMuonIsolCandidates, process.pathDAT)
#-------------------------------------------------------------------------------
