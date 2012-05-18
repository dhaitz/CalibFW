import FWCore.ParameterSet.Config as cms

## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *

process = cms.Process("jectxt")

from CondCore.DBCommon.CondDBSetup_cfi import *

# define your favorite global tag and a list of algorithms
globaltag = 'GR_R_44_V12' #'Jec12_V7'
sqlitefile = 'Jec12_V7.db'
use_gt = True
algorithms = ['AK5PFchs', "AK5PF"]
jecversion = "JetCorrectorParametersCollection_Jec11_V7"

# if you want to load custom corrections
thelist = cms.VPSet()
for algo in algorithms:
    thelist.append(cms.PSet(
        record = cms.string("JetCorrectionsRecord"),
        tag = cms.string(jecversion + "_" + algo),
        label = cms.untracked.string(algo)
    ))
if use_gt:
    process.jec = cms.ESSource("PoolDBESSource", CondDBSetup,
        connect = cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS"),
        toGet = thelist,
    )
    process.load('Configuration.StandardSequences.Services_cff')
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
    process.GlobalTag.globaltag = globaltag + '::All'
else:
    process.jec = cms.ESSource("PoolDBESSource",
        DBParameters = cms.PSet(messageLevel = cms.untracked.int32(0)),
        timetype = cms.string('runnumber'),
        connect = cms.string('sqlite:'+sqlitefile),
        toGet = thelist
)

process.es_prefer_jec = cms.ESPrefer("PoolDBESSource", "jec")

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
