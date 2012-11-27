import FWCore.ParameterSet.Config as cms

def getBaseConfig(globaltag, testfile="", maxevents=0, datatype='mc'):
    """Default config for Z+jet skims with Kappa

       This is used in a cmssw config file via:
       import skim_base
       process = skim_base.getBaseConfig('START53_V12', "testfile.root")
    """
    # Set the globalt tag and datatype for testing or by grid-control ---------
    data = (datatype == 'data')
    if data:
        testfile = 'file:/storage/6/berger/testfiles/data_2012C_AOD.root'
        if '@' in globaltag: globaltag = 'GR_P_V40_AN1'
        maxevents = maxevents or 500
    else:
        testfile = 'file:/storage/6/berger/testfiles/mc_madgraphV9_AOD.root'
        if '@' in globaltag: globaltag = 'START53_V6'
        maxevents = maxevents or 100
        datatype = 'mc'
    print "GT:", globaltag, "| TYPE:", datatype, "| maxevents:", maxevents, "| file:", testfile

    # Basic process setup -----------------------------------------------------
    process = cms.Process('kappaSkim')
    process.source = cms.Source('PoolSource',
        fileNames = cms.untracked.vstring(testfile))
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(maxevents))

    # Includes + Global Tag ---------------------------------------------------
    process.load('FWCore.MessageService.MessageLogger_cfi')
    process.load('Configuration.StandardSequences.Services_cff')
    process.load('Configuration.StandardSequences.MagneticField_38T_cff')
    process.load('Configuration.Geometry.GeometryIdeal_cff')
    process.load('Configuration.Geometry.GeometryPilot2_cff')
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
    process.goodOfflinePrimaryVertices = cms.EDFilter("PrimaryVertexObjectFilter",
        filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
        src=cms.InputTag('offlinePrimaryVertices')
    )
    process.ak5PFJets.srcPVs = cms.InputTag('goodOfflinePrimaryVertices')
    process.ak7PFJets.srcPVs = cms.InputTag('goodOfflinePrimaryVertices')
    process.kt4PFJets.srcPVs = cms.InputTag('goodOfflinePrimaryVertices')
    process.kt6PFJets.srcPVs = cms.InputTag('goodOfflinePrimaryVertices')

    # CHS Jets with the NoPU sequence -----------------------------------------
    process.load('CommonTools.ParticleFlow.PFBRECO_cff')
    process.pfPileUp.Vertices = cms.InputTag('goodOfflinePrimaryVertices')
    process.pfPileUp.checkClosestZVertex = cms.bool(False)
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

    # MET filters -------------------------------------------------------------
    process.load('RecoMET.METFilters.ecalLaserCorrFilter_cfi')
    # Create good vertices for the trackingFailure MET filter
    process.goodVertices = cms.EDFilter("VertexSelector",
        filter = cms.bool(False),
        src = cms.InputTag("offlinePrimaryVertices"),
        cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.rho < 2"),
    )
    # The good primary vertex filter for other MET filters
    process.primaryVertexFilter = cms.EDFilter("VertexSelector",
        filter = cms.bool(True),
        src = cms.InputTag("offlinePrimaryVertices"),
        cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.Rho <= 2"),
    )
    process.noscraping = cms.EDFilter("FilterOutScraping",
        applyfilter = cms.untracked.bool(True),
        debugOn = cms.untracked.bool(False),
        numtrack = cms.untracked.uint32(10),
        thresh = cms.untracked.double(0.25)
    )
    process.load('CommonTools.RecoAlgos.HBHENoiseFilter_cfi')
    process.load('RecoMET.METAnalyzers.CSCHaloFilter_cfi')
    process.load('RecoMET.METFilters.hcalLaserEventFilter_cfi')
    process.hcalLaserEventFilter.vetoByRunEventNumber=cms.untracked.bool(False)
    process.hcalLaserEventFilter.vetoByHBHEOccupancy=cms.untracked.bool(True)
    process.load('RecoMET.METFilters.EcalDeadCellTriggerPrimitiveFilter_cfi')
    process.EcalDeadCellTriggerPrimitiveFilter.tpDigiCollection = cms.InputTag("ecalTPSkimNA")
    process.load('RecoMET.METFilters.EcalDeadCellBoundaryEnergyFilter_cfi')
    process.load('RecoMET.METFilters.eeBadScFilter_cfi')
    process.load('RecoMET.METFilters.eeNoiseFilter_cfi')
    process.load('RecoMET.METFilters.ecalLaserCorrFilter_cfi')
    process.load('RecoMET.METFilters.trackingFailureFilter_cfi')
    process.load('RecoMET.METFilters.inconsistentMuonPFCandidateFilter_cfi')
    process.load('RecoMET.METFilters.greedyMuonPFCandidateFilter_cfi')

    process.hcalLaserEventFilter.taggingMode = cms.bool(True)
    process.EcalDeadCellTriggerPrimitiveFilter.taggingMode = cms.bool(True)
    process.EcalDeadCellBoundaryEnergyFilter.taggingMode = cms.bool(True)
    process.trackingFailureFilter.taggingMode = cms.bool(True)
    process.eeBadScFilter.taggingMode = cms.bool(True)
    process.eeNoiseFilter.taggingMode = cms.bool(True)
    process.ecalLaserCorrFilter.taggingMode = cms.bool(True)
    process.trackingFailureFilter.taggingMode = cms.bool(True)
    process.inconsistentMuonPFCandidateFilter.taggingMode = cms.bool(True)
    process.greedyMuonPFCandidateFilter.taggingMode = cms.bool(True)
    process.beamScrapingFilter = process.inconsistentMuonPFCandidateFilter.clone(
        ptMin = cms.double(5000.0)
    )
    process.hcalNoiseFilter = process.beamScrapingFilter.clone()
    process.beamHaloFilter = process.beamScrapingFilter.clone()
    process.filtersSeq = cms.Sequence(
        process.primaryVertexFilter *
        process.hcalLaserEventFilter +
        process.EcalDeadCellTriggerPrimitiveFilter +
        process.EcalDeadCellBoundaryEnergyFilter +
        process.eeBadScFilter +
        process.eeNoiseFilter +
        process.ecalLaserCorrFilter +
        process.goodVertices * process.trackingFailureFilter +
        process.inconsistentMuonPFCandidateFilter +
        process.greedyMuonPFCandidateFilter +
        process.noscraping * process.beamScrapingFilter +
        process.HBHENoiseFilter * process.hcalNoiseFilter +
        process.CSCTightHaloFilter * process.beamHaloFilter
    )
    process.metFilters = cms.Path(process.filtersSeq)

    # MET correction for ak5PFJets and ak7PFJets (useful ones only) -----------
    process.load("JetMETCorrections.Type1MET.pfMETCorrections_cff")
    process.load("JetMETCorrections.Type1MET.pfMETsysShiftCorrections_cfi")
    process.selectedVerticesForMEtCorr.src = cms.InputTag('goodOfflinePrimaryVertices')
    # These lines get obsolete when JetMETCorrections.Type1MET.pfMETsysShiftCorrections_cfi
    # gets updated to version 1.6
    process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_data = cms.PSet(
        px = cms.string("+0.2661 + 0.3217*Nvtx"),
        py = cms.string("-0.2251 - 0.1747*Nvtx")
    )
    process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_mc = cms.PSet(
        px = cms.string("+0.1166 + 0.0200*Nvtx"),
        py = cms.string("+0.2764 - 0.1280*Nvtx")
    )
    if data:
        process.pfMEtSysShiftCorr.parameter = process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_data
    else:
        process.pfMEtSysShiftCorr.parameter = process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_mc
    # Add AK7 correction services
    process.ak7PFL1Fastjet = process.ak5PFL1Fastjet.clone(algorithm = cms.string('AK7PF'))
    process.ak7PFL1FastL2L3 = process.ak5PFL1FastL2L3.clone(algorithm = cms.string('AK7PF'))
    process.ak7PFL1FastL2L3Residual = process.ak5PFL1FastL2L3Residual.clone(algorithm = cms.string('AK7PF'))
    # MET correction levels
    process.pfJetMETcorrAK5PFL2L3 = process.pfJetMETcorr.clone(
        jetCorrLabel = cms.string("ak5PFL1FastL2L3")
    )
    process.pfJetMETcorrAK5PFL2L3Res = process.pfJetMETcorr.clone(
        jetCorrLabel = cms.string("ak5PFL1FastL2L3Residual")
    )
    process.pfJetMETcorrAK7PFL2L3 = process.pfJetMETcorr.clone(
        src = cms.InputTag("ak7PFJets"),
        jetCorrLabel = cms.string("ak7PFL1FastL2L3"),
        offsetCorrLabel = cms.string("ak7PFL1Fastjet")
    )
    process.pfJetMETcorrAK7PFL2L3Res = process.pfJetMETcorrAK7PFL2L3.clone(
        jetCorrLabel = cms.string("ak7PFL1FastL2L3Residual")
    )
    # Type-I
    process.ak5PFMETL2L3 = process.pfType1CorrectedMet.clone(
        srcType1Corrections = cms.VInputTag(cms.InputTag('pfJetMETcorrAK5PFL2L3', 'type1'))
    )
    process.ak5PFMETL2L3Res = process.pfType1CorrectedMet.clone(
        srcType1Corrections = cms.VInputTag(cms.InputTag('pfJetMETcorrAK5PFL2L3Res', 'type1'))
    )
    process.ak7PFMETL2L3 = process.pfType1CorrectedMet.clone(
        srcType1Corrections = cms.VInputTag(cms.InputTag('pfJetMETcorrAK7PFL2L3', 'type1'))
    )
    process.ak7PFMETL2L3Res = process.pfType1CorrectedMet.clone(
        srcType1Corrections = cms.VInputTag(cms.InputTag('pfJetMETcorrAK7PFL2L3Res', 'type1'))
    )
    # Type-0+I
    process.pfMETCHS = process.pfType1CorrectedMet.clone(
        applyType1Corrections = cms.bool(False),
        applyType0Corrections = cms.bool(True)
    )
    process.ak5PFMETCHSL2L3 = process.ak5PFMETL2L3.clone(applyType0Corrections = cms.bool(True))
    process.ak5PFMETCHSL2L3Res = process.ak5PFMETL2L3Res.clone(applyType0Corrections = cms.bool(True))
    process.ak7PFMETCHSL2L3 = process.ak7PFMETL2L3.clone(applyType0Corrections = cms.bool(True))
    process.ak7PFMETCHSL2L3Res = process.ak7PFMETL2L3Res.clone(applyType0Corrections = cms.bool(True))
    # MET phi corrections
    process.pfMETCHSPhi = process.pfMETCHS.clone(
        srcType1Corrections = cms.VInputTag(
            cms.InputTag('pfMEtSysShiftCorr')
        )
    )
    process.ak5PFMETCHSL2L3Phi = process.ak5PFMETCHSL2L3.clone(
        srcType1Corrections = cms.VInputTag(
            cms.InputTag('pfJetMETcorr', 'type1'),
            cms.InputTag('pfMEtSysShiftCorr')
        )
    )
    process.ak5PFMETCHSL2L3ResPhi = process.ak5PFMETCHSL2L3Res.clone(
        srcType1Corrections = cms.VInputTag(
            cms.InputTag('pfJetMETcorr', 'type1'),
            cms.InputTag('pfMEtSysShiftCorr')
        )
    )

    # MET Path
    process.metCorrections = cms.Path(
            process.pfMEtSysShiftCorrSequence * process.producePFMETCorrections *
            process.pfJetMETcorrAK5PFL2L3 * process.pfJetMETcorrAK5PFL2L3Res *
            process.pfJetMETcorrAK7PFL2L3 * process.pfJetMETcorrAK7PFL2L3Res *
            process.pfMETCHS *
            process.ak5PFMETL2L3 * process.ak5PFMETCHSL2L3 *
            process.ak7PFMETL2L3 * process.ak7PFMETCHSL2L3 *
            process.pfMETCHSPhi * process.ak5PFMETCHSL2L3Phi
        )
    if data:
        process.metCorrections *= process.ak5PFMETL2L3Res
        process.metCorrections *= process.ak5PFMETCHSL2L3Res
        process.metCorrections *= process.ak7PFMETL2L3Res
        process.metCorrections *= process.ak7PFMETCHSL2L3Res
        process.metCorrections *= process.ak5PFMETCHSL2L3ResPhi

    # Require one good muon ---------------------------------------------------
    process.goodMuons = cms.EDFilter('CandViewSelector',
        src = cms.InputTag('muons'),
        cut = cms.string("pt > 12.0 & abs(eta) < 8.0 & isGlobalMuon()"),
    )
    process.twoGoodMuons = cms.EDFilter('CandViewCountFilter',
        src = cms.InputTag('goodMuons'),
        minNumber = cms.uint32(2),
    )

    # Configure tuple generation ----------------------------------------------
    process.load('Kappa.Producers.KTuple_cff')
    process.kappatuple = cms.EDAnalyzer('KTuple',
        process.kappaTupleDefaultsBlock,
        outputFile = cms.string("skim_"+datatype+".root"),
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
        KT4CaloJets = cms.PSet(
            src = cms.InputTag("kt4CaloJets"),
            srcExtender = cms.InputTag("kt4JetExtender"),
            srcJetID = cms.InputTag("kt4JetID")
        ),
        KT6CaloJets = cms.PSet(
            src = cms.InputTag("kt6CaloJets"),
            srcExtender = cms.InputTag(""),
            srcJetID = cms.InputTag("kt6JetID")
        ),
    )
    )
    process.kappatuple.verbose = cms.int32(0)
    process.kappatuple.active = cms.vstring(
        'LV', 'Muons', 'TrackSummary', 'VertexSummary', 'BeamSpot',
        'JetArea', 'PFMET', 'PFJets', 'CaloJets', 'FilterSummary',
    )
    if data:
        additional_actives = ['DataMetadata']
    else:
        additional_actives = ['GenMetadata', 'GenParticles']
    for active in additional_actives:
        process.kappatuple.active.append(active)

    # custom whitelist, otherwise the HLT trigger bits are not sufficient!
    process.kappatuple.Metadata.hltWhitelist = cms.vstring(
        # matches 'HLT_Mu17_Mu8_v7' etc.
        "^HLT_(Double)?Mu([0-9]+)_(Double)?Mu([0-9]+)(_v[[:digit:]]+)?$",
        # matches 'HLT_DoubleMu7_v8' etc.
        "^HLT_(Double)?Mu([0-9]+)(_v[[:digit:]]+)?$",
    )
    process.kappatuple.Metadata.tauDiscrProcessName = cms.untracked.string("XXXXXXXXX")
    process.kappatuple.GenParticles.genParticles.selectedStatus = cms.int32(31)

    # use the good objects not temporary ones
    process.kappatuple.VertexSummary.whitelist = cms.vstring(".*goodOfflinePrimaryVertices.*")
    process.kappatuple.LV.whitelist += cms.vstring("recoCaloJets.*")
    process.kappatuple.PFJets.whitelist = cms.vstring("recoPFJets.*kappaSkim")
    process.kappatuple.PFMET.blacklist = cms.vstring("pfType1.*CorrectedMet")
    process.kappatuple.PFJets.blacklist = cms.vstring(".*Tau.*", "recoPFJets_pfJets.*kappaSkim")
    del process.kappatuple.GenParticles.genStableMuons

    process.pathKappa = cms.Path(
        process.goodMuons * process.twoGoodMuons * process.kappatuple
    )

    # Process schedule --------------------------------------------------------
    process.schedule = cms.Schedule(
        process.metFilters,
        process.pfCHS,
        process.jetsRedo,
        process.pfMuonIso,
        process.metCorrections,
        process.pathKappa,
    )

    return process


def addOutputModule(process, filename="test_out.root"):
    """Additional output file for testing.

       Do not use for a full skim, only for a few 100 events.
       Usage in cmssw config: process = base.addOutputModule(process)
    """
    process.Out = cms.OutputModule("PoolOutputModule",
         fileName = cms.untracked.string(filename)
    )
    process.end = cms.EndPath(process.Out)
    process.schedule.append(process.end)
    return process

if __name__ == "__main__":
    process = getBaseConfig('@GLOBALTAG@', datatype='@TYPE@')
