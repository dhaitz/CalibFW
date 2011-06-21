// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/JPTJet.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

// Root
#include "TList.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TParticle.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "TVector.h"

//
// class decleration
//
const int Z_pdgId=23;
const int g_pdgId=21;
const int u_pdgId=2;
const int d_pdgId=1;
const int s_pdgId=3;
const int c_pdgId=4;
const int b_pdgId=5;


typedef struct {float ChargedHadronEnergy, 
                ChargedHadronMultiplicity,
                ChargedHadronEnergyFraction,
                
                NeutralHadronEnergy,                        
                NeutralHadronMultiplicity,
                NeutralHadronEnergyFraction,
                
                ChargedEmEnergy,
                NeutralEmEnergy,

                NeutralMultiplicity,
                ChargedMultiplicity,

                ElectronEnergy,
                ElectronMultiplicity,
                ElectronEnergyFraction,
                
                MuonEnergy,
                MuonMultiplicity,
                MuonEnergyFraction,
                
                PhotonEnergy,
                PhotonMultiplicity,
                PhotonEnergyFraction,

                Constituents;} PFProperties;


class ZplusjetTreeMaker : public edm::EDAnalyzer {
   public:
    explicit ZplusjetTreeMaker(const edm::ParameterSet&);

    ~ZplusjetTreeMaker();

    /// Apply all the cuts to obtain our events
    template <class myjetsCollection>
    int  apply_cuts (
		    const edm::Event& iEvent,
		    const myjetsCollection& jets,
                    const reco::Candidate* z,
                    std::string algoname,
                    int& jet1_index,
                    int& jet2_index,
                    int& jet3_index,
                    int algo_counter=0,
                    bool is_calo=false);

    /// See if the muon and the jet axis are near
    /// and check wether the jet passes the jetId cuts
    bool is_valid_jet (const edm::Event& iEvent,
		       const reco::Jet* jet,
			const int jet_index,
                           const reco::Muon** z_muons,
                           bool is_calo,
			    std::string algoname);

    // extract the trigger information from the event
    void FillTriggerInfo(const edm::Event& iEvent,
                         const edm::EventSetup& iEventSetup);

    // extracts Reco Information
    void FillRecoVertices(const edm::Event& iEvent);
    // extracts BeamSpot Information
    void FillBeamSpot(const edm::Event& iEvent);

   private:
    virtual void beginJob() ;
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;

    /// Find the flavour of the balancing parton
    int m_balancing_parton_flavour(const edm::Event& iEvent);

    /// Fill the TParticle for the flat ntuple
    TParticle *  m_fill_tparticle( const reco::Candidate* recocandidate, bool is_jet=false);

    /// Get the calojet associated to the pfjet
    const reco::Candidate* m_get_calojet(const edm::Event& iEvent,std::string& algoname,reco::CandidateBaseRef& jetbaseref);

    /// Get the jptjet associated to the pfjet
    const reco::Candidate* m_get_jptjet(const edm::Event& iEvent,std::string& algoname,reco::CandidateBaseRef& jetbaseref);    
    
    /// Get the genjet associated to the recojet
    const reco::Candidate* m_get_genjet(const edm::Event& iEvent,std::string& algoname,reco::CandidateBaseRef& jetnaseref);    
    
    /// Get the gen Z associated to the reco Z
    TParticle* m_get_genZ(const edm::Event& iEvent,reco::CandidateBaseRef& zref);    
    
    /// Accep all algos if whitelist is empty, if not try matching
    bool acceptAlgo(const TString &names);

    /// Reco Z collection name
    std::string m_reco_zs_name;

    ///// Reco Z no iso collection name
    //std::string m_reco_zs_name_noiso;

    /// Gen Z collection name
    std::string m_gen_zs_name;

    /// White list algoritm keywords
    std::vector<std::string> m_algo_whitelist_vstring;
    
    /// White list algoritm keywords
    std::vector<std::string> m_algo_blacklist_vstring;
    
    // Output file

    /// Output plain file
    std::string m_output_filename;

    /// Output root file
    TFile* m_output_file;

    /// Get the properties of the pfproperties
    void m_get_pfproprerties(const reco::Candidate* recoparticle, PFProperties& propr);    
    
    /// Jet Correction Service
    std::string m_jetCorrectionService;
    
    // cuts

    /// Jet max eta
    double m_jetMaxEta;

    /// Jet-Muon min dr (Muon isolation)
    double m_zMuMinDr;

    /// 2nd leading jet/Z pt ratio
    double m_2ndJetZptRatio;

    /// Z Jet phi angle balancing
    double m_zJetPhiBalancing;

    // Flat ntuples

    /// Trees for the various gen algos
    TTree* m_gen_flat_trees;

    /// Trees for the various reco algos
    TTree* m_reco_flat_trees;

    // control

    /// Bool to register products only at the first event
    bool m_first_evt;

    /// To count the events processed
    double m_store_nevts;

    /// List of gen algo names
    TList m_gen_jetalgonames;

    /// List of reco algo names
    TList m_reco_jetalgonames;

    /// Muon is seed counter
    bool m_muon_is_seed;

    /// Total mu cut passed
    unsigned int m_Z_tot;

    /// Total mu iso cut passed
    unsigned int m_iso_Z_tot;

    /// Total pt ratio cut passed
    unsigned int* m_pt_ratio_tot_v;

    /// Total delta phi cut passed
    unsigned int* m_deltaphi_tot_v;

    /// Total eta cut passed
    unsigned int* m_eta_tot_v;

    /// contains the name > strings of all fired HLT triggers
    TClonesArray* m_HLTriggers_fired;
    
    // contains the position of the recontructed vertices
    TClonesArray* m_reco_vertices;

    // contains the error of the recontructed vertices
    TClonesArray* m_reco_vertices_error;

    // contains the Chi2, Ndof, nTracks of the reco vertices
    TClonesArray* m_reco_vertices_info;

    TVector3 * m_beamSpot;


    edm::InputTag m_triggerResultsTag;   


    /// contains strings to map Calo jet algo names to Calo JetId names
    std::map< std::string , std::string > m_caloJetIdMapping;

      // ----------member data ---------------------------
public:
	// this members are linked to a TTree in a Branch
int m_store_cmsEventNum;
int m_store_cmsRun;
int m_store_luminosityBlock;
int m_store_matrix_element_flavour;
double m_store_weight;

double m_store_filtereff;
double m_store_xsection;
double m_store_eventIndex;

TParticle * m_store_Z_part;
TParticle * m_store_mu_plus_part;
TParticle * m_store_mu_minus_part;
TParticle * m_store_jet_part;
TParticle * m_store_jet_part3;
TParticle * m_store_jet_part2;

TParticle * m_store_matched_calo_jet_part;
TParticle * m_store_matched_calo_jet_part2;
TParticle * m_store_matched_calo_jet_part3;

TParticle * m_store_matched_gen_jet_part;
TParticle * m_store_matched_gen_jet_part2;
TParticle * m_store_matched_gen_jet_part3;

TParticle * m_store_matched_jpt_jet_part;
TParticle * m_store_matched_jpt_jet_part2;
TParticle * m_store_matched_jpt_jet_part3;

TParticle * m_store_matched_gen_Z_part;

TParticle * m_store_met_part;
TParticle * m_store_tcmet_part;
TObjString * m_store_algoname_tree;

PFProperties m_pfjet_part1_properties;
PFProperties m_pfjet_part2_properties;
PFProperties m_pfjet_part3_properties;


};
