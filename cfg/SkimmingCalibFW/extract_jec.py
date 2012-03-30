import FWCore.ParameterSet.Config as cms

## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *

process = cms.Process("jectxt")

from CondCore.DBCommon.CondDBSetup_cfi import *

# define your favorite global tag and a list of algorithms
globaltag = 'GR_R_44_V13'
algorithms = ['AK5PF']
jecversion = "JetCorrectorParametersCollection_Jec11_V12"

# if you want to load custom corrections
process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
    connect = cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS"),
    toGet = cms.VPSet(
        cms.PSet(
            record = cms.string("JetCorrectionsRecord"),
            tag = cms.string(jecversion + "_KT6PF"),
            label = cms.untracked.string("KT6PF")),
        cms.PSet(
            record = cms.string("JetCorrectionsRecord"),
            tag = cms.string(jecversion + "_AK5PF"),
            label = cms.untracked.string("AK5PF")),
        cms.PSet(
            record = cms.string("JetCorrectionsRecord"),
            tag = cms.string(jecversion + "_AK5PFchs"),
            label = cms.untracked.string("AK5PFchs")),
        cms.PSet(
            record = cms.string("JetCorrectionsRecord"),
            tag = cms.string(jecversion + "_AK5Calo"),
            label = cms.untracked.string("AK5Calo")),
        cms.PSet(
            record = cms.string("JetCorrectionsRecord"),
            tag = cms.string(jecversion + "_AK5JPT"),
            label = cms.untracked.string("AK5JPT")),
    )
)
process.es_prefer_jec = cms.ESPrefer("PoolDBESSource", "jec")


process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = globaltag + '::All'

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")

# set the Analyzer for the first algorithm
setattr(process, 'read' + algorithms[0],
    cms.EDAnalyzer('JetCorrectorDBReader',
        # below is the communication to the database
        payloadName    = cms.untracked.string(algorithms[0]),
        # this is used ONLY for the name of the printed txt files.
        # You can use any name that you like, but it is recommended to use
        # the GT name that you retrieved the files from.
        globalTag      = cms.untracked.string(globaltag),
        printScreen    = cms.untracked.bool(False),
        createTextFile = cms.untracked.bool(True),
    )
)

pathlist = getattr(process, 'read' + algorithms[0])

# do the other algorithms
for algo in algorithms[1:]:
    setattr(process, 'read' + algo,
        getattr(process, 'read' + algorithms[0]).clone(payloadName = algo))
    pathlist *= getattr(process, 'read' + algo)

process.p = cms.Path(pathlist)
process.schedule = cms.Schedule(process.p)
