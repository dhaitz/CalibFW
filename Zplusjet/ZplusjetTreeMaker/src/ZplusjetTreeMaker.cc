// -*- C++ -*-
//
// Package:    ZplusjetTreeMaker
// Class:      ZplusjetTreeMaker
// 
/**\class ZplusjetTreeMaker ZplusjetTreeMaker.cc Zplusjet/ZplusjetTreeMaker/src/ZplusjetTreeMaker.cc

 Description: Mark an event as good or bad Z+jet event according to a certain algo

 Implementation:

A set of cuts is applied to the event.
We work on the collection of Z candidates according to the EWK group
(https://twiki.cern.ch/twiki/bin/view/CMS/DimuonsSWRecipes)

The cuts are
1) Muon pt greater than m_muonMinPt and Muon eta smaller than m_muonMaxEta
2) DeltaR muon-Z greater than m_zMuMinDr *
3) Leading jet |eta| smaller than m_jetMaxEta
4) second_leading_jet_Pt/Z_Pt smaller than m_2ndJetZptRatio
5) DeltaPhi (Jet,Z) smaller than m_zJetPhiBalancing

* this cut does not cause the event to be skipped but it forces the tool to
seek for another leading jet. Indeed a bug of cmssw makes the muons to be
considered jets. The new collections thanks to the work of A. Oehler and
K. rabbertz, in CMSSW 2 this should not happen anymore. Keep this cut for
cross checks.

The idea is to loop on all the jet algorithms present in the file.
For each algo, there is an iteration on the Zs.
As soon as a match Z-Jet is found the associated TTree is filled.

*/
//
// Original Author:  DaniloPiparo
//         Created:  Wed Jan  7 08:25:28 CET 2009
// $Id: ZplusjetTreeMaker.cc,v 1.43 2011/03/08 19:10:47 dpiparo Exp $
//
//

#include "Zplusjet/ZplusjetTreeMaker/interface/ZplusjetTreeMaker.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"


#include "DataFormats/Candidate/interface/CandMatchMap.h"


#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/JetID.h"

#include <DataFormats/METReco/interface/MET.h>
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/CaloMET.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include <FWCore/Common/interface/TriggerNames.h>
#include <DataFormats/Common/interface/TriggerResults.h>
#include <DataFormats/HLTReco/interface/TriggerEvent.h>
#include <DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h>
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

// Root includes
#include "TObjString.h"
#include "TIterator.h"
#include "TMath.h"
#include "Math/GenVector/VectorUtil.h"
#include "TH1F.h"
//
// constants, enums and typedefs
//

//
// static data member definitions
//
// TString struct_descr("ChargedHadronEnergy: ChargedHadronMultiplicity:NeutralHadronEnergy:NeutralHadronMultiplicity:ChargedEmEnergy:NeutralEmEnergy:NeutralMultiplicity:ChargedMultiplicity:ElectronEnergy:ElectronMultiplicity:MuonEnergy:MuonMultiplicity:PhotonEnergy:PhotonMultiplicity:Constituents");
TString struct_descr("ChargedHadronEnergy:ChargedHadronMultiplicity:ChargedHadronEnergyFraction:NeutralHadronEnergy:NeutralHadronMultiplicity:NeutralHadronEnergyFraction:ChargedEmEnergy:NeutralEmEnergy:NeutralMultiplicity:ChargedMultiplicity:ElectronEnergy:ElectronMultiplicity:ElectronEnergyFraction:MuonEnergy:MuonMultiplicity:MuonEnergyFraction:PhotonEnergy:PhotonMultiplicity:PhotonEnergyFraction:Constituents");

//------------------------------------------------------------------------------

ZplusjetTreeMaker::ZplusjetTreeMaker(const edm::ParameterSet& iConfig) {

    m_jetCorrectionService = iConfig.getParameter<std::string> ("JetCorrectionService");
//    m_correctionLevels = iConfig.getParameter<std::vector <int> > ("correctionLevels");

    // Muon jets cuts
    m_zMuMinDr = iConfig.getParameter<double> ("zMuMinDr");

    // Jets cuts
    m_jetMaxEta = iConfig.getParameter<double> ("jetMaxEta");

    // Z jets cuts
    m_2ndJetZptRatio = iConfig.getParameter<double> ("secondJetZptRatio");
    m_zJetPhiBalancing = iConfig.getParameter<double> ("zJetPhiBalancing");

    // Names of Z collections
    m_reco_zs_name = iConfig.getParameter<std::string> ("reco_zs_name");
    //m_reco_zs_name_noiso = iConfig.getParameter<std::string> ("reco_zs_name_noiso");
    m_gen_zs_name = iConfig.getParameter<std::string> ("gen_zs_name");

    // Outputfilename
    m_output_filename = iConfig.getUntrackedParameter<std::string>
                        ("output_filename","plain_Zplusjettree.root");

    // Trigger config
    m_triggerResultsTag = iConfig.getUntrackedParameter<edm::InputTag>("triggerResultsTag");


    // Open outfile
    m_output_file = new TFile (m_output_filename.c_str(),"RECREATE");
    m_output_file->cd();

    // Algos whitelist vstring
    m_algo_whitelist_vstring = iConfig.getUntrackedParameter<std::vector <std::string> >
                               ("algo_whitelist_vstring");

    m_algo_blacklist_vstring = iConfig.getUntrackedParameter<std::vector <std::string> >
                               ("algo_blacklist_vstring");                               
                               
    // Control Members
    m_store_nevts=0;
    m_first_evt=true;
    m_muon_is_seed=0;

    m_caloJetIdMapping.insert( std::pair< std::string, std::string> ("ak5CaloJets","ak5JetID") );
    m_caloJetIdMapping.insert( std::pair< std::string, std::string> ("ak7CaloJets","ak7JetID") );
    m_caloJetIdMapping.insert( std::pair< std::string, std::string> ("kt4CaloJets","kt4JetID") );
    m_caloJetIdMapping.insert( std::pair< std::string, std::string> ("kt6CaloJets","kt6JetID") );
    m_caloJetIdMapping.insert( std::pair< std::string, std::string> ("iterativeCone5CaloJets","ic5JetID") );
    
    m_caloJetIdMapping.insert( std::pair< std::string, std::string> ("ak5CaloJetsL1","ak5CaloJetsL1ID") );
    m_caloJetIdMapping.insert( std::pair< std::string, std::string> ("ak5CaloJetsL1L2","ak5CaloJetsL1L2ID") );
    m_caloJetIdMapping.insert( std::pair< std::string, std::string> ("ak5CaloJetsL1L2L3Res","ak5CaloJetsL1L2L3ResID"));
}

//------------------------------------------------------------------------------

ZplusjetTreeMaker::~ZplusjetTreeMaker() {

    LogDebug("TreeOutput") <<  "Starting Deconstruct";

    m_output_file->cd();
    // Write the trees
    LogDebug("TreeOutput") << "writing trees";
    LogDebug("TreeOutput") << "m_gen_jetalgonames.GetSize() = " << m_gen_jetalgonames.GetSize();
    LogDebug("TreeOutput") << "sizeof( m_gen_flat_trees ) " << sizeof(m_gen_flat_trees);


    if ( m_gen_jetalgonames.GetSize() == 0 )
    {
        edm::LogWarning("TreeOutput") << "No Gen jets available, skipping";
    }
    else
    {
        for (int i=0; i < m_gen_jetalgonames.GetSize() + 1;++i) {
            // Add the total number of evts processed
            LogDebug("TreeOutput") << "AlgoName " << m_gen_jetalgonames.At(i);
            LogDebug("TreeOutput") << "creating branch #" << i;
            LogDebug("TreeOutput") << &m_gen_flat_trees[i];

            TBranch *newBranch=m_gen_flat_trees[i].Branch("eventsProcessed",
                               &m_store_nevts,
                               "eventsProcessed/D");
            LogDebug("TreeOutput") << "branch created";
            Long64_t nentries = m_gen_flat_trees[i].GetEntries();
            LogDebug("TreeOutput") << "filling branch";

            for (Long64_t j = 0; j < nentries; j++)
                newBranch->Fill();

            LogDebug("TreeOutput") << "Writing " << m_gen_flat_trees[i].GetName();

            m_output_file->cd();
            m_gen_flat_trees[i].Write();
        }
    }

    LogDebug("TreeOutput") << "writing branches";
    for (int i=0;i<m_reco_jetalgonames.GetSize()+1;++i) {
        TBranch *newBranch=m_reco_flat_trees[i].Branch("eventsProcessed",
                           &m_store_nevts,
                           "eventsProcessed/D");
        TBranch *pt_ratio_tot_branch
        =m_reco_flat_trees[i].Branch("pt_ratio_tot",
                                     &m_pt_ratio_tot_v[i],
                                     "pt_ratio_tot/i");
        TBranch *iso_Z_tot_branch
        =m_reco_flat_trees[i].Branch("iso_Z_tot",
                                     &m_iso_Z_tot,
                                     "iso_Z_tot/i");
        TBranch *Z_tot_branch
        =m_reco_flat_trees[i].Branch("Z_tot",
                                     &m_Z_tot,
                                     "Z_tot/i");

        TBranch *deltaphi_tot_branch
        =m_reco_flat_trees[i].Branch("deltaphi_tot",
                                     &m_deltaphi_tot_v[i],
                                     "deltaphi_tot/i");

        TBranch *eta_tot_branch
        =m_reco_flat_trees[i].Branch("eta_tot",
                                     &m_eta_tot_v[i],
                                     "eta_tot/i");


        Long64_t nentries = m_reco_flat_trees[i].GetEntries();
        for (Long64_t j = 0; j < nentries;j++) {
            newBranch->Fill();
            Z_tot_branch->Fill();
            iso_Z_tot_branch->Fill();
            pt_ratio_tot_branch->Fill();
            deltaphi_tot_branch->Fill();
            eta_tot_branch->Fill();
        }
        LogDebug("TreeOutput") << "Writing " << m_reco_flat_trees[i].GetName();
        m_output_file->cd();
        m_reco_flat_trees[i].Write();
    }

    LogDebug("TreeOutput") << "Debug: writing algos";
    // Write the algo names list
    m_gen_jetalgonames.Write("gen_jetalgonames", TObject::kSingleKey);
    m_reco_jetalgonames.Write("reco_jetalgonames", TObject::kSingleKey);

    LogDebug("TreeOutput") << "Debug: writing histo";
    // Histo to save the total number of evts
    TH1F h("number_of_processed_events",
           "number_of_processed_events",
           11,m_store_nevts-5,m_store_nevts+5);
    h.Fill(m_store_nevts);
    h.Write();

    m_output_file->Close();
    delete m_output_file;
}

