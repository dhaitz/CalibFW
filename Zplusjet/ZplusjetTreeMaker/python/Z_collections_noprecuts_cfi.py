import FWCore.ParameterSet.Config as cms

# HLT Trigger filer

# dont use this trigger for offline, hlt trigger infos are written to output root files
#import copy
#from HLTrigger.HLTfilters.hltHighLevel_cfi import * 
#hltSelection = copy.deepcopy(hltHighLevel)
#hltSelection.throw = cms.bool(True)
#hltSelection.HLTPaths = ["HLT_Mu9"]

mu_pt= 12
mu_iso = 3.0
mu_eta = 8

goodMuons_cut="pt > %s && "%mu_pt +\
              "isGlobalMuon && " +\
              "isolationR03.sumPt <%s && " %mu_iso +\
              "abs(eta) < %s " %mu_eta

Z_mass_cut = "60.0 < mass < 120.0"

# Reco Zs
goodMuonsReco = cms.EDFilter("MuonSelector",
              src = cms.InputTag("muons"),
              cut = cms.string(goodMuons_cut))

goodZToMuMuReco = cms.EDProducer("CandViewShallowCloneCombiner",
    cut = cms.string(Z_mass_cut),
    decay = cms.string('goodMuonsReco@+ goodMuonsReco@-'))

goodZToMuMuRecoPath = cms.Path( goodMuonsReco * goodZToMuMuReco )

#Reco Zs no iso
# not used at the moment
mu_iso=100000
goodMuons_cut_noiso="pt > %s && "%mu_pt +\
              "isGlobalMuon && " +\
              "isolationR03.sumPt <%s && " %mu_iso +\
              "abs(eta) < %s " %mu_eta

goodMuonsReconoiso = cms.EDFilter("MuonSelector",
              src = cms.InputTag("muons"),
              cut = cms.string(goodMuons_cut_noiso))

goodZToMuMuReconoiso = cms.EDProducer("CandViewShallowCloneCombiner",
                    cut = cms.string(Z_mass_cut),
                    decay = cms.string('goodMuonsReconoiso@+ goodMuonsReconoiso@-'))


#Gen Zs
goodMuonsGen = cms.EDFilter("PdgIdAndStatusCandViewSelector",
                status = cms.vint32(1),
                src = cms.InputTag("genParticles"),
                pdgId = cms.vint32(13,-13))

goodZToMuMuGen = cms.EDProducer("CandViewShallowCloneCombiner",
                cut = cms.string(Z_mass_cut),
                decay = cms.string('goodMuonsGen@+ goodMuonsGen@-'))

goodZToMuMuGenPath = cms.Path(goodMuonsGen*goodZToMuMuGen)


# contains all events, coming from reco + gen
# use this path for further analysis
allgoodZToMuMuPath = cms.Path( (goodMuonsReco*goodZToMuMuReco) + (goodMuonsGen*goodZToMuMuGen))


print "Cuts are:"
print " * Muons: ",goodMuons_cut 
print " * Z: " , Z_mass_cut
