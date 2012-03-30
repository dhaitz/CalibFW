import FWCore.ParameterSet.Config as cms

## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *

process = cms.Process("jectxt")

from CondCore.DBCommon.CondDBSetup_cfi import *

# if you want to load custom corrections
process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
                           connect = cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS"),
                           toGet =  cms.VPSet(
                            cms.PSet(record = cms.string("JetCorrectionsRecord"),
                                     tag = cms.string("JetCorrectorParametersCollection_Jec11_V12_AK5Calo"),
                                    label= cms.untracked.string("AK5Calo")),
                            cms.PSet(record = cms.string("JetCorrectionsRecord"),
                                    tag = cms.string("JetCorrectorParametersCollection_Jec11_V12_AK5PF"),
                                    label=cms.untracked.string("AK5PF")),
                            cms.PSet(record = cms.string("JetCorrectionsRecord"),
                                    tag = cms.string("JetCorrectorParametersCollection_Jec11_V12_AK5PFchs"),
                                    label=cms.untracked.string("AK5PFchs")),
                            cms.PSet(record = cms.string("JetCorrectionsRecord"),
                                     tag = cms.string("JetCorrectorParametersCollection_Jec11_V12_AK5JPT"),
                                     label=cms.untracked.string("AK5JPT")),
                              )
                        )
process.es_prefer_jec = cms.ESPrefer("PoolDBESSource","jec")



process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
# define your favorite global tag
#process.GlobalTag.globaltag = 'GR_R_42_V19::All'


#the_global_tag =  'MC_42_V14B'
the_global_tag =  'START44_V12'


process.GlobalTag.globaltag = the_global_tag + "::All"

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")

process.readAK5PF    = cms.EDAnalyzer('JetCorrectorDBReader',  
# below is the communication to the database 
payloadName    = cms.untracked.string('AK5PFchs'),
	# this is used ONLY for the name of the printed txt files. You can use any name that you like, 
	# but it is recommended to use the GT name that you retrieved the files from.
	globalTag      = cms.untracked.string( the_global_tag ),  
	printScreen    = cms.untracked.bool(False),
	createTextFile = cms.untracked.bool(True)
)
#  process.readAK5Calo = process.readAK5PF.clone(payloadName = 'AK5Calo')
#  process.readAK5JPT = process.readAK5PF.clone(payloadName = 'AK5JPT')
#  process.p = cms.Path(process.readAK5PF * process.readAK5Calo * process.readAK5JPT)

#process.readAK7PF = process.readAK5PF.clone(payloadName = 'AK7PF')

#process.readKT4PF = process.readAK5PF.clone(payloadName = 'KT4PF')
#process.readKT6PF = process.readAK5PF.clone(payloadName = 'KT6PF')

process.p = cms.Path(process.readAK5PF )
#* 
#	process.readAK7PF * 
#	process.readKT4PF * 
#	process.readKT6PF )
