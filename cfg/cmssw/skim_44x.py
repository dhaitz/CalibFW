import FWCore.ParameterSet.Config as cms

def getBaseConfig(globaltag, testfile="", maxevents=300, datatype='mc'):
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
        maxevents = 800
    else:
        testfile = 'file:/storage/6/berger/testfiles/mc_madgraphV9_AOD.root'
        if '@' in globaltag: globaltag = 'START53_V6'
        datatype = 'mc'
    print "GT:", globaltag, "| TYPE:", datatype

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

    # Produce rho distribution-------------------------------------------------
    process.load('RecoJets.JetProducers.kt4PFJets_cfi')
    process.kt6PFJetsRho = process.kt4PFJets.clone(
        rParam = cms.double(0.6),
        doRhoFastjet = cms.bool(True)
    )
    process.kt6PFJetsRho.Rho_EtaMax = cms.double(5.0)
    process.jetArea = cms.Path(process.kt6PFJetsRho)

    # enable area calculation for all algos used ------------------------------
    process.ak5PFJets.doAreaFastjet = cms.bool(True)
    process.ak7PFJets.doAreaFastjet = cms.bool(True)
    process.kt4PFJets.doAreaFastjet = cms.bool(True)
    process.kt6PFJets.doAreaFastjet = cms.bool(True)

    # CHS Jets with the NoPU sequence -----------------------------------------
    process.load('CommonTools.ParticleFlow.pfParticleSelection_cff')
    process.load('CommonTools.ParticleFlow.pfNoPileUp_cff')
    # switching back to the regular PF input in 44x as PF2PAT does
    # 'PF2PAT+PAT is probably doing the same workaround' (Colin Bernet)
    # cf. CommonTools/ParticleFlow/python/PF2PAT_cff.py
    process.pfPileUp.PFCandidates = cms.InputTag('particleFlow')
    process.pfNoPileUp.bottomCollection = cms.InputTag('particleFlow')
    process.pfCHS = cms.Path(process.pfNoPileUpSequence)

    process.ak5PFJetsCHS = process.ak5PFJets.clone( src = cms.InputTag('pfNoPileUp') )
    process.ak7PFJetsCHS = process.ak7PFJets.clone( src = cms.InputTag('pfNoPileUp') )
    process.kt4PFJetsCHS = process.kt4PFJets.clone( src = cms.InputTag('pfNoPileUp') )
    process.kt6PFJetsCHS = process.kt6PFJets.clone( src = cms.InputTag('pfNoPileUp') )

    # Path to Redo all Jets
    process.jetsRedo = cms.Path(
        process.ak5PFJets * process.ak7PFJets * process.kt4PFJets * process.kt6PFJets *
        process.ak5PFJetsCHS * process.ak7PFJetsCHS * process.kt4PFJetsCHS * process.kt6PFJetsCHS
    )


    # Require two good muons --------------------------------------------------
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
        outputFile = cms.string("skim_"+datatype+".root")
    )
    process.kappatuple.verbose = cms.int32(0)
    process.kappatuple.active = cms.vstring(
        'LV', 'L1Muons', 'Muons', 'TrackSummary', 'VertexSummary', 'BeamSpot',
        'JetArea', 'PFMET', 'PFJets',
    )
    if data:
        additional_actives = ['DataMetadata', 'TriggerObjects']
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

    # use the jets created during the kappa skim and not the RECO Jet
    process.kappatuple.PFJets.whitelist = cms.vstring("recoPFJets.*kappaSkim")
    process.pathKappa = cms.Path(
        process.goodMuons * process.twoGoodMuons * process.kappatuple
    )

    # Process schedule --------------------------------------------------------
    process.schedule = cms.Schedule(
            process.pfCHS,
            process.jetArea,
            process.jetsRedo,
            process.pfMuonIso,
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