//------------------------------------------------------------------------------


void ZplusjetTreeMaker::analyze(const edm::Event& iEvent,
                                const edm::EventSetup& iSetup) {

//     std::cout << " **** ANALIZE\n";

//    const JetCorrector* corrector = JetCorrector::getJetCorrector("ak5PFL1FastL2L3",iSetup);
//    
//    edm::RefToBase<reco::Jet> jetRef(edm::Ref<JetCollection>(jets,index));
//    double jec = corrector->correction(*i_jet,jetRef,iEvent,iSetup);


    using namespace edm;

    m_output_file->cd();

    Handle< double > events_processed;
    // this entry has been written by our very own EventCounter
    iEvent.getByLabel("processedEvents","processedEvents",events_processed);

    LogInfo ("processedEvents") << " --> Events Processed: "
    << *events_processed << std::endl;
    m_store_nevts=*events_processed;

    // Event num and Id
    m_store_cmsEventNum=iEvent.id().event();
    m_store_cmsRun=iEvent.id().run();
    m_store_luminosityBlock = iEvent.luminosityBlock();

    LogDebug ("EventMeta") << "Event Number:" << m_store_cmsEventNum
                           << " run number:" << m_store_cmsRun
                           << " lumi block:" << m_store_luminosityBlock;

    // The it geninfoproduct ---------------------------------------------------
    m_store_weight=-1.23;
    double binningValue=0;
    try {
        edm::Handle<GenEventInfoProduct> genInfoProd;
        bool is_mc = iEvent.getByLabel( "generator", genInfoProd );
        if (is_mc) m_store_weight=genInfoProd->weight();
        // Info on pt_hat aka. hard scatter
        if (is_mc) binningValue=genInfoProd->binningValues()[0];
        //hweight->Fill(weight);
    }
    catch (...) {
        LogPrint ("GenEventInfoProduct") << "No GenEventInfoProduct found!";
    }

    // The run info product
    m_store_xsection=1;
    m_store_filtereff=1;
//    try {
//        edm::Handle<GenRunInfoProduct> genRunInfo;
//        iEvent.getRun().getByLabel( "generator", genRunInfo );
//        m_store_xsection=genRunInfo->crossSection();
//        m_store_filtereff=genRunInfo->filterEfficiency();
//    }
//    catch (...) {
//        LogPrint ("GenRunInfoProduct") << "No GenRunInfoProduct found!";
//    }

    m_store_matrix_element_flavour=0;

    if (m_first_evt) {


        // first init of storage variables
        m_HLTriggers_fired = new TClonesArray("TObjString");

        m_reco_vertices = new TClonesArray("TVector3");
        m_reco_vertices_error = new TClonesArray("TVector3");
        m_reco_vertices_info = new TClonesArray("TVector3");

        m_beamSpot = new TVector3;

        m_store_Z_part = 0;
        m_store_matched_gen_Z_part=0;
        m_store_mu_plus_part= 0;
        m_store_mu_minus_part= 0;
        m_store_jet_part= 0;
        m_store_jet_part3= 0;
        m_store_jet_part2= 0;

        m_store_matched_calo_jet_part= 0;
        m_store_matched_calo_jet_part2= 0;
        m_store_matched_calo_jet_part3= 0;

        m_store_matched_jpt_jet_part= 0;
        m_store_matched_jpt_jet_part2= 0;
        m_store_matched_jpt_jet_part3= 0;
        
        m_store_matched_gen_jet_part= 0;
        m_store_matched_gen_jet_part2= 0;
        m_store_matched_gen_jet_part3= 0;        
        
        m_store_met_part= 0;
        m_store_tcmet_part= 0;
        m_store_genmet_part= 0;
        m_store_algoname_tree = 0;

        std::string name="";
        std::string procname="";

        // Gen Jets
        std::vector< Handle<std::vector<reco::GenJet> > > genjetsvec;
        iEvent.getManyByType(genjetsvec);

        // hash map for gen jets
        for (unsigned int i=0;i<genjetsvec.size();++i) {
            name=iEvent.getProvenance(genjetsvec[i].id()).moduleLabel();
            procname=iEvent.getProvenance(genjetsvec[i].id()).processName();
            TString names(name.c_str());
            if (not acceptAlgo(names))
                continue;
            LogInfo ("GenJetAlgos") << procname << " " << name << std::endl;
            TObjString* s = new TObjString(name.c_str());
            if (not m_gen_jetalgonames.Contains(s)) {
                m_gen_jetalgonames.Add(s);
                LogInfo("Algorithms") << "Adding algorithm " << s->String().Data();
            }
        }

        m_gen_flat_trees = new TTree[m_gen_jetalgonames.GetSize()+1];

        TIterator* iter =m_gen_jetalgonames.MakeIterator();
        int gen_algo_counter=0;
        while (TObjString *objstr = (TObjString*) iter->Next()) {
            TTree* gentree=&(m_gen_flat_trees[gen_algo_counter]);


            TString tree_name(objstr->String());
            tree_name+="_Zplusjet";
            // Set the name-title of the tree <algoname>_Zplusjet
            gentree->SetName(tree_name.Data());
            gentree->SetTitle(tree_name.Data());
            LogInfo ("test") << "tree: " << gentree->GetName() << " " << gentree->GetTitle()<< std::endl;

            // Set the branches of the tree
            gentree->Branch("Z","TParticle",&m_store_Z_part);
            gentree->Branch("mu_plus","TParticle",&m_store_mu_plus_part);
            gentree->Branch("mu_minus","TParticle",&m_store_mu_minus_part);
            gentree->Branch("jet","TParticle",&m_store_jet_part);
            gentree->Branch("jet2","TParticle",&m_store_jet_part2);
            gentree->Branch("jet3","TParticle",&m_store_jet_part3);

            gentree->Branch("met","TParticle",&m_store_met_part);
            gentree->Branch("genmet","TParticle",&m_store_genmet_part);

            gentree->Branch("algoname","TObjString",&m_store_algoname_tree);
            gentree->Branch("eventIndex",&m_store_nevts,"eventsProcessed/D");
            gentree->Branch("xsection",&m_store_xsection,"xsection/D");
            gentree->Branch("filtereff",&m_store_filtereff,"filtereff/D");
            gentree->Branch("cmsEventNum",&m_store_cmsEventNum,"cmsEventNum/I");
            gentree->Branch("cmsRun",&m_store_cmsRun,"cmsRun/I");
            gentree->Branch("luminosityBlock",&m_store_luminosityBlock,"luminosityBlock/I");

            gentree->Branch("matrix_element_flavour",&m_store_matrix_element_flavour,"matrix_element_flavour/I");
            gentree->Branch("weight",&m_store_weight,"weight/D");

//             gentree->Branch("iso_Z_tot",&m_iso_Z_tot_v[reco_algo_counter],"iso_Z_cut_tot/i");
//             gentree->Branch("pt_ratio_tot",&m_pt_ratio_tot_v[reco_algo_counter],"pt_ratio_tot/i");
//             gentree->Branch("deltaphi_tot",&m_deltaphi_tot_v[reco_algo_counter],"deltaphi_tot/i");
//             gentree->Branch("eta_tot",&m_eta_tot_v[reco_algo_counter],"eta_tot/O");

            gen_algo_counter++;
        }

        delete iter;

        // Pure Z w/o balancing Jet (GEN)
        TTree* gentree=&(m_gen_flat_trees[m_gen_jetalgonames.GetSize()]);
        TString tree_name("Zonly_gen");
        gentree->SetName(tree_name.Data());
        gentree->SetTitle(tree_name.Data());
        LogInfo ("test") << "tree: " << gentree->GetName() << " " << gentree->GetTitle()<< std::endl;

        gentree->Branch("Z","TParticle",&m_store_Z_part);
        

        gentree->Branch("mu_plus","TParticle",&m_store_mu_plus_part);
        gentree->Branch("mu_minus","TParticle",&m_store_mu_minus_part);

        gentree->Branch("eventIndex",&m_store_nevts,"eventsProcessed/D");
        gentree->Branch("xsection",&m_store_xsection,"xsection/D");
        gentree->Branch("filtereff",&m_store_filtereff,"filtereff/D");
        gentree->Branch("cmsEventNum",&m_store_cmsEventNum,"cmsEventNum/I");
        gentree->Branch("cmsRun",&m_store_cmsRun,"cmsRun/I");
        gentree->Branch("luminosityBlock",&m_store_luminosityBlock,"luminosityBlock/I");

        gentree->Branch("matrix_element_flavour",&m_store_matrix_element_flavour,"matrix_element_flavour/I");
        gentree->Branch("weight",&m_store_weight,"weight/D");

        // Calo Jets
        std::vector< Handle<std::vector<reco::CaloJet> > > calojetsvec;
        iEvent.getManyByType(calojetsvec);

        // Fill the hash map of leading jets indexs for calo jets
        for (unsigned int i=0;i<calojetsvec.size();++i) {
            name=iEvent.getProvenance(calojetsvec[i].id()).moduleLabel();
            procname=iEvent.getProvenance(calojetsvec[i].id()).processName();
            TString names(name.c_str());
            if (not acceptAlgo(names))
                continue;
            LogInfo ("RecoJetAlgos") << procname << " " << name << std::endl;
            TObjString* s = new TObjString(name.c_str());
            if (not m_reco_jetalgonames.Contains(s)) {
                m_reco_jetalgonames.Add(s);
                LogInfo ("test") << "Adding algorithm " << s->String().Data();
            }
        }

        // PFJets
        std::vector< Handle<std::vector<reco::PFJet> > > pfjetsvec;
        iEvent.getManyByType(pfjetsvec);

        // hash map for gen jets
        for (unsigned int i=0;i<pfjetsvec.size();++i) {
            name=iEvent.getProvenance(pfjetsvec[i].id()).moduleLabel();
            procname=iEvent.getProvenance(pfjetsvec[i].id()).processName();
            TString names(name.c_str());
            if (not acceptAlgo(names))
                continue;
            LogInfo ("PFJetAlgos") << procname << " " << name << std::endl;
            TObjString* s = new TObjString(name.c_str());
            if (not m_reco_jetalgonames.Contains(s)) {
                m_reco_jetalgonames.Add(s);
                LogInfo("Algorithms") << "Adding algorithm " << s->String().Data();
            }
        }

        // JPTJets
        std::vector< Handle<std::vector<reco::JPTJet> > > jptjetsvec;
        iEvent.getManyByType(jptjetsvec);

        // hash map for gen jets
        for (unsigned int i=0;i<jptjetsvec.size();++i) {
            name=iEvent.getProvenance(jptjetsvec[i].id()).moduleLabel();
            procname=iEvent.getProvenance(jptjetsvec[i].id()).processName();
            TString names(name.c_str());
            if (not acceptAlgo(names))
                continue;
            LogInfo ("JPTJetAlgos") << procname << " " << name << std::endl;
            TObjString* s = new TObjString(name.c_str());
            if (not m_reco_jetalgonames.Contains(s)) {
                m_reco_jetalgonames.Add(s);
                LogInfo("Algorithms") << "Adding algorithm " << s->String().Data();
            }
        }


        int n_algonames=m_reco_jetalgonames.GetSize()+1;
        m_reco_flat_trees = new TTree[n_algonames];
        m_pt_ratio_tot_v = new unsigned int[n_algonames];
        m_deltaphi_tot_v = new unsigned int[n_algonames];
        m_eta_tot_v = new unsigned int[n_algonames];

        for (int jetalgoname=0;jetalgoname<n_algonames;++jetalgoname)
            m_Z_tot=
                m_iso_Z_tot=
                    m_pt_ratio_tot_v[jetalgoname]=
                        m_deltaphi_tot_v[jetalgoname]=
                            m_eta_tot_v[jetalgoname] = 0;

        LogInfo ("test") << "jetalgonames list size: " <<n_algonames<< std::endl;

        iter = m_reco_jetalgonames.MakeIterator();
        int reco_algo_counter=0;
        while (TObjString *objstr = (TObjString*) iter->Next()) {
            TTree* recotree=&(m_reco_flat_trees[reco_algo_counter]);
            TString tree_name(objstr->String());
            tree_name+="_Zplusjet";
            recotree->SetName(tree_name.Data());
            recotree->SetTitle(tree_name.Data());
            LogInfo ("test") << "tree: " << recotree->GetName() << " " << recotree->GetTitle()<< std::endl;

            recotree->Branch("Z","TParticle",&m_store_Z_part);
            recotree->Branch("matched_Z","TParticle",&m_store_matched_gen_Z_part);

            recotree->Branch("mu_plus","TParticle",&m_store_mu_plus_part);
            recotree->Branch("mu_minus","TParticle",&m_store_mu_minus_part);

            recotree->Branch("jet","TParticle",&m_store_jet_part);
            recotree->Branch("jet2","TParticle",&m_store_jet_part2);
            recotree->Branch("jet3","TParticle",&m_store_jet_part3);

            recotree->Branch("matched_calo_jet","TParticle",&m_store_matched_calo_jet_part);
            recotree->Branch("matched_calo_jet2","TParticle",&m_store_matched_calo_jet_part2);
            recotree->Branch("matched_calo_jet3","TParticle",&m_store_matched_calo_jet_part3);

            recotree->Branch("matched_gen_jet","TParticle",&m_store_matched_gen_jet_part);
            recotree->Branch("matched_gen_jet2","TParticle",&m_store_matched_gen_jet_part2);
            recotree->Branch("matched_gen_jet3","TParticle",&m_store_matched_gen_jet_part3);            

            recotree->Branch("matched_jpt_jet","TParticle",&m_store_matched_jpt_jet_part);
            recotree->Branch("matched_jpt_jet2","TParticle",&m_store_matched_jpt_jet_part2);
            recotree->Branch("matched_jpt_jet3","TParticle",&m_store_matched_jpt_jet_part3);
            
            recotree->Branch("pfjet_part1_properties",&m_pfjet_part1_properties,struct_descr);
            recotree->Branch("pfjet_part2_properties",&m_pfjet_part2_properties,struct_descr);
            recotree->Branch("pfjet_part3_properties",&m_pfjet_part3_properties,struct_descr);
           
            
            recotree->Branch("met","TParticle",&m_store_met_part);
            recotree->Branch("tcmet","TParticle",&m_store_tcmet_part);
            recotree->Branch("genmet","TParticle",&m_store_genmet_part);

            // Branch for trigger info
            recotree->Branch("HLTriggers_accept", "TClonesArray", &m_HLTriggers_fired, 32000, 0);
            recotree->Branch("recoVertices", "TClonesArray", &m_reco_vertices, 32000, 0);
            recotree->Branch("recoVerticesInfo", "TClonesArray", &m_reco_vertices_info, 32000, 0);
            recotree->Branch("recoVerticesError", "TClonesArray", &m_reco_vertices_error, 32000, 0);

            recotree->Branch("beamSpot", "TVector3",&m_beamSpot);

            recotree->Branch("algoname","TObjString",&m_store_algoname_tree);

            recotree->Branch("eventIndex",&m_store_nevts,"eventsProcessed/D");
            recotree->Branch("xsection",&m_store_xsection,"xsection/D");
            recotree->Branch("filtereff",&m_store_filtereff,"filtereff/D");
            recotree->Branch("cmsEventNum",&m_store_cmsEventNum,"cmsEventNum/I");
            recotree->Branch("cmsRun",&m_store_cmsRun,"cmsRun/I");
            recotree->Branch("luminosityBlock",&m_store_luminosityBlock,"luminosityBlock/I");

            recotree->Branch("matrix_element_flavour",&m_store_matrix_element_flavour,"matrix_element_flavour/I");
            recotree->Branch("weight",&m_store_weight,"weight/D");                        
            
            //recotree->Branch("xs",&xs,"xs/D");


//             recotree->Branch("iso_Z_tot",&m_iso_Z_tot_v[reco_algo_counter],"iso_Z_cut_tot/i");
//
//             recotree->Branch("pt_ratio_tot",&m_pt_ratio_tot_v[reco_algo_counter],"pt_ratio_tot/i");
//
//             recotree->Branch("deltaphi_tot",&m_deltaphi_tot_v[reco_algo_counter],"deltaphi_tot/i");
//
//             recotree->Branch("eta_tot",&m_eta_tot_v[reco_algo_counter],"eta_tot/O");

            reco_algo_counter++;
        }

        delete iter;

        // Pure Z w/o balancing Jet
        TTree* recotree=&(m_reco_flat_trees[m_reco_jetalgonames.GetSize()]);
        TString tree_name_reco("Zonly_reco");
        recotree->SetName(tree_name_reco.Data());
        recotree->SetTitle(tree_name_reco.Data());
        LogInfo ("test") << "tree: " << recotree->GetName() << " " << recotree->GetTitle()<< std::endl;

        recotree->Branch("Z","TParticle",&m_store_Z_part);
        recotree->Branch("matched_Z","TParticle",&m_store_matched_gen_Z_part);        

        recotree->Branch("mu_plus","TParticle",&m_store_mu_plus_part);
        recotree->Branch("mu_minus","TParticle",&m_store_mu_minus_part);

        recotree->Branch("eventIndex",&m_store_nevts,"eventsProcessed/D");
        recotree->Branch("xsection",&m_store_xsection,"xsection/D");
        recotree->Branch("filtereff",&m_store_filtereff,"filtereff/D");
        recotree->Branch("cmsEventNum",&m_store_cmsEventNum,"cmsEventNum/I");
        recotree->Branch("cmsRun",&m_store_cmsRun,"cmsRun/I");
        recotree->Branch("luminosityBlock",&m_store_luminosityBlock,"luminosityBlock/I");

        recotree->Branch("matrix_element_flavour",&m_store_matrix_element_flavour,"matrix_element_flavour/I");
        recotree->Branch("weight",&m_store_weight,"weight/D");
    }
    m_first_evt=false;


    // End of first event ------------------------------------------------------

    m_store_matrix_element_flavour = m_balancing_parton_flavour(iEvent);

    bool is_reco_there=true;
//     Handle< reco::CompositeCandidateCollection > reco_zs;
    Handle<reco::CandidateView > reco_zs;
    //std::cout << "\n\n\n --------> emptyhandle size: " << reco_zs << std::endl;
    // try {
    //std::cout << "Could not find product with:\n"<< m_reco_zs_name.c_str() << "\n";
    //std::vector< edm::Handle< reco::CompositeCandidateCollection > > allHandles;
    //iEvent.getManyByType(allHandles);
    //std::vector< edm::Handle< reco::CompositeCandidateCollection > >::iterator it;
    //for (it = allHandles.begin(); it != allHandles.end(); it++)
    //{
    //    std::cout << "module label:        " << (*it).provenance()->moduleLabel() << "\n";
    //    std::cout << "productInstanceName: " << (*it).provenance()->productInstanceName() << "\n";
    //    std::cout << "processName:         " << (*it).provenance()->processName() << "\n\n";
    //}

    //std::cout << "Obviously this is done." << std::endl;

    try {
        iEvent.getByLabel(m_reco_zs_name.c_str(), reco_zs);
    } catch (...) {
        ;
    }
    if ( !reco_zs.isValid())
    {
        LogPrint ("Reco") << "No Reco named " << m_reco_zs_name << " found!";
        is_reco_there = false;
    }
    if (is_reco_there)
        m_Z_tot+=1;

    //std::cout<< "Reco Z size "<< reco_zs->size() <<std::endl;
    if (not is_reco_there)
        LogInfo ("recoZnotFound")<< "Reco Z collection empty!\n";
    else {
        LogInfo ("recoZFound")<< "Reco Z collection not empty: "<< reco_zs->size() <<" Z found !\n";
        m_iso_Z_tot+=1;

        for (reco::CandidateView::const_iterator z = reco_zs->begin();
                z!=reco_zs->end(); ++z) {
            LogInfo ("dbg")
            << "RECO: Z plus jet found!!\n"
            << "Z info: "
            << "Mass = " << z->mass() << "\n"
            << "Pt = " << z->pt() << "\n"
            << "E = " << z->energy() << "\n";
        }

        // Cycle over the Zs and for each of them cycle over the Jets collections
        int reco_jet_index=0;
        int second_reco_jet_index=0;
        int third_reco_jet_index=0;

//         std::cout << "\n --> NUMBER OF Zs = "<< reco_zs->size() << "\n";
        int reco_z_index=0;
        for (reco::CandidateView::const_iterator z = reco_zs->begin();
                z!=reco_zs->end(); ++z) {

          
          // Match the reco Z with the Gen Z
// edm::RefToBase<reco::Candidate>::RefToBase(edm::Handle<std::vector<reco::CompositeCandidate, std::allocator<reco::CompositeCandidate> > >&, int&)'
//         XXXXXXXX
//           std::cout << "Matching the Zs\n\n\n";
          reco::CandidateBaseRef  zbaseref(reco_zs,reco_z_index);
          m_store_matched_gen_Z_part = m_get_genZ(iEvent,zbaseref);
//           m_store_matched_gen_Z_part->Dump();
          
          TIterator* iter =m_reco_jetalgonames.MakeIterator();
            int reco_algo_counter=0;
            while (TObjString *objstr = (TObjString*) iter->Next()) {
                std::string algoname(objstr->String().Data());

//                 LogInfo ("test") << "Reco Z loop: " << algoname << std::endl;
//                 std::cout << "Reco Z loop: " << algoname << std::endl;
                Handle< reco::CandidateView > jets;
                iEvent.getByLabel(algoname.c_str(), jets);

                apply_cuts (iEvent,
                            *jets,&(*z),   /// TODO
                            algoname,
                            reco_jet_index,
                            second_reco_jet_index,
                            third_reco_jet_index,
                            reco_algo_counter,
                            true);

                // if all the cuts were passed, fill the tree!
                if (reco_jet_index>=0 and reco_jet_index<(int)jets->size()) { // Set the branches attributes and fill!

                    // Read out the met
                    TString metname("");
                    if (algoname.find("PF")!=std::string::npos) {
                        //                         std::cout << "\n\n PF ALGORITHM! seeking for the PF MET\n\n";
                        metname="pfMet";
                    }
                    else
                        metname="met";

                    edm::Handle< edm::View< reco::MET > > met;
                    iEvent.getByLabel(metname.Data(), met);
                    m_store_met_part = m_fill_tparticle(& (*met)[0] );

                    iEvent.getByLabel("tcMet", met);
                    m_store_tcmet_part = m_fill_tparticle(& (*met)[0] );
                    
                    edm::Handle< edm::View< reco::MET > > gmet;
                    iEvent.getByLabel("genMetTrue", gmet);
                    m_store_genmet_part = m_fill_tparticle(& (*gmet)[0] );

                    // Set the algoname branch
                    m_store_algoname_tree->SetString(algoname.c_str());

                    // Set HLT Info
                    FillTriggerInfo( iEvent, iSetup );
                    
                    FillRecoVertices( iEvent);
                    FillBeamSpot( iEvent);

                    // Set the Z particle
                    m_store_Z_part = m_fill_tparticle(&(*z));
                    m_store_Z_part->SetStatusCode(z->status());

                    // Set the jet "particle"
                    m_store_jet_part = m_fill_tparticle(&((*jets)[reco_jet_index]),true);
                    m_store_jet_part->SetPdgCode(0);
                    /*m_pfjet_part1_properties = */m_get_pfproprerties(&((*jets)[reco_jet_index]),m_pfjet_part1_properties);
                    
                    reco::CandidateBaseRef jetbaseref(jets,reco_jet_index);
                    const reco::Candidate* calojet =m_get_calojet(iEvent,
                                                    algoname,
                                                    jetbaseref);
                    m_store_matched_calo_jet_part = m_fill_tparticle(calojet,true);

                    const reco::Candidate* jptjet =m_get_jptjet(iEvent,
                                                   algoname,
                                                   jetbaseref);
                    m_store_matched_jpt_jet_part = m_fill_tparticle(jptjet,true);                    
                    
                    const reco::Candidate* genjet = m_get_genjet(iEvent,
                                                    algoname,
                                                    jetbaseref);
                    
                    m_store_matched_gen_jet_part = m_fill_tparticle(genjet,true);
                    
                    // Set the 2nd most energetic jet "particle" (if possible)
                    //unsigned int second_reco_jet_index=reco_jet_index+1;
                    if ((int)(*jets).size() >= second_reco_jet_index+1) {
                        m_store_jet_part2 = m_fill_tparticle(&((*jets)[second_reco_jet_index]),true);
                        m_store_jet_part2->SetPdgCode(0);
                        /*m_pfjet_part2_properties =*/ m_get_pfproprerties(&((*jets)[second_reco_jet_index]),m_pfjet_part2_properties);
                        reco::CandidateBaseRef jetbaseref(jets,second_reco_jet_index);
                        const reco::Candidate* calojet = m_get_calojet(iEvent,
                                                        algoname,
                                                        jetbaseref);
                        m_store_matched_calo_jet_part2 = m_fill_tparticle(calojet,true);
                        
                        const reco::Candidate* jptjet = m_get_jptjet(iEvent,
                                                        algoname,
                                                        jetbaseref);
                        m_store_matched_jpt_jet_part2 = m_fill_tparticle(jptjet,true);                        
                        
                        const reco::Candidate* genjet = m_get_genjet(iEvent,
                                                                     algoname,
                                                                     jetbaseref);
                    
                        m_store_matched_gen_jet_part2 = m_fill_tparticle(genjet,true);                        
                    }
                    else {
                        m_store_jet_part2 = new TParticle();
                        m_store_jet_part2->SetMomentum(0,0,0,0);
                        m_store_jet_part2->SetProductionVertex(0,0,0,0);
                        m_store_matched_calo_jet_part2 = new TParticle();
                        m_store_matched_jpt_jet_part2 = new TParticle();
                        m_store_matched_gen_jet_part2 = new TParticle();
                        /*m_pfjet_part2_properties =*/ m_get_pfproprerties(0,m_pfjet_part2_properties);
                    }

                    // Set the 3rd most energetic jet "particle" (if possible)
                    //unsigned int third_reco_jet_index=reco_jet_index+2;
                    if ((int)(*jets).size()>=third_reco_jet_index+1) {
                        m_store_jet_part3 = m_fill_tparticle(&((*jets)[third_reco_jet_index]),true);
                        m_store_jet_part3->SetPdgCode(0);
                       /* m_pfjet_part3_properties = */m_get_pfproprerties(&((*jets)[third_reco_jet_index]),m_pfjet_part3_properties);
                        reco::CandidateBaseRef jetbaseref(jets,third_reco_jet_index);
                        const reco::Candidate* calojet =m_get_calojet(iEvent,
                                                       algoname,
                                                       jetbaseref);

                        m_store_matched_calo_jet_part3 = m_fill_tparticle(calojet,true);

                        const reco::Candidate* jptjet =m_get_jptjet(iEvent,
                                                       algoname,
                                                       jetbaseref);

                        m_store_matched_jpt_jet_part3 = m_fill_tparticle(jptjet,true);                        
                        
                        const reco::Candidate* genjet = m_get_genjet(iEvent,
                                                                     algoname,
                                                                     jetbaseref);
                    
                        m_store_matched_gen_jet_part3 = m_fill_tparticle(genjet,true);                        
                    }
                    else {
                        m_store_jet_part3 = new TParticle();
                        m_store_jet_part3->SetMomentum(0,0,0,0);
                        m_store_jet_part3->SetProductionVertex(0,0,0,0);
                        m_store_matched_calo_jet_part3 = new TParticle();
                        m_store_matched_jpt_jet_part3 = new TParticle();
                        m_store_matched_gen_jet_part3 = new TParticle();                        
                        /*m_pfjet_part3_properties = */m_get_pfproprerties(0,m_pfjet_part3_properties);
                    }
                    // Now the muons..
                    const reco::Muon* z_muons[2];
                    z_muons[0]= (const reco::Muon*) z->daughter(0);
                    z_muons[1]= (const reco::Muon*) z->daughter(1);
                    for (int i=0;i<2;++i) {
                        if (z_muons[i]->charge()>0) {
                            m_store_mu_plus_part = m_fill_tparticle(z_muons[i]);
                            m_store_mu_plus_part->SetStatusCode(z_muons[i]->status());
                        }
                        else {
                            m_store_mu_minus_part = m_fill_tparticle(z_muons[i]);
                            m_store_mu_minus_part->SetStatusCode(z_muons[i]->status());
                        }
                    }
                    m_output_file->cd();
                    m_reco_flat_trees[reco_algo_counter].Fill();
                }
                reco_algo_counter++;
            }
            // Fill the Z only tree
            m_reco_flat_trees[reco_algo_counter].Fill();
            reco_z_index++;
        }
      
    }

    //     }
    //  catch(...) {
    // is_reco_there=false;
    //     std::cout << "No RECO found. Variable is_reco_there is set to false." << std::endl;
    //     }

    // Gen Zs --------------
    Handle<reco::CompositeCandidateCollection > gen_zs;
    try
    {
        iEvent.getByLabel(m_gen_zs_name.c_str(), gen_zs);
    }
    catch (...)
    {
        ;
    }

    if ( gen_zs.isValid() )
    {
        if (gen_zs->size()==0)
            LogInfo ("genZnotFound")<< "Gen Z collection empty!\n";
        else
            LogInfo ("genZFound")<< "Gen Z collection not empty: "<< gen_zs->size()<<" Z found !\n";

        for (reco::CompositeCandidateCollection::const_iterator z = gen_zs->begin();
                z!=gen_zs->end(); ++z) {

            LogInfo ("dbg")
            << "GEN: Z plus jet found!\n"
            << "Z info: "
            << "Mass = " << z->mass() << "\n"
            << "Pt = " << z->pt() << "\n"
            << "E = " << z->energy() << "\n";
        }

        // Cycle over the Zs and for each of them cycle over the Jets collections
        int gen_jet_index=0;
        int second_gen_jet_index=0;
        int third_gen_jet_index=0;

        for (reco::CompositeCandidateCollection::const_iterator z = gen_zs->begin();
                z!=gen_zs->end(); ++z) {
            TIterator* iter =m_gen_jetalgonames.MakeIterator();
            int gen_algo_counter=0;
            while (TObjString *objstr = (TObjString*)iter->Next()) {
                std::string algoname(objstr->String().Data());
                LogInfo ("test") << "Gen Z loop: " << algoname << std::endl;


                // Get the met!
                edm::Handle< edm::View< reco::MET > > met;
                iEvent.getByLabel("genMetTrue", met);
                m_store_met_part = m_fill_tparticle(& (*met)[0] );

                // Get the jet collection form the file
                Handle<reco::GenJetCollection> jets;
                iEvent.getByLabel(algoname.c_str(), jets);

                // See which is the jet which balances the Z (if any!)
                apply_cuts (iEvent,
                            *jets,&(*z),
                            algoname,
                            gen_jet_index,
                            second_gen_jet_index,
                            third_gen_jet_index);
                // if all the cuts were passed, fill the tree!
                if (gen_jet_index>=0 and gen_jet_index<(int)jets->size()) { // Set the branches attributes and fill!

                    // Set the algoname branch
                    m_store_algoname_tree->SetString(algoname.c_str());

                    // Set the Z particle
                    m_store_Z_part = m_fill_tparticle(&(*z));
                    m_store_Z_part->SetStatusCode(z->status());

                    // Set the jet "particle"
                    m_store_jet_part = m_fill_tparticle(&((*jets)[gen_jet_index]),true);

                    int originatingpartoncode=0;

//                 int originatingpartoncode=(((*jets)[reco_jet_index]).getGenConstituents())[0]->pdgId();
//                 std::vector< const reco::GenParticle * > v = (&((*jets)[reco_jet_index]))->getGenConstituents();
//                 LogInfo ("gengetparton") << "Gen parts coll size = " << v.size();
//                 LogInfo ("gengetparton") << "genjet parton: " << originatingpartoncode;

                    m_store_jet_part->SetPdgCode(originatingpartoncode);


                    // Set the 2nd most energetic jet "particle" (if possible)
                    //unsigned int second_gen_jet_index=gen_jet_index+1;
                    if ((int)(*jets).size()>=second_gen_jet_index+1) {
                        m_store_jet_part2 = m_store_jet_part2 = m_fill_tparticle(&((*jets)[second_gen_jet_index]),true);
                        m_store_jet_part2->SetPdgCode(0);
                    }
                    else {
                        m_store_jet_part2 = new TParticle();
                        m_store_jet_part2->SetMomentum(0,0,0,0);
                        m_store_jet_part2->SetProductionVertex(0,0,0,0);
                    }

                    // Set the 3rd most energetic jet "particle" (if possible)
                    // unsigned int third_gen_jet_index=gen_jet_index+2;
                    if ((int)(*jets).size()>=third_gen_jet_index+1) {
                        m_store_jet_part3 = m_fill_tparticle(&((*jets)[third_gen_jet_index]),true);
                        m_store_jet_part3->SetPdgCode(0);
                    }
                    else {
                        m_store_jet_part3 = new TParticle();
                        m_store_jet_part3->SetMomentum(0,0,0,0);
                        m_store_jet_part3->SetProductionVertex(0,0,0,0);
                    }



                    // Now the muons..
                    const reco::Muon* z_muons[2];
                    z_muons[0]= (const reco::Muon*) z->daughter(0);
                    z_muons[1]= (const reco::Muon*) z->daughter(1);
                    for (int i=0;i<2;++i) {
                        if (z_muons[i]->charge()>0) {
                            m_store_mu_plus_part = m_fill_tparticle(z_muons[i]);
                            m_store_mu_plus_part->SetStatusCode(z_muons[i]->status());
                        }
                        else {
                            m_store_mu_minus_part = m_fill_tparticle(z_muons[i]);
                            m_store_mu_minus_part->SetStatusCode(z_muons[i]->status());
                        }
                    }
                    m_output_file->cd();
                    m_gen_flat_trees[gen_algo_counter].Fill();
                }
                gen_algo_counter++;
            }
            m_gen_flat_trees[gen_algo_counter].Fill();
        }
    }
}


