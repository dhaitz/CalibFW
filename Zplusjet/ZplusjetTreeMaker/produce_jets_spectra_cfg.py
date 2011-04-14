import FWCore.ParameterSet.Config as cms

# Meta configuration |----------------------------------------------------------

# input files

files_dir='/afs/cern.ch/user/d/dpiparo/localscratch/ZplusJet09/ImportDesy/Skim/Summer09/Summer09-MC_31X_V2_preproduction_311_AODSIM'
files_list=[]

rootnames=\
["ZmumuJet_Pt80to120_5_skim.root",
"ZmumuJet_Pt80to120_6_skim.root",
"ZmumuJet_Pt80to120_7_skim.root",
"ZmumuJet_Pt80to120_8_skim.root",
"ZmumuJet_Pt80to120_9_skim.root"]

for i in rootnames:
    files_list.append('file:%s/%s' %(files_dir,i))

events_to_process=10

calojetcollections = ["iterativeCone5CaloJets",
                       "kt4CaloJets",
                       "kt6CaloJets",
                       "sisCone5CaloJets",
                       "sisCone7CaloJets"]
genjetcollections = ["iterativeCone5GenJets","kt4GenJets","kt6GenJets","sisCone5GenJets","sisCone7GenJets"]

# Meta configuration |----------------------------------------------------------

# Configuration |---------------------------------------------------------------

process = cms.Process("Zplusjet2009controlplots")

p = process # shortcut!

p.source = cms.Source("PoolSource",
                      fileNames = cms.untracked.vstring(files_list))

# The max events number to process ---------------------------------------------

p.maxEvents=cms.untracked.PSet(input = cms.untracked.int32(events_to_process))

#-------------------------------------------------------------------------------

# The test histos --------------------------------------------------------------

p.TFileService = cms.Service ("TFileService",
                            fileName = cms.string("J_test_histos.root"))

caloJ_pt=cms.PSet(min = cms.untracked.double(0),
                  max = cms.untracked.double(1000), 
                  nbins =cms.untracked.int32(500),
                  description = cms.untracked.string("Calo J transverse momentum [GeV/c]"),
                  name = cms.untracked.string("CaloJpt"),
                  plotquantity = cms.untracked.string("pt"))

genJ_pt=cms.PSet(min = cms.untracked.double(0),
                 max = cms.untracked.double(1000), 
                 nbins =cms.untracked.int32(500),
                 description = cms.untracked.string("Gen J transverse momentum [GeV/c]"),
                 name = cms.untracked.string("GenJpt"),
                 plotquantity = cms.untracked.string("pt"))

p.schedule=cms.Schedule()

for calojetcollection,genjetcollection in zip (calojetcollections,genjetcollections):

    caloattrname="histsmaker%s" %calojetcollection
    setattr(p,caloattrname,cms.EDAnalyzer("CandViewHistoAnalyzer",
                           src = cms.InputTag(calojetcollection),
                           histograms = cms.VPSet(caloJ_pt)))
    setattr(p,caloattrname+"path",cms.Path(getattr(p,caloattrname)))
    p.schedule.append(getattr(p,caloattrname+"path"))

    genattrname="histsmaker%s" %genjetcollection
    setattr(p,genattrname,cms.EDAnalyzer("CandViewHistoAnalyzer",
                           src = cms.InputTag(genjetcollection),
                           histograms = cms.VPSet(genJ_pt)))
    setattr(p,genattrname+"path",cms.Path(getattr(p,genattrname)))

    p.schedule.append(getattr(p,genattrname+"path"))
