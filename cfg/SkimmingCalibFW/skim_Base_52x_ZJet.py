import FWCore.ParameterSet.Config as cms

def getBaseConfig(globaltag, srcfile="", additional_actives=[], maxevents=-1, residual=False):
    """Default config for Z+jet skims with Kappa

       This is used in a cmssw config file via:
       import skim_base
       process = skim_base.getBaseConfig('START44_V12', "filename of testfile")

       additional_actives can for example be used for MC-only products
    """
    # Basic process setup -----------------------------------------------------
    process = cms.Process('kappaSkim')
    process.source = cms.Source('PoolSource',
        fileNames = cms.untracked.vstring(srcfile))
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(maxevents))

    # Includes + Global Tag ---------------------------------------------------
    process.load('FWCore.MessageService.MessageLogger_cfi')
    process.load('Configuration.StandardSequences.Services_cff')
    process.load('Configuration.StandardSequences.MagneticField_38T_cff')
    process.load('Configuration.StandardSequences.Geometry_cff')
    process.load('Configuration.StandardSequences.GeometryPilot2_cff')
    process.load('TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi')
    process.load('TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi')
    process.load('TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi')
    process.load('RecoMuon.DetLayers.muonDetLayerGeometry_cfi')
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
    process.load('Configuration.StandardSequences.Reconstruction_cff')
    process.GlobalTag.globaltag = globaltag + '::All'

    # Reduce amount of messages -----------------------------------------------
    process.MessageLogger.default = cms.untracked.PSet(
        ERROR = cms.untracked.PSet(limit = cms.untracked.int32(5)))
    process.MessageLogger.cerr.FwkReport.reportEvery = 40

    # Produce PF muon isolation -----------------------------------------------
    from CommonTools.ParticleFlow.Isolation.tools_cfi import isoDepositReplace
    process.pfmuIsoDepositPFCandidates = isoDepositReplace('muons', 'particleFlow')
    process.pfMuonIso = cms.Path(process.pfmuIsoDepositPFCandidates)

    # Create good primary vertices to be used for PF association --------------
    from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
    process.goodOfflinePrimaryVertices = cms.EDFilter(
        "PrimaryVertexObjectFilter",
        filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
        src=cms.InputTag('offlinePrimaryVertices')
    )
    process.ak5PFJets.srcPVs = cms.InputTag('goodOfflinePrimaryVertices')
    process.ak7PFJets.srcPVs = cms.InputTag('goodOfflinePrimaryVertices')
    process.kt4PFJets.srcPVs = cms.InputTag('goodOfflinePrimaryVertices')
    process.kt6PFJets.srcPVs = cms.InputTag('goodOfflinePrimaryVertices')
    process.pfPileUp.Vertices = cms.InputTag("goodOfflinePrimaryVertices")
    process.pfPileUp.checkClosestZVertex = cms.bool(False)

    # CHS Jets with the NoPU sequence -----------------------------------------
    process.load('CommonTools.ParticleFlow.PFBRECO_cff')
    process.pfCHS = cms.Path(process.goodOfflinePrimaryVertices * process.PFBRECO)
    process.ak5PFJetsCHS = process.ak5PFJets.clone( src = cms.InputTag('pfNoPileUp') )
    process.ak7PFJetsCHS = process.ak7PFJets.clone( src = cms.InputTag('pfNoPileUp') )
    process.kt4PFJetsCHS = process.kt4PFJets.clone( src = cms.InputTag('pfNoPileUp') )
    process.kt6PFJetsCHS = process.kt6PFJets.clone( src = cms.InputTag('pfNoPileUp') )

    # Path to Redo all Jets
    process.jetsRedo = cms.Path(
        process.ak5PFJets * process.ak7PFJets * process.kt4PFJets * process.kt6PFJets *
        process.ak5PFJetsCHS * process.ak7PFJetsCHS * process.kt4PFJetsCHS * process.kt6PFJetsCHS
    )

    # MET correction (only for ak5PFJets) -------------------------------------
    process.load("JetMETCorrections.Type1MET.pfMETCorrections_cff")
    process.pfJetMETcorrAK5PFL2L3Res = process.pfJetMETcorr.clone(
        jetCorrLabel = cms.string("ak5PFL1FastL2L3Residual")
    )
    process.pfJetMETcorrAK5PFL2L3 = process.pfJetMETcorr.clone(
        jetCorrLabel = cms.string("ak5PFL1FastL2L3")
    )
    process.pfJetMETcorrAK5PFL1L2L3 = process.pfJetMETcorrAK5PFL2L3.clone(
        offsetCorrLabel = cms.string("")
    )
    process.pfJetMETcorrAK5PFL1 = process.pfJetMETcorrAK5PFL1L2L3.clone(
        jetCorrLabel = cms.string("ak5PFL1Fastjet")
    )
    # Type-0+I
    process.pfMETCHS = process.pfType1CorrectedMet.clone(
        applyType1Corrections = cms.bool(False)
    )
    process.ak5PFMETCHSL1 = process.pfType1CorrectedMet.clone(
        srcType1Corrections = cms.VInputTag(cms.InputTag('pfJetMETcorrAK5PFL1', 'type1'))
    )
    process.ak5PFMETCHSL2L3 = process.pfType1CorrectedMet.clone(
        srcType1Corrections = cms.VInputTag(cms.InputTag('pfJetMETcorrAK5PFL2L3', 'type1'))
    )
    process.ak5PFMETCHSL1L2L3 = process.pfType1CorrectedMet.clone(
        srcType1Corrections = cms.VInputTag(cms.InputTag('pfJetMETcorrAK5PFL1L2L3', 'type1'))
    )
    process.ak5PFMETCHSL2L3Res = process.pfType1CorrectedMet.clone(
        srcType1Corrections = cms.VInputTag(cms.InputTag('pfJetMETcorrAK5PFL2L3Res', 'type1'))
    )
    # Type-I only
    process.ak5PFMETL1 = process.ak5PFMETCHSL1.clone(applyType0Corrections = cms.bool(False))
    process.ak5PFMETL2L3 = process.ak5PFMETCHSL2L3.clone(applyType0Corrections = cms.bool(False))
    process.ak5PFMETL1L2L3 = process.ak5PFMETCHSL1L2L3.clone(applyType0Corrections = cms.bool(False))
    process.ak5PFMETL2L3Res = process.ak5PFMETCHSL2L3Res.clone(applyType0Corrections = cms.bool(False))
    # MET Path
    process.metCorrections = cms.Path(process.producePFMETCorrections)
    process.metRedo = cms.Path(
            process.pfJetMETcorrAK5PFL2L3Res * process.pfJetMETcorrAK5PFL2L3 *
            process.pfJetMETcorrAK5PFL1L2L3 * process.pfJetMETcorrAK5PFL1 *
            process.pfMETCHS *
            process.ak5PFMETL1  * process.ak5PFMETCHSL1 *
            process.ak5PFMETL1L2L3 * process.ak5PFMETCHSL1L2L3 *
            process.ak5PFMETL2L3 * process.ak5PFMETCHSL2L3
        )
    if True:
        process.metRedo *= process.ak5PFMETL2L3Res
        process.metRedo *= process.ak5PFMETCHSL2L3Res

    # Require one good muon --------------------------------------------------------
    process.goodMuons = cms.EDFilter('CandViewSelector',
        src = cms.InputTag('muons'),
        cut = cms.string("pt > 12.0 & abs(eta) < 8.0 & isGlobalMuon()"),
    )
    process.oneGoodMuon = cms.EDFilter('CandViewCountFilter',
        src = cms.InputTag('goodMuons'),
        minNumber = cms.uint32(2),
    )

    # Configure tuple generation ---------------------------------------------------
    process.load('Kappa.Producers.KTuple_cff')
    process.kappatuple = cms.EDAnalyzer('KTuple',
        process.kappaTupleDefaultsBlock,
        outputFile = cms.string("skim.root")
    )
    process.kappatuple.verbose = cms.int32(0)
    process.kappatuple.active = cms.vstring(
        'LV', 'L1Muons', 'Muons', 'TrackSummary', 'VertexSummary', 'BeamSpot',
        'JetArea', 'PFMET', 'PFJets',
    )
    for active in additional_actives:
        process.kappatuple.active.append(active)

    # custom whitelist, otherwise the HLT trigger bits are not sufficient !
    process.kappatuple.Metadata.hltWhitelist = cms.vstring(
        # matches 'HLT_Mu17_Mu8_v7' etc.
        "^HLT_(L[123])?(Iso|Double)?Mu([0-9]+)_(L[123])?(Double)?Mu([0-9]+)(_v[[:digit:]]+)?$",
        # matches 'HLT_DoubleMu7_v8' etc.
        "^HLT_(L[123])?(Iso|Double)?Mu([0-9]+)(_v[[:digit:]]+)?$",
    )

    # use the jets created during the kappa skim and not the RECO Jet
    process.kappatuple.PFJets.whitelist = cms.vstring("recoPFJets.*kappaSkim")
    process.pathKappa = cms.Path(process.goodMuons*process.oneGoodMuon*process.kappatuple)

    # Process schedule --------------------------------------------------------
    process.schedule = cms.Schedule(
            process.pfCHS,
            process.jetsRedo,
            process.pfMuonIso,
            process.metCorrections,
            process.metRedo,
            process.pathKappa,
    )

    return process


def getOutputModule(process):
    process.RECOSIMoutput = cms.OutputModule('PoolOutputModule',
        splitLevel = cms.untracked.int32(0),
        eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
        outputCommands = process.RECOSIMEventContent.outputCommands,
        fileName = cms.untracked.string('step3_RELVAL_RAW2DIGI_L1Reco_RECO_VALIDATION_RECO.root'),
        dataset = cms.untracked.PSet(
            filterName = cms.untracked.string(''),
            dataTier = cms.untracked.string('GEN-SIM-RECO'),
        )
    )
    # and add this to the end step
    process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)