//------------------------------------------------------------------------------

void ZplusjetTreeMaker::beginJob() {
}

//------------------------------------------------------------------------------

void ZplusjetTreeMaker::endJob() {
    std::cout << "\n\n\nEvents where all the collection were produced: " << m_store_nevts << std::endl;
    std::cout << "\n\n\nMuon were jets seeds: " << m_muon_is_seed << std::endl;

}

//------------------------------------------------------------------------------

/**
This function actually applies the set of cuts of the Z+Jet analysis of Volker
Buege. It returns the index of the leading jet that balances the z that is in
the input. It is a template to threat simmetrically Gen jets and Calo Jets
collections.
For more details about the cuts and the strategies to implement them, see the
comments in the code.
Remember that the -1 index means that no suitable jet was found in the
collection.
**/
template <class myjetsCollection>
int ZplusjetTreeMaker::apply_cuts (const edm::Event& iEvent,
                                   const myjetsCollection& jets,
                                   const reco::Candidate* z,
                                   std::string algoname,
                                   int& jet1_index,
                                   int& jet2_index,
                                   int& jet3_index,
                                   int algo_counter,
                                   bool is_calo) {

    using namespace edm;
    // If no jets, skip!
    if (jets.size()==0) {
        LogInfo ("Nojet") <<  "+++ No jet Found!, skipping!\n";
        jet1_index=jet2_index=jet3_index=99999999;
        return -1;
    }

    /*
    See if the isolation cuts are respected.
    Avoid also to consider jetsthat are indeed muons.
    */

    const reco::Jet* leading_jet=0;
    const reco::Jet* second_leading_jet=0;

    int leading_jet_index=0;
    leading_jet = (reco::Jet*) &( jets[leading_jet_index] );

    /*
    Here we build the array of muons that will be filled with the decay
    products of the Z.
    */
    const reco::Muon* z_muons[2];
    z_muons[0]= (const reco::Muon*) z->daughter(0);
    z_muons[1]= (const reco::Muon*) z->daughter(1);


    // Find the Leading and second Leading jets

    // Leading jet
    while (not is_valid_jet ( iEvent, leading_jet, leading_jet_index, z_muons, is_calo, algoname) ) {

        ++leading_jet_index;

        // to avoid segfaults
        if (leading_jet_index == (int) jets.size()) {
            LogInfo ("Nojet") <<"+++ "<<algoname<<" : No leading jet found!\n";
            jet1_index=jet2_index=jet3_index=99999999;
            return -1;
        }

        leading_jet = (reco::Jet*) &( jets[leading_jet_index] );

    }

    // now do that for the second leading jet. Same strategy..
    bool second_leading_jet_present=true;
    /*
    It must be at least after the leading one, since the jets are
    ordered in pt.
    */

    int second_leading_jet_index=leading_jet_index+1;

    if (leading_jet_index== (int) jets.size()-1)
        second_leading_jet_present=false;
    else {
        second_leading_jet =
            (reco::Jet*) &(jets[second_leading_jet_index]);

        while (not is_valid_jet ( iEvent, second_leading_jet, second_leading_jet_index, z_muons, is_calo, algoname) ) {
            LogInfo ("Nojet")<< "+++ " << algoname
            << " : Second Leading Jet is a Mu... Next one!\n";
            ++second_leading_jet_index;

            if (second_leading_jet_index== (int) jets.size()) {
                LogInfo ("Nojet")<< "+++ " << algoname
                << " : No second leading jet found!\n";
                second_leading_jet_present=false;
                second_leading_jet_index=99999999;
                break;
            }
            second_leading_jet =
                (reco::Jet*) &( jets[second_leading_jet_index] );
        }
    }

    LogInfo ("Nojet")<<  "+++ " << algoname <<" : Leading Jet is the number: "
    << leading_jet_index << " in the collection.\n";

    /*
        Second leading Jet pt cut. --------------
        If no 2nd lead jet present, it's passed.
    */

    // DEBUG
    double ratio_factor=1.;
    if (not is_calo)
        ratio_factor=1.;

    bool pt_ratio_cut=false;
    if (second_leading_jet_present) {
        double pt_ratio = second_leading_jet->pt()/z->pt();
        pt_ratio_cut = pt_ratio < m_2ndJetZptRatio*ratio_factor;
    }
    else
        pt_ratio_cut = true;

    if (pt_ratio_cut and second_leading_jet_present)
        LogInfo ("Ptratio")<<"+++ "<< algoname
        << " : PT ratio = " << second_leading_jet->pt()/z->pt()
        << "( leadingJPt = " << leading_jet->pt()
        << " , 2ndJPt= "  <<  second_leading_jet->pt()
        << " , Zpt = " << z->pt() << " ) " << " --> "
        << "Pt ratio 2nd leading jet's cut for algorithm passed!\n";

    if (not pt_ratio_cut) {
        LogInfo ("Ptratio")<<"+++ "<< algoname
        << " : PT ratio = " << second_leading_jet->pt()/z->pt()
        << "( leadingJPt = " << leading_jet->pt()
        << " , 2ndJPt= "  <<  second_leading_jet->pt()
        << " , Zpt = " << z->pt() << " ) " << " --> "
        << "Pt ratio 2nd leading jet's cut for algo not passed!\n";
        jet1_index=jet2_index=jet3_index=99999999;
        return -1;
    }
    if (is_calo)
        m_pt_ratio_tot_v[algo_counter]+=1;
    // End Pt Ratio cut-----------------------

    // Phi balancing cut ---------------------

    double absdeltaphi=
        fabs(ROOT::Math::VectorUtil::DeltaPhi(leading_jet->p4(),z->p4()));

    // DEBUG try to use root
//     double test_absdeltaphi=fabs(ROOT::Math::VectorUtil::Phi_mpi_pi(leading_jet->phi())-
//                                  ROOT::Math::VectorUtil::Phi_mpi_pi(z->phi()));

//     std::cout << "Test deltaphi:\n"
//               << "  * TMath::Pi()-absdeltaphi " <<  TMath::Pi()-absdeltaphi
//               << "  * fabs(TMath::Pi()-test_absdeltaphi) = " <<  fabs(TMath::Pi()-test_absdeltaphi) << std::endl;


//     LogInfo ("deltaphidebug") << "1)absdeltaphi =" << absdeltaphi;

//    if (absdeltaphi>TMath::Pi()/2)
    absdeltaphi=TMath::Pi()-absdeltaphi;

//     LogInfo ("deltaphidebug") << "2)absdeltaphi =" << absdeltaphi;

    double maxdeltaphi = m_zJetPhiBalancing;
    if (absdeltaphi > maxdeltaphi)
    {
        LogInfo ("Balancing")<<"+++ "<<algoname<<" : Delta Phi Z-Jet: "
        << absdeltaphi << " --> "
        << "Phi balancing cut for algorithm not passed!\n";
        jet1_index=jet2_index=jet3_index=99999999;
        return -1;
    }
    LogInfo ("Balancing")<<"+++ "<<algoname<<" : Delta Phi Z-Jet: "
    << absdeltaphi << " --> "
    << "Phi balancing cut for algorithm passed!\n";

    if (is_calo)
        m_deltaphi_tot_v[algo_counter]+=1;
    // End Phi balancing cut -----------------


    // Eta of the Leading Jet------------------
    if (leading_jet->eta()*leading_jet->eta()>m_jetMaxEta*m_jetMaxEta) {
        LogInfo ("Nojet")<< "+++ "<< algoname
        << " : Leading Jet Eta= " << leading_jet->eta() << " --> "
        << "Max leading jet's eta cut for algorithm not passed!\n";
        jet1_index=jet2_index=jet3_index=99999999;
        return -1;
    }

    if (is_calo)
        m_eta_tot_v[algo_counter]+=1;
    // End Eta of the Jet-----------------------



    // At this point all the cuts are passed:
    LogInfo ("AllCuts")<<"+++ "<<algoname<<" : All cuts passed!\n";

    jet1_index=leading_jet_index;
    jet2_index=second_leading_jet_index;

    // find 3rd jet
    jet3_index=jet2_index+1;
    if (jet3_index<(int)jets.size()) {
        reco::Jet* third_leading_jet =(reco::Jet*) &(jets[jet3_index]);

        while (not is_valid_jet ( iEvent, third_leading_jet, jet3_index, z_muons, is_calo, algoname)) {
            jet3_index+=1;
            if (jet3_index==(int)jets.size()) {
                jet3_index=99999999;
                break;
            }
            third_leading_jet =(reco::Jet*) &(jets[jet3_index]);
        }
    }
    else
        jet3_index=99999999;

    return leading_jet_index;
//     std::cout << "Reco jet indexes " << jet1_index << "\n" << std::endl <<
//                                         jet2_index << "\n" << std::endl <<
//                                         jet3_index << "\n" << std::endl;

}

