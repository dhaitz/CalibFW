import FWCore.ParameterSet.Config as cms

process = cms.Process("OWNPARTICLES")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

files_dir='/home/ws/bd103/scartch/Zmumu/'
files_list=['file:%s/06029757-B588-DD11-BDD7-001CC4AA8E08.root' %files_dir,
            'file:%s/0807444B-B388-DD11-B593-001CC445D6D2.root' %files_dir]

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        files_list[0]
    )
)

process.myProducerLabel = cms.EDProducer('EventCounter',
    indexLabel=cms.untracked.string("processedEvents")
    )

process.p = cms.Path(process.myProducerLabel)

#process.e = cms.EndPath(process.out)