//------------------------------------------------------------------------------

/**
Simple function that checks if the muon and the jet in input are far, speaking
of Delta R, enough and the jet is within JetID cuts
**/
bool ZplusjetTreeMaker::is_valid_jet ( const edm::Event& iEvent,
                                       const reco::Jet* jet,
                                       int jet_index,
                                       const reco::Muon** z_muons,
                                       bool is_calo, // NOTE + TODO is not always set the correct value
                                       std::string algoname) {
//     if (is_calo)
//         return true;

    using namespace edm;

    double dr0=deltaR(*z_muons[0], *jet);
    double dr1=deltaR(*z_muons[1], *jet);

    bool is_good_jet=(dr0 > m_zMuMinDr) and (dr1 > m_zMuMinDr);

//     if (not is_good_jet)
//         std::cout <<"+++ deltaR(*z_muons[0], *jet) = "
//                   << dr0
//                   << " , deltaR(*z_muons[1], *jet) = "
//                   << dr1 << std::endl;


    if (!is_good_jet) {
        m_muon_is_seed++;
    }
    // for calo jets
    const reco::CaloJet * caloJet = dynamic_cast<const reco::CaloJet *> ( jet );
    if ( caloJet != NULL )
    {
        // this looks not so nice, but there seems to be no other way
        std::map< std::string , std::string >::iterator it =  this->m_caloJetIdMapping.find( algoname );
        if ( it == this->m_caloJetIdMapping.end() )
        {
            std::cout << "Calo Algorithm " << algoname << " not supported by JetIDCut. Skipping JetId Cut";
            exit(10);
        }
        else
        {
//             std::cout << "Calo Jet " << algoname << " PT:" << caloJet->pt() << std::endl;
            
            std::string algoJetId = it->second;

            edm::Handle<reco::JetIDValueMap> hJetIDMap;
            iEvent.getByLabel( algoJetId, hJetIDMap );

            edm::Handle<edm::View< reco::CaloJet > > hJets;
            iEvent.getByLabel(algoname, hJets );

            edm::RefToBase<reco::CaloJet> jetRef = hJets->refAt(jet_index);
            reco::JetID jetId = (*hJetIDMap)[ jetRef ];

            // start with cuts
            LogDebug("JetIDCut") << "Applying JetIdCut to CaloJets" << std::endl;

            // restrictedEMF > 0.01 , Jet EMF : fraction of jet energy that is electromagnetic.
            if ( !( caloJet->emEnergyFraction()  > 0.01 ))
                is_good_jet = false;

            // minimal number of RecHits containing 90% of the jet energy
            if ( !( jetId.n90Hits >= 2 ))
                is_good_jet = false;

            // the fraction of jet energy from the hottest HPD
            if ( !( jetId.fHPD < 0.98 ))
                is_good_jet = false;
        }
    }

    // for particle flow jets https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
    const reco::PFJet * pfJet = dynamic_cast<const reco::PFJet *> ( jet );
    if (pfJet != NULL)
    {
//         std::cout << "PF non nullo\n";
        LogDebug("JetIDCut") << "Applying JetIdCut to PFJet" << std::endl;

        // chfJet > 0 : Fraction of Jet energy carried by charged hadrons
//         if ( !( (pfJet->chargedHadronEnergy() /  pfJet->energy() ) > 0.0 ))
//             is_good_jet = false;

        //cmultiJet > 0 : Number of charged Jet particles
        // Double check with danilo
//         if ( !( (pfJet->chargedHadronMultiplicity() +  pfJet->electronMultiplicity() + pfJet->muonMultiplicity()  ) > 0 ))
//             is_good_jet = false;

        //cemfJet < 1 : Fraction of Jet energy carried by charged EM particles
//         if ( !( (pfJet->chargedEmEnergy() /  pfJet->energy() ) < 1.0 ))
//             is_good_jet = false;

        //nfhJet < 1 : Fraction of Jet energy carried by neutral hadrons
        if ( !( (pfJet->neutralHadronEnergy() / pfJet->energy() ) < .99 ))
            is_good_jet = false;

        //nemfJet < 1 : Fraction of Jet energy carried by neutral EM particles
        if ( !( (pfJet->neutralEmEnergy() / pfJet->energy() ) < .99 ))
            is_good_jet = false;

        //ncj > 1  : Number of Jet constituents
        if ( !( (pfJet->getJetConstituents().size() ) > 1 ))
            is_good_jet = false;


        // Add criteria for PF jets with eta > 2.4 according to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
        if (TMath::Abs(pfJet->eta()) > 2.4 ){

            if ( !( (pfJet->chargedHadronEnergy() /  pfJet->energy() ) > 0.0 ))
                is_good_jet = false;

            if ( !( pfJet->chargedHadronMultiplicity()  > 0 ))
                is_good_jet = false;

            if ( !( (pfJet->chargedEmEnergy() /  pfJet->energy() ) < .99 ))
                is_good_jet = false;
            }


        LogDebug("JetIDCut") << "PFJet cut result: " << is_good_jet << std::endl;
    }

    return is_good_jet;
}

//------------------------------------------------------------------------------

/**
Fill the TParticle with all the relevant attributes of the recoparticle.
**/
TParticle * ZplusjetTreeMaker::m_fill_tparticle(
    const reco::Candidate* recoparticle,
    bool is_jet) {
    TParticle* tparticle = new TParticle();

    if (recoparticle==NULL) {
        //std::cout << "[m_fill_tparticle] Empty particle..\n";
        tparticle->SetMomentum(0,0,0,0);
        tparticle->SetProductionVertex(0,0,0,0);
        tparticle->SetCalcMass(0);
    }
    else {
        //std::cout << "[m_fill_tparticle] Full particle..\n";
        tparticle->SetMomentum(recoparticle->px(),
                               recoparticle->py(),
                               recoparticle->pz(),
                               recoparticle->energy());

        tparticle->SetProductionVertex(recoparticle->vx(),
                                       recoparticle->vy(),
                                       recoparticle->vz(),
                                       0);
        tparticle->SetCalcMass(recoparticle->mass());

        // The particle is a Jet
        if (is_jet) {
            //std::cout << "[m_fill_tparticle] Jet areas..\n";
            double jet_area=static_cast<const reco::Jet*>(recoparticle)->jetArea();
            tparticle->SetProductionVertex(jet_area,jet_area,jet_area,jet_area);
        }

    }

    return tparticle;
}

void ZplusjetTreeMaker::m_get_pfproprerties(const reco::Candidate* recoparticle, PFProperties& pf_proprerties){

  
//   for (int i=0;i<15;++i)
//     pf_proprerties_arr[i]=0;
//   
//   return pf_proprerties_arr;
  
  //

  const reco::PFJet * pfJet = dynamic_cast<const reco::PFJet *> ( recoparticle );
  if (pfJet != NULL){
    pf_proprerties.ChargedHadronEnergy= pfJet->chargedHadronEnergy();
    pf_proprerties.ChargedHadronMultiplicity=pfJet->chargedHadronMultiplicity();
    pf_proprerties.ChargedHadronEnergyFraction=pfJet->chargedHadronEnergyFraction();
    
    pf_proprerties.NeutralHadronEnergy=pfJet->neutralHadronEnergy();                  
    pf_proprerties.NeutralHadronMultiplicity=pfJet->neutralHadronMultiplicity();
    pf_proprerties.NeutralHadronEnergyFraction=pfJet->neutralHadronEnergyFraction();
    
    pf_proprerties.ChargedEmEnergy=pfJet->chargedEmEnergy();
    pf_proprerties.NeutralEmEnergy=pfJet->neutralEmEnergy();

    pf_proprerties.NeutralMultiplicity=pfJet->neutralMultiplicity();   
    pf_proprerties.ChargedMultiplicity=pfJet->chargedMultiplicity();   
    
    pf_proprerties.ElectronEnergy=pfJet->electronEnergy();
    pf_proprerties.ElectronMultiplicity=pfJet->electronMultiplicity();       
    pf_proprerties.ElectronEnergyFraction=pfJet->electronEnergyFraction();       
    
    pf_proprerties.MuonEnergy=pfJet->muonEnergy();
    pf_proprerties.MuonMultiplicity=pfJet->muonMultiplicity();        
    pf_proprerties.MuonEnergyFraction=pfJet->muonEnergyFraction();        
    
    pf_proprerties.PhotonEnergy=pfJet->photonEnergy();
    pf_proprerties.PhotonMultiplicity=pfJet->photonMultiplicity();
    pf_proprerties.PhotonEnergyFraction=pfJet->photonEnergyFraction();

    pf_proprerties.Constituents=pfJet->getPFConstituents().size();
    }
  else{
      pf_proprerties.ChargedHadronEnergy= 0;
      pf_proprerties.ChargedHadronMultiplicity= 0;
      pf_proprerties.ChargedHadronEnergyFraction=0;
      
      pf_proprerties.NeutralHadronEnergy= 0;
      pf_proprerties.NeutralHadronMultiplicity= 0;
      pf_proprerties.NeutralHadronEnergyFraction=0;
      
      pf_proprerties.ChargedEmEnergy= 0;
      pf_proprerties.NeutralEmEnergy= 0;

      pf_proprerties.NeutralMultiplicity= 0;
      pf_proprerties.ChargedMultiplicity= 0;

      pf_proprerties.ElectronEnergy= 0;
      pf_proprerties.ElectronMultiplicity= 0;
      pf_proprerties.ElectronEnergyFraction=0;
      
      pf_proprerties.MuonEnergy= 0;
      pf_proprerties.MuonMultiplicity= 0;
      pf_proprerties.MuonEnergyFraction=0;
      
      pf_proprerties.PhotonEnergy= 0;
      pf_proprerties.PhotonMultiplicity= 0;
      pf_proprerties.PhotonEnergyFraction=0;
      
      pf_proprerties.Constituents= 0;
      }

 
  }

void ZplusjetTreeMaker::FillBeamSpot(const edm::Event& iEvent)
{
  reco::BeamSpot recobeamSpot;
  edm::Handle<reco::BeamSpot> recobeamSpotHandle;
  iEvent.getByLabel("offlineBeamSpot", recobeamSpotHandle);
  m_beamSpot->Clear("C");
  if ( recobeamSpotHandle.isValid() )
    {
      recobeamSpot = *recobeamSpotHandle;
      m_beamSpot->SetX(recobeamSpot.x0());
      m_beamSpot->SetY(recobeamSpot.y0());
      m_beamSpot->SetZ(recobeamSpot.z0());
    } 
  else
    {
      edm::LogWarning("BeamSpot")	<< "No beam spot available from EventSetup";
    }
}

void ZplusjetTreeMaker::FillRecoVertices(const edm::Event& iEvent)
        {
       edm::Handle<reco::VertexCollection> recoVertices;
       iEvent.getByLabel("offlinePrimaryVertices",recoVertices);
       m_reco_vertices->Clear("C");
       m_reco_vertices_info->Clear("C");
       m_reco_vertices_error->Clear("C");
       TVector3 *vert = new TVector3;
       TVector3 *vert_error = new TVector3;
       TVector3 *vert_info = new TVector3;

       for (unsigned int i=0;i< recoVertices->size();i++){
         if ((*recoVertices)[i].isValid()  and not (*recoVertices)[i].isFake()){

           vert->SetX((*recoVertices)[i].x());
           vert->SetY((*recoVertices)[i].y());
           vert->SetZ((*recoVertices)[i].z());

           vert_info->SetX((*recoVertices)[i].chi2());
           vert_info->SetY((*recoVertices)[i].ndof());
           vert_info->SetZ((*recoVertices)[i].tracksSize());

           vert_error->SetX((*recoVertices)[i].xError());
           vert_error->SetY((*recoVertices)[i].yError());
           vert_error->SetZ((*recoVertices)[i].zError());
           
           new ((*m_reco_vertices)[i]) TVector3(*vert);
           new ((*m_reco_vertices_info)[i]) TVector3(*vert_info);
           new ((*m_reco_vertices_error)[i]) TVector3(*vert_error);
         }
       }

    delete vert;
    delete vert_info;
    delete vert_error;
}


//------------------------------------------------------------------------------

/**
Select algorithm if the name matches any entry in the white list.
If the whitelist is empty, accept all algorithms.
**/
bool ZplusjetTreeMaker::acceptAlgo(const TString &names) {

    bool selectAlgo = false;

    std::vector<std::string>::const_iterator whiteList =
        m_algo_whitelist_vstring.begin();

    std::vector<std::string>::const_iterator blackList =
        m_algo_blacklist_vstring.begin();
                
        
    if (whiteList == m_algo_whitelist_vstring.end()) {
        selectAlgo = true;
    }
    else {
        for (whiteList = m_algo_whitelist_vstring.begin();
                whiteList!=m_algo_whitelist_vstring.end(); ++whiteList)
            if (names.Contains((*whiteList).c_str()))
                selectAlgo = true;
        for (blackList = m_algo_blacklist_vstring.begin();
                blackList!=m_algo_blacklist_vstring.end(); ++blackList)
          if (names.Contains((*blackList).c_str()))
            selectAlgo = false;    
                
            }



    return selectAlgo;
}

//------------------------------------------------------------------------------

/**
Get the matched calo jet
**/
const reco::Candidate* ZplusjetTreeMaker::m_get_calojet(const edm::Event& iEvent,
        std::string& algoname,
        reco::CandidateBaseRef& jetbaseref) {


  
    std::string CaloToGenJetMap_name(algoname);
    CaloToGenJetMap_name+="calomatch";

//     std::cout << " -->MATCHING algo " << algoname << " with map " << CaloToGenJetMap_name << ": ";
    
    if (CaloToGenJetMap_name.find("Calo") == std::string::npos) {

        edm::RefToBase<reco::Candidate> candGenJetRef;

        try {

            edm::Handle< reco::CandViewMatchMap > CaloToGenJetMaphandle;

            iEvent.getByLabel(CaloToGenJetMap_name,"rec2gen",CaloToGenJetMaphandle);

            reco::CandViewMatchMap CaloToGenJetMap(*CaloToGenJetMaphandle);

            candGenJetRef = CaloToGenJetMap[jetbaseref];
//             std::cout << " SUCCESS \n";
        }

        catch (...) {
//             LogDebug("Match map") << "Map used for the matching not found!!!";
//             std::cout << " FAIL\n";
            return 0;
        }

        return candGenJetRef.get();

    }
    else return 0;
}

//------------------------------------------------------------------------------

/**
Get the matched jpt jet
**/
const reco::Candidate* ZplusjetTreeMaker::m_get_jptjet(const edm::Event& iEvent,
        std::string& algoname,
        reco::CandidateBaseRef& jetbaseref) {

 
    std::string jptToGenJetMap_name(algoname);
    jptToGenJetMap_name+="jptmatch";

//      std::cout << " -->MATCHING algo " << algoname << " with map " << jptToGenJetMap_name << ": ";
    
    if (jptToGenJetMap_name.find("PF") != std::string::npos) {

        edm::RefToBase<reco::Candidate> candGenJetRef;

        try {

            edm::Handle< reco::CandViewMatchMap > jptToGenJetMaphandle;

            iEvent.getByLabel(jptToGenJetMap_name,"rec2gen",jptToGenJetMaphandle);

            reco::CandViewMatchMap jptToGenJetMap(*jptToGenJetMaphandle);

            candGenJetRef = jptToGenJetMap[jetbaseref];
//              std::cout << " SUCCESS \n";
        }

        catch (...) {
             LogDebug("Match map") << "Map used for the matching not found!!!";
//              std::cout << " FAIL\n";
            return 0;
        }

        return candGenJetRef.get();

    }
    else return 0;
}

//------------------------------------------------------------------------------

TParticle* ZplusjetTreeMaker::m_get_genZ(const edm::Event& iEvent,reco::CandidateBaseRef& jetbaseref){
   
   std::string ZToGenJetMap_name("matchZs");
//    std::cout << " -->MATCHING " << ZToGenJetMap_name << ": "; 
   edm::RefToBase<reco::Candidate> ZGenJetRef;

   try {
        edm::Handle< reco::CandViewMatchMap > ZToGenJetMaphandle;
        iEvent.getByLabel(ZToGenJetMap_name,"rec2gen",ZToGenJetMaphandle);
        reco::CandViewMatchMap ZToGenJetMap(*ZToGenJetMaphandle);
        ZGenJetRef = ZToGenJetMap[jetbaseref];
//         std::cout << " SUCCESS \n";
       }

   catch (...) {
        LogDebug("Match map") << "Map used for the matching not found!!!";
//         std::cout << " FAIL\n";
        return 0;
        }

   
   return m_fill_tparticle(ZGenJetRef.get());

  
  }

//------------------------------------------------------------------------------
/**
Get the matched gen jet
**/
const reco::Candidate* ZplusjetTreeMaker::m_get_genjet(const edm::Event& iEvent,
        std::string& algoname,
        reco::CandidateBaseRef& jetbaseref) {


  
    std::string PFToGenJetMap_name(algoname);
    PFToGenJetMap_name+="genmatch";

//     std::cout << " -->MATCHING algo " << algoname << " with map " << PFToGenJetMap_name << ": ";
    
    if (PFToGenJetMap_name.find("Calo") == std::string::npos) {

        edm::RefToBase<reco::Candidate> candGenJetRef;

        try {

            edm::Handle< reco::CandViewMatchMap > PFToGenJetMaphandle;

            iEvent.getByLabel(PFToGenJetMap_name,"rec2gen",PFToGenJetMaphandle);

            reco::CandViewMatchMap PFToGenJetMap(*PFToGenJetMaphandle);

            candGenJetRef = PFToGenJetMap[jetbaseref];
//             std::cout << " SUCCESS \n";
        }

        catch (...) {
            LogDebug("Match map") << "Map used for the matching not found!!!";
//             std::cout << " FAIL\n";
            return 0;
        }

        return candGenJetRef.get();

    }
    else return 0;
}


//------------------------------------------------------------------------------

void  ZplusjetTreeMaker::FillTriggerInfo(const edm::Event& iEvent,const edm::EventSetup& iEventSetup)
{
    LogDebug( "Trigger" ) << "Filling Trigger information";
    // HLT information first
    edm::Handle<edm::TriggerResults> triggerResults;

    bool productFound = iEvent.getByLabel( m_triggerResultsTag, triggerResults );
    m_HLTriggers_fired = new TClonesArray("TObjString");
    if ( productFound )
    {
        unsigned int iAcceptedHLTriggers( 0 );
        if ( triggerResults.product()->wasrun() )
        {
            if ( triggerResults.product()->accept() )
            {
                const unsigned int n_TriggerResults( triggerResults.product()->size() );
                for ( unsigned int itrig( 0 ); itrig < n_TriggerResults; ++itrig )
                {
                    if ( triggerResults.product()->accept( itrig ) )
                    {
                        // save name of accepted trigger path
                        new((*m_HLTriggers_fired)[iAcceptedHLTriggers]) TObjString( (iEvent.triggerNames(*(triggerResults.product())).triggerName( itrig )).c_str() );
                        LogDebug( "Trigger" ) << "HLT Trigger path " << (iEvent.triggerNames(*(triggerResults.product())).triggerName( itrig )).c_str() << " has accepted the event.";
                        ++iAcceptedHLTriggers;
                    }
                }
            }
        }
    }
    else
    {
        edm::LogWarning("Trigger") << "Trigger information " << m_triggerResultsTag << " not found in Event";
    }
}
//------------------------------------------------------------------------------

/**
Use the information of pythia matrix element to find the sister of the Z.
**/
int ZplusjetTreeMaker::m_balancing_parton_flavour(const edm::Event& iEvent) {

    try
    {
        edm::Handle< std::vector<reco::GenParticle> > genparticles;
        iEvent.getByLabel("genParticles", genparticles);

        LogDebug("balancing flavour ME")  << "N of particles = " << (*genparticles).size()  << std::endl;

        int status=0;
        unsigned int stat3parts=0;
        int balancing_parton_pdgId=0;

        // The idea here is to check the mothers of the Jet, then chek the daughters
        // of the mothers, and see the brother of the Z. RedunDANT BUT UNBREAKABLE.

        for (std::vector<reco::GenParticle>::const_iterator partit=genparticles->begin();
                partit!=genparticles->end(); ++partit) {

            status = partit->status();

            // check status 3 and the Z
            if (status!=3 or partit->pdgId() !=Z_pdgId )
                continue;

//         std::cout << "Particle info:\n"
//                   << "* Status " << partit->status() << "\n"
//                   << "* PdgId " << partit->pdgId() << "\n"
//                   << "* Daughters " << real_number_of_dau<< "\n";
//         for (int dau_i=0;dau_i<real_number_of_dau;++dau_i)
//             std::cout << "   - Daughter " << dau_i << " " << partit->daughter(dau_i)->pdgId() << "\n";

            int number_of_mot= partit->numberOfMothers();

            if (number_of_mot!=2)
                return 0;

//         std::cout << "* Mothers " << number_of_mot << "\n";
//         for (int mot_i=0;mot_i<number_of_mot;++mot_i)
//             std::cout << "   - Mother " << mot_i << " " << partit->mother(mot_i)->pdgId() << "\n";
//         std::cout << "* Daughters of the Mothers ;) " << partit->mother(0)->numberOfDaughters() << " - "  << partit->mother(1)->numberOfDaughters() << "\n";


            // Loop on the daughters of the mothers to find the actual sister
            int dummy_id=0;
            for (int mot_i=0;mot_i<2;++mot_i)
                for (unsigned int dau_i=0;dau_i < partit->mother(mot_i)->numberOfDaughters();++dau_i) {
                    dummy_id=partit->mother(mot_i)->daughter(dau_i)->pdgId();
                    //std::cout << "   - Daughter "<< dau_i << " of mother " << mot_i << " " << dummy_id << "\n";
                    if (dummy_id != Z_pdgId) {
                        balancing_parton_pdgId=dummy_id;
                        continue;
                    }
                }

            LogDebug("balancing flavour ME")  << "Balancing Parton flavour = " << balancing_parton_pdgId << std::endl;

            stat3parts++;
        }
        return balancing_parton_pdgId;

    }
    catch (...)
    {
//	isFail = true;
        // don't use flavour when running on real data
        return 0;
    }
}

//------------------------------------------------------------------------------

//define this as a plug-in
DEFINE_FWK_MODULE(ZplusjetTreeMaker);
