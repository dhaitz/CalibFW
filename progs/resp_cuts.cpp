#include "RootIncludes.h"

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>

// this include will overload comparison headers automatically
#include <utility>
using namespace std::rel_ops;


#include "MinimalParser.h"

#include "CompleteJetCorrector.h"

/*
#include <iostream>
#include <hash_map>
*/
#include "Json_wrapper.h"
#include "read_csv.h"
#include "EventData.h"

#include "DrawBase.h"
#include "PtBinWeighter.h"
#include "CutHandler.h"

/* BASIC CONFIGURATION */

// DP made the variables not const to set them by command line args

//const TString g_sDataSource("/scratch/hh/lustre/cms/user/hauth/Zplusjet/data_job*.root");
//const std::string g_sDataSource = "/local/scratch/hauth/data/Zplusjet_2010_08_16/data_job*.root";
std::string g_sSource ("");
//const TString g_sDataSource("/local/scratch/hauth/data/Zplusjet_2010_08_16/mc_job*.root");
bool g_doMc = true;
bool g_doData = true;


// if true, the number in Event.weight is used for MC weigting
bool g_useEventWeight = false;

// check if an event has certain hltrigger
bool g_useHLT = false;

// if true, all plots are done one time without cuts applied
bool g_plotNoCuts = false;
bool g_plotCutEff = false;

bool g_doL2Correction = true;
bool g_doL3CorrectionFormula = false;
bool g_doL3Correction = false;

vString g_l2CorrFiles;
vString g_l3CorrFiles;

TString g_l3Formula;
vdouble g_l3FormulaParams;

vdouble g_customBinning;

enum WriteEventsEnum { NoEvents, OnlyInCutEvents, AllEvents };
WriteEventsEnum g_writeEventsSetting;

//const TString g_sJsonFile("Cert_139779-140159_7TeV_July16thReReco_Collisions10_JSON.txt");
std::string g_sJsonFile("not set");
std::string g_sOutputPath = "default_zjetres";

boost::scoped_ptr<std::ofstream> g_logFile;

long g_lOverallNumberOfProcessedEvents = 0;

std::map< std::string, std::string > g_l2CorrData;

/*
class CHistEvtCount : public  CHistDataDrawBase< PtBinWeighter * >
{
public:

    virtual void Draw( TH1D * pHist, PtBinWeighter * pData )
    {
        std::vector<PtBinWeight>::iterator it;

        for ( it = pData->m_weights.begin(); it != pData->m_weights.end(); ++it)
        {
            //pHist->Fill
        }
    }
};
*/

class ExcludedEvent
{
public:
    ExcludedEvent( double zpt, double jet1pt, double jet1phi, double jet2pt, double jet2phi)
    {
        m_eventFound = false;
        m_zpt = zpt;
        m_jet1pt = jet1pt;
        m_jet1phi = jet1phi;
        m_jet2pt = jet2pt;
        m_jet2phi = jet2phi;
    }

    double m_zpt;
    double m_jet1pt;
    double m_jet1phi;
    double m_jet2pt;
    double m_jet2phi;

    bool m_eventFound;

    bool MatchesEvent ( evtData * pRealEvent )
    {
        if ( ! FLOAT_COMPARE( pRealEvent->Z->Pt(), m_zpt ))
            return false;
        if ( ! FLOAT_COMPARE( pRealEvent->jets[0]->Pt(), m_jet1pt ))
            return false;
        if ( ! FLOAT_COMPARE( pRealEvent->jets[0]->Phi(), m_jet1phi ))
            return false;
        if ( ! FLOAT_COMPARE( pRealEvent->jets[1]->Pt(), m_jet2pt ))
            return false;
        if ( ! FLOAT_COMPARE( pRealEvent->jets[1]->Phi(), m_jet2phi ))
            return false;

        return true;
    }
};

vString g_lCorrFiles;
std::vector<ExcludedEvent * > g_mcExcludedEvents;

boost::scoped_ptr<TFile> g_resFile;

CutHandler g_cutHandler;

TString g_sOutFolder("out/");
boost::shared_ptr<Json_wrapper>  g_json;

evtData g_ev;
TChain * g_pChain;

EventSet g_trackedEvents;

//EventSet g_eventsInCut;
EventVector g_eventsDataset;



PtBinWeighter g_mcWeighter;


// set via config file
boost::ptr_vector<PtBin> g_newPtBins;

/* Danillos Custom Binning*/
/*[] = {
    PtBin(0.0, 25.0),
    PtBin(25.0, 30.0),
    PtBin(30.0, 36.0),
    PtBin(36.0, 43.0),
    PtBin(43.0, 51.0),
    PtBin(51.0, 61.0),
    PtBin(61.0, 73.0),
    PtBin(73.0, 87.0),
    PtBin(87.0, 104.0),
    PtBin(104.0, 124.0),
    PtBin(124.0, 148.0),
    PtBin(148.0, 177.0),
    PtBin(177.0, 212.0),
    PtBin(212.0, 254.0),
    PtBin(254.0, 304.0),
    PtBin(304.0, 364.0)
};

0.0, 25.0, 30.0,  36.0, 36.0, 43.0, 51.0, 61.0, 73.0,87.0, 104.0, 124.0, 148.0, 177.0 212.0, 254.0, 304.0, 364.0
/* MC Sample binning */
/*
PtBin g_newPtBins[] = {
		PtBin(0.0, 15.0),
		PtBin(15.0, 20.0),
		PtBin(20.0, 30.0),
		PtBin(30.0, 50.0),
		PtBin(50.0, 80.0),
		PtBin(80.0, 120.0),
		PtBin(120.0, 170.0),
		PtBin(170.0, 230.0),
		PtBin(230.0, 300.0),
		PtBin(300.0, 99999.0)
};*/

void calcJetEnergyCorrection( EventResult * res, CompleteJetCorrector *  pJetCorr )
{
    for ( int i = 0; i < 3; i++ )
    {
        pJetCorr->CalcCorrectionForEvent( res );
        /*
          if ( g_doL2Correction )
          {
              pJetCorr->setJetEta(res->m_pData->jets[i]->Eta());
              pJetCorr->setJetPt(res->m_pData->jets[i]->Pt());
          res->m_l2CorrJets[i] = pJetCorr->getCorrection();
        //    std::cout << "JetCorr " << jetcorr << std::endl;
              //res->m_pData->jets[i]-> = res->m_pData->jets[i]->Pt()*jetcorr;
          }

          if ( g_doL3Correction )
          {
              // more magic to come !
          }*/
    }
}

bool IsEventHltAccepted( evtData & evt )
{
    TString hltName = "HLT_Mu9";

    /* 1 trigger approach */
    if ( evt.cmsRun >= 147146  )
        hltName = "HLT_Mu15_v1";

    /*
     * 2 trigger approach old
    if ( evt.cmsRun > 147196  )
        hltName = "HLT_Mu11";
    if ( evt.cmsRun > 148108 )
        hltName = "HLT_Mu15_v1";
*/
    const int nHLTriggers = evt.HLTriggers_accept->GetEntries();

    if ( nHLTriggers == 0) {
        std::cout<<"No HLT Trigger in Event! \n";
        exit(0);
    }

    TObjString *theHLTbit = NULL;
    //std::cout << "Checking HLT of Event " << std::endl;

    for (int i=0; i<nHLTriggers; ++i) {
	
	theHLTbit = (TObjString*) evt.HLTriggers_accept->At(i);
	TString curName = theHLTbit->GetString();
	//std::cout << "HLT " << curName.Data() << " included" << std::endl;
        
        
        if (hltName == curName)
	{
	    //std::cout << "!! HLT trigger " << curName.Data() << " matched" << std::endl;
            return true;
	}
    }

    return false;
}

void importEvents( bool bUseJson,
                   bool bApplyWeighting,
                   std::vector<ExcludedEvent *> exludeEventsByValue,
                   bool bDiscardOutOfCutEvents,
                   CompleteJetCorrector * correction // can be null
                 )
{
    int entries=g_pChain->GetEntries();

    TString  sNewFile = "";
    bool bUseEvent;
    long lProcEvents = 0;
    g_lOverallNumberOfProcessedEvents = 0;

    std::cout << "Processing " << entries << " events ...";
    for (Long_t ievt=0; ievt < entries ;++ievt)
    {
        bUseEvent = true;
        g_pChain->GetEntry(ievt);

        // check if this is an excluded event
        BOOST_FOREACH( ExcludedEvent * pEx, exludeEventsByValue)
        {
            if ( pEx->MatchesEvent( &g_ev ) )
            {
                if ( pEx->m_eventFound )
                {
                    std::cout << "Excluded Event found 2 times, this is usually *NOT* good." << std::endl;
                    exit(10);
                }
                pEx->m_eventFound = true;
                bUseEvent = false;
                std::cout << "Excluded Event due to exclusion list." << std::endl;

                break;
            }
        }

        // check if this event matches our hlt trigger criteria
        if ( bUseEvent && g_useHLT )
        {
            bUseEvent = IsEventHltAccepted( g_ev );
        }

        if ( bUseEvent )
        {
            EventResult * res = new EventResult;

            res->m_pData = g_ev.Clone();
            // either keep it or kick it
            //if ( res->IsInCut() || ( ! bDiscardOutOfCutEvents ))
            //{
            calcJetEnergyCorrection(res, correction);

            g_cutHandler.SetEnableCut( JsonCut::CudId, bUseJson );
            g_cutHandler.ApplyCuts( res );

            g_eventsDataset.push_back( res );
        }

        // build the weighting data
        if ( sNewFile != g_pChain->GetCurrentFile()->GetName())
        {
            //std::cout << "old file: " << sNewFile.Data() << std::endl;
            // new file opened, check the processed event
            // we assume all events in one root file have got the same xsection
            sNewFile = g_pChain->GetCurrentFile()->GetName();
            TH1F * pH = (TH1F *) g_pChain->GetCurrentFile()->Get("number_of_processed_events");

            lProcEvents = TMath::Nint( pH->GetMean());
            std::cout << "new file: " <<  sNewFile.Data() << " number " << lProcEvents << std::endl;
            g_lOverallNumberOfProcessedEvents += lProcEvents;

            g_mcWeighter.IncreaseCountByXSection( g_ev.xsection,
                                                  TMath::Nint( pH->GetMean()));
        }
    }

    // ensure all exluded events have been found
    BOOST_FOREACH( ExcludedEvent * pEx, exludeEventsByValue)
    {
        if (  ! pEx->m_eventFound )
        {
            std::cout << "Not all excluded events have been found !" << std::endl;
            exit(0);
        }
    }

    std::cout << "done" << std::endl;

    if ( bApplyWeighting)
    {
        std::cout << "Applying weighting ... ";
        for ( EventVector::iterator iter = g_eventsDataset.begin();
                !(iter == g_eventsDataset.end());
                ++iter)
        {
            //EventId myid = (*iter).first;
            //EventResult res = (*iter).second;
            //(*iter).second.m_weight = g_mcWeighter.GetWeightByZPt( (*iter).second.m_pData->Z->Pt() );
            //std::cout << "run " << iter->m_cmsRun << " num " << iter->m_cmsEventNum << std::endl;

            if ( g_useEventWeight )
            {
                iter->m_weight = iter->m_pData->weight;
            }
            else
            {
                iter->m_weight = g_mcWeighter.GetWeightByXSection( iter->m_pData->xsection );
            }
            //std::cout << "W : "<< (*iter).second.m_weight << " xsec: " << iter->second.m_pData->xsection << std::endl;
        }

        std::cout << "done" << std::endl;
    }

    // sort by evt/run number
    g_eventsDataset.sort( CompareEventResult() );

}

TChain * getChain( TString sName, evtData * pEv, std::string sRootfiles)
{
    TChain * mychain = new TChain( sName);

    pEv->Z=new TParticle();
    pEv->jets[0]=new TParticle();
    pEv->jets[1]=new TParticle();
    pEv->jets[2]=new TParticle();
    pEv->mu_minus=new TParticle();
    pEv->mu_plus=new TParticle();
    
    pEv->met=new TParticle();
    pEv->tcmet=new TParticle();
    
    pEv->HLTriggers_accept = new TClonesArray("TObjString");

    int addedfiles = 0;

    addedfiles = mychain->Add(sRootfiles.c_str());

    std::cout << addedfiles << " file(s) added to TChain";

    // improves I/O
    // disable all branches
    /*
    mychain->SetBranchStatus("*",0);

    // only enable the one we need
    mychain->SetBranchStatus("Z*",1);
    mychain->SetBranchStatus("jet*",1);
    mychain->SetBranchStatus("jet2*",1);
    mychain->SetBranchStatus("jet3*",2);
    mychain->SetBranchStatus("mu_plus*",1);
    mychain->SetBranchStatus("mu_minus*",1);
    mychain->SetBranchStatus("cmsEventNum",1);
    mychain->SetBranchStatus("cmsRun",1);
    mychain->SetBranchStatus("luminosityBlock",1);
    mychain->SetBranchStatus("xsection",1);
    */

    // TParticles
    mychain->SetBranchAddress("Z",&pEv->Z);
    mychain->SetBranchAddress("jet",&pEv->jets[0]);
    mychain->SetBranchAddress("jet2",&pEv->jets[1]);
    mychain->SetBranchAddress("jet3",&pEv->jets[2]);
    mychain->SetBranchAddress("mu_plus",&pEv->mu_plus);
    mychain->SetBranchAddress("mu_minus",&pEv->mu_minus);
    
    mychain->SetBranchAddress("met",&pEv->met);
    mychain->SetBranchAddress("tcmet",&pEv->tcmet);

    // Triggers
    mychain->SetBranchAddress("HLTriggers_accept",&pEv->HLTriggers_accept);

    // Vertex Info
    mychain->SetBranchAddress("recoVertices",&pEv->recoVertices);
    mychain->SetBranchAddress("recoVerticesInfo",&pEv->recoVerticesInfo);
    mychain->SetBranchAddress("recoVerticesError",&pEv->recoVerticesError);
    
    // scalars
    mychain->SetBranchAddress("cmsEventNum",&pEv->cmsEventNum);
    mychain->SetBranchAddress("cmsRun",&pEv->cmsRun);
    mychain->SetBranchAddress("luminosityBlock",&pEv->luminosityBlock);
    mychain->SetBranchAddress("xsection",&pEv->xsection);
    mychain->SetBranchAddress("weight",&pEv->weight);
    
    mychain->SetBranchAddress("beamSpot",&pEv->beamSpot);

    return mychain;
}

inline void PrintEvent( EventResult & data, std::ostream & out, EventFormater * p = NULL, bool bAddNewline = true )
{
    EventFormater * pForm = p;

    if (pForm == NULL)
        pForm =  new EventFormater();

    pForm->FormatEventResultCorrected(out, &data);

    if ( bAddNewline)
        out << std::endl;

    if ( p == NULL )
        // own formater created, delete it again
        delete pForm;
}

void ReapplyCut( bool bUseJson)
{
    for ( EventVector::iterator iter = g_eventsDataset.begin();
            !(iter == g_eventsDataset.end());
            ++iter )
    {
//        applyCut( &*iter, bUseJson );
        g_cutHandler.SetEnableCut( JsonCut::CudId, bUseJson );
        g_cutHandler.ApplyCuts( &*iter );
    }
}

void PrintCutReport( std::ostream & out)
{
    std::map< std::string, long > CountMap;

    for ( EventVector::iterator iter = g_eventsDataset.begin();
            !(iter == g_eventsDataset.end());
            ++iter )
    {
        CountMap[ iter->m_sCutUsed ]++;
    }

    out << std::setprecision(3) << std::fixed ;
    out  << "--- Event Cut Report ---" << std::endl;
    out  << std::setw(35) << "CutName" << std::setw(20) << "EvtsLeftRel [%]" << std::setw(20)<< "EvtsLeft" << std::setw(20)<< "EvtsDropRel [%]"<< std::setw(20)  << "EvtsDropAbs"   << std::setw(20)
    <<
    std::endl;

/// todo: actually use the processed events from root file

    long overallCountLeft = g_lOverallNumberOfProcessedEvents;
    //long refCount = g_eventsDataset.size();
    double droppedRel;

    out  << std::setw(35) <<  "NumberOfProcessedEvents" <<  std::setw(20) << overallCountLeft << std::endl;

    droppedRel = 1.0f -  (double) ( g_eventsDataset.size()) / (double) overallCountLeft;
    overallCountLeft = g_eventsDataset.size();
    out  << std::setw(35) <<  "0) precuts" << std::setw(20) << ( 1.0f - droppedRel ) * 100.0f << std::setw(20) << overallCountLeft << std::setw(20) << droppedRel * 100.0f <<  std::setw(20) <<
    (g_lOverallNumberOfProcessedEvents - g_eventsDataset.size()) << std::endl;

    for ( std::map< std::string, long >::iterator iter = CountMap.begin();
            !(iter == CountMap.end());
            ++iter )
    {
        droppedRel = 1.0f -(double) ( overallCountLeft - iter->second ) / (double) overallCountLeft;
        overallCountLeft -= iter->second;
        if ( iter->first == "8) within cut" )
            out  << std::setw(35) <<  iter->first << std::setw(20) <<  iter->second << std::endl;
        else
            out  << std::setw(35) <<  iter->first << std::setw(20) << ( 1.0f - droppedRel ) * 100.0f << std::setw(20) << overallCountLeft <<  std::setw(20) << droppedRel * 100.0f<<  std::setw(20)
            <<
            iter->second  << std::endl;
    }

}

void PrintTrackedEventsReport( bool bShort = false)
{
    std::set< EventId >::iterator iterTracked;
    std::map< EventId, EventResult >::iterator iter;

    EventFormater eFormat;

    eFormat.Header(std::cout);
    std::cout << std::endl;

    int iInCut = 0;
    int iOutOfCut = 0;
    int iOutOfDataset = 0;

    for ( iterTracked = g_trackedEvents.begin();
            iterTracked != g_trackedEvents.end();
            iterTracked++ )
    {
        EventId id = *iterTracked;
        /*   if ( g_eventsInCut.find( id) !=  g_eventsInCut.end() )
           {
             PrintEventById( id );
             continue;
           }
        */
        /*        if ( g_eventsDataset.find( id) !=  g_eventsDataset.end() )
                {
                    if ( !bShort )
                    {
                        PrintEventById( id, &eFormat, false );
                        std::cout << " CutResult: " << g_eventsDataset[id].m_sCutResult << std::endl;
                    }
                    if ( g_eventsDataset[id].IsInCut() )
                        iInCut++;
                    else
                        iOutOfCut++;

                    continue;
                }
                if ( !bShort )
                    std::cout << "!!>> not within dataset" << std::endl;
        */
        iOutOfDataset++;
    }

    std::cout << "In Cut: " << iInCut << "  Out of Cut: "<< iOutOfCut << "  Out of Dataset: "<< iOutOfDataset << std::endl;
}

void PrintEventsReport( std::ostream & out, bool bOnlyInCut )
{
    EventVector::iterator iterInCut;

    EventFormater eFormat;
    eFormat.Header(out);
    out << std::endl;

    out << "Events in Cut" << std::endl << std::endl;

    int i = 0;

    for ( iterInCut = g_eventsDataset.begin();
            !(iterInCut == g_eventsDataset.end());
            iterInCut++ )
    {
        if ( iterInCut->IsInCut()  || (!bOnlyInCut ) )
        {
            PrintEvent( *iterInCut, out, NULL, true);
            ++i;
        }
    }

    out << ">> " << i << " Events in Cut" << std::endl;
}

void ModEvtDraw( CHistEvtMapBase * pDraw, bool bUseCut,
                 bool bPtCut, double ptLow, double ptHigh )
{
    pDraw->m_bOnlyEventsInCut = bUseCut;
    pDraw->m_bUsePtCut = bPtCut;

    pDraw->m_dLowPtCut = ptLow;
    pDraw->m_dHighPtCut = ptHigh;

}

void WriteSelectedEvents(TString algoName, TString prefix,  EventVector & events, TFile * pFileOut )
{
    if ( g_writeEventsSetting == NoEvents )
        return;

    TTree* gentree = new TTree(algoName + prefix + "_events",
                               algoName + prefix + "_events");

    evtData localData;
    Double_t l2corr = 1.0f;
    Double_t l2corrPtJet2 = 1.0f;
    Double_t l2corrPtJet3 = 1.0f;

    localData.jets[0] = new TParticle();
    localData.Z = new TParticle();


    // more data can go here
    gentree->Branch("Z","TParticle",&localData.Z);
    gentree->Branch("jet1","TParticle",&localData.jets[0]);
    gentree->Branch("jet2","TParticle",&localData.jets[1]);
    gentree->Branch("jet3","TParticle",&localData.jets[2]);
    gentree->Branch("l2corrJet", &l2corr, "l2corrJet/D");
    gentree->Branch("l2corrPtJet2", &l2corrPtJet2, "l2corrPtJet2/D");
    gentree->Branch("l2corrPtJet3", &l2corrPtJet3, "l2corrPtJet3/D");

    gentree->Branch("cmsEventNum",&localData.cmsEventNum, "cmsEventNum/L");
    gentree->Branch("cmsRun",&localData.cmsRun, "cmsRun/L");
    gentree->Branch("luminosityBlock",&localData.luminosityBlock, "cmsRun/L");
    //  gentree->Branch("xsection",&localData.xsection,"xsection/D");

    EventVector::iterator it;
    for ( it =  events.begin();
            !(it == events.end());
            ++it)
    {
        if ( it->IsInCut() || ( g_writeEventsSetting == AllEvents ))
        {
            localData.Z = new TParticle ( *it->m_pData->Z );
            localData.jets[0] = new TParticle ( *it->m_pData->jets[0] );
            localData.jets[1] = new TParticle ( *it->m_pData->jets[1] );
            localData.jets[2] = new TParticle ( *it->m_pData->jets[2] );
            l2corr = it->m_l2CorrPtJets[0];
            l2corrPtJet2 = it->m_l2CorrPtJets[1];
            l2corrPtJet3 = it->m_l2CorrPtJets[2];

            localData.cmsEventNum  = it->m_pData->cmsEventNum;
            localData.cmsRun  = it->m_pData->cmsRun;
            localData.luminosityBlock  = it->m_pData->luminosityBlock;

            gentree->Fill();
        }
    }

    pFileOut->cd();
    gentree->Write();
}

void WriteCuts(TString algoName,  TFile * pFileOut )
{/*
  const double g_kCutZmassWindow = 20.0; // +/-
const double g_kCutMuPt = 15.0; // Mu.Pt() > 15 !
const double g_kCutMuEta = 2.3;
const double g_kCutLeadingJetEta = 1.3;
const double g_kCut2ndJetToZPt = 0.2; // 2nd leading jet to Z pt
const double g_kCutBackToBack = 0.2; // 2nd leading jet to Z pt
*/

    //TBranch * br = new TBranch(
    TH1D* hCutZmassWindow = new TH1D("cut_" +  algoName + "_CutZmassWindow",
                                     "cut_" +  algoName + "_CutZmassWindow",
                                     1, g_kCutZmassWindow, g_kCutZmassWindow);
    hCutZmassWindow->Fill( g_kCutZmassWindow );

    TH1D* hCutMuPt = new TH1D("cut_" +  algoName + "_CutMuPt",
                              "cut_" +  algoName + "_CutMuPt",
                              1, g_kCutMuPt, g_kCutMuPt);
    hCutMuPt->Fill( g_kCutMuPt );

    TH1D* hCutMuEta = new TH1D("cut_" +  algoName + "_CutMuEta",
                               "cut_" +  algoName + "_CutMuEta",
                               1, g_kCutMuEta, g_kCutMuEta);
    hCutMuEta->Fill( g_kCutMuEta );

    
    
    TH1D* hCutLeadingJetEta = new TH1D("cut_" +  algoName + "_CutLeadingJetEta",
                                       "cut_" +  algoName + "_CutLeadingJetEta",
                                       1, g_kCutLeadingJetEta, g_kCutLeadingJetEta);
    hCutLeadingJetEta->Fill( g_kCutLeadingJetEta );

    double f2ndJet = (( SecondLeadingToZPtCut *) g_cutHandler.GetById( 16 ))->m_f2ndJetRatio;
    TH1D* hCut2ndJetToZPt = new TH1D("cut_" +  algoName + "_Cut2ndJetToZPt",
                                     "cut_" +  algoName + "_Cut2ndJetToZPt",
                                     1, f2ndJet, f2ndJet);
    hCut2ndJetToZPt->Fill( f2ndJet);

    TH1D* hCutBackToBack = new TH1D("cut_" +  algoName + "_CutBackToBack",
                                    "cut_" +  algoName + "_CutBackToBack",
                                    1, g_kCutBackToBack, g_kCutBackToBack);
    hCutBackToBack->Fill( g_kCutBackToBack );

    double fZPt = (( ZPtCut *) g_cutHandler.GetById( 128 ))->m_fMinZPt;
    TH1D* hZPtCut = new TH1D("cut_" +  algoName + "_ZPt",
                                     "cut_" +  algoName + "_ZPt",
                                     1, fZPt, fZPt);
    hCut2ndJetToZPt->Fill( fZPt);

    
    pFileOut->cd();
    hCutZmassWindow->Write();
    hCutMuPt->Write( );
    hCutMuEta->Write(  );
    hCutLeadingJetEta->Write(  );
    hCut2ndJetToZPt->Write(  );
    hCutBackToBack->Write(  );
    hZPtCut->Write(  );

}

void DrawJetResponsePlots( TString algoName,
                           TFile * pFileOut )
{
    // todo here
}


void DrawHistoSet( TString algoName,
                   TString sPostfix,
                   TFile * pFileOut,
                   bool useCutParameter,
                   bool bPtCut,
                   double ptLow = 0.0,
                   double ptHigh = 0.0)
{
    std::cout << "Drawing Plots " << algoName << sPostfix << " Cuts: " << useCutParameter
              << " PtCut-low: " << ptLow << " Pt-Cut-high:" << ptHigh << std::endl;

    // ZMass with cut/
    ModifierList modList;
    modList.push_back( new CModHorizontalLine( g_kZmass - g_kCutZmassWindow ));
    modList.push_back( new CModHorizontalLine( g_kZmass + g_kCutZmassWindow));
    modList.push_back( new CModTdrStyle());

    CHistDrawBase zmass( "zmass_" + algoName + sPostfix,
                         pFileOut,
                         modList);

    CHistEvtDataZMass zdraw;
    ModEvtDraw( &zdraw, useCutParameter, bPtCut, ptLow, ptHigh );
    zmass.Execute < EventVector & > ( g_eventsDataset, &zdraw );
    // ZPt
    CHistDrawBase zPt( "zPt_" + algoName + sPostfix,
                       pFileOut);
    CHistEvtDataZPt zPtdraw;
    /* dont use dynamic binning, it is bad when comparing MC and Data
    if ( useCutParameter )
      zPt.AddModifier(new CModBinRange(ptLow - ( ptLow * .3f ), ptHigh + ( ptLow * .3f )));
    else*/
    zPt.AddModifier(new CModBinRange(0.0, 500.0));

    ModEvtDraw( &zPtdraw, useCutParameter, bPtCut, ptLow, ptHigh );
    zPt.Execute < EventVector & > ( g_eventsDataset, &zPtdraw );

  
    // MET
    CHistDrawBase met( "met_" + algoName + sPostfix,
                       pFileOut);    
    CHistEvtDataMet metdraw;
    met.AddModifier(new CModBinRange(0.0, 300.0));

    ModEvtDraw( &metdraw, useCutParameter, bPtCut, ptLow, ptHigh );
    met.Execute < EventVector & > ( g_eventsDataset, &metdraw );

    // tcMET
    CHistDrawBase tcmet( "tcmet_" + algoName + sPostfix,
                       pFileOut);    
    CHistEvtDataTcMet tcmetdraw;
    tcmet.AddModifier(new CModBinRange(0.0, 300.0));

    ModEvtDraw( &tcmetdraw, useCutParameter, bPtCut, ptLow, ptHigh );
    tcmet.Execute < EventVector & > ( g_eventsDataset, &metdraw );

    
    if ( !bPtCut )
    {
        CHistDrawBase zPtEff( "zPt_CutEff" + algoName + sPostfix,
                              pFileOut);
        CHistEvtDataZPtCutEff zPtEffdraw;
        zPtEff.AddModifier(new CModBinRange(0.0, 500.0));

        ModEvtDraw( &zPtEffdraw, useCutParameter, bPtCut, ptLow, ptHigh );
        zPtEff.Execute < EventVector & > ( g_eventsDataset, &zPtdraw );
    }

    // ZEta
    CHistDrawBase zEta( "zEta_" + algoName + sPostfix,
                        pFileOut,
                        CHistEtaMod::DefaultModifiers());
    CHistEvtDataZEta zEtadraw;
    ModEvtDraw( &zEtadraw, useCutParameter, bPtCut, ptLow, ptHigh );
    zEta.Execute < EventVector & > ( g_eventsDataset, &zEtadraw );

    // ZPhi
    CHistDrawBase zPhi( "zPhi_" + algoName+ sPostfix,
                        pFileOut,
                        CHistPhiMod::DefaultModifiers());
    CHistEvtDataZPhi zPhiDraw;
    ModEvtDraw( &zPhiDraw, useCutParameter, bPtCut, ptLow, ptHigh );
    zPhi.Execute < EventVector & > ( g_eventsDataset, &zPhiDraw  );

    // mu plus pt with cut
    modList.clear();
    modList.push_back( new CModHorizontalLine( g_kCutMuPt ));

    CHistDrawBase muplus_pt( "muplus_pt_" + algoName+ sPostfix,
                             pFileOut,	 modList);
    muplus_pt.AddModifier(new CModBinRange(0.0, 500.0));
    CHistEvtDataMuPlusPt muplus_ptdraw;
    ModEvtDraw( &muplus_ptdraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muplus_pt.Execute < EventVector & > ( g_eventsDataset, &muplus_ptdraw );

    // mu minus pt with cut
    modList.clear();
    modList.push_back( new CModHorizontalLine( g_kCutMuPt ));

    CHistDrawBase muminus_pt( "muminus_pt_" + algoName+ sPostfix,
                              pFileOut,	 modList);
    muminus_pt.AddModifier(new CModBinRange(0.0, 500.0));

    CHistEvtDataMuMinusPt muminus_ptdraw;
    ModEvtDraw( &muminus_ptdraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muminus_pt.Execute < EventVector & > ( g_eventsDataset, &muminus_ptdraw );


    // mu minus eta with cut
    CHistDrawBase muminus_eta( "muminus_eta_" + algoName+ sPostfix,  pFileOut,
                               CHistEtaMod::DefaultModifiers());
    muminus_eta.AddModifier(new CModHorizontalLine( g_kCutMuEta ));
    muminus_eta.AddModifier(new CModHorizontalLine( - g_kCutMuEta ));

    CHistEvtDataMuMinusEta muminus_etadraw;
    ModEvtDraw( &muminus_etadraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muminus_eta.Execute < EventVector & > ( g_eventsDataset, &muminus_etadraw );

    // mu plus eta with cut
    CHistDrawBase muplus_eta( "muplus_eta_" + algoName+ sPostfix,
                              pFileOut,
                              CHistEtaMod::DefaultModifiers());
    muplus_eta.AddModifier(new CModHorizontalLine( g_kCutMuEta ));
    muplus_eta.AddModifier(new CModHorizontalLine( - g_kCutMuEta ));

    CHistEvtDataMuMinusEta muplus_etadraw;
    ModEvtDraw( &muplus_etadraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muplus_eta.Execute < EventVector & > ( g_eventsDataset, &muplus_etadraw );

    // mu minus phi
    CHistDrawBase muminus_phi( "muminus_phi_" + algoName+ sPostfix,
                               pFileOut,
                               CHistPhiMod::DefaultModifiers());
    CHistEvtDataMuMinusPhi muminus_phidraw;
    ModEvtDraw( &muminus_phidraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muminus_phi.Execute < EventVector & > ( g_eventsDataset, &muminus_phidraw );

    // mu plus phi
    CHistDrawBase muplus_phi( "muplus_phi_" + algoName+ sPostfix,
                              pFileOut,
                              CHistPhiMod::DefaultModifiers());
    CHistEvtDataMuPlusPhi muplus_phidraw;
    ModEvtDraw( &muplus_phidraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muplus_phi.Execute < EventVector & > ( g_eventsDataset, &muplus_phidraw );


    // mu all pt
    modList.clear();
    modList.push_back( new CModHorizontalLine( g_kCutMuPt ));

    CHistDrawBase muall_pt( "mus_pt_" + algoName+ sPostfix,
                            pFileOut,	 modList);

    CHistEvtDataMuAllPt muall_ptdraw;
    muall_pt.AddModifier(new CModBinRange(0.0, 500.0));
    ModEvtDraw( &muall_ptdraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muall_pt.Execute < EventVector & > ( g_eventsDataset, &muall_ptdraw );

    // mu all eta
    CHistDrawBase muall_eta( "mus_eta_" + algoName+ sPostfix,  pFileOut,
                             CHistEtaMod::DefaultModifiers());

    CHistEvtDataMuAllEta muall_etadraw;
    ModEvtDraw( &muall_etadraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muall_eta.Execute < EventVector & > ( g_eventsDataset, &muall_etadraw );

    // mu all phi
    CHistDrawBase muall_phi( "mus_phi_" + algoName+ sPostfix,
                             pFileOut,
                             CHistPhiMod::DefaultModifiers());
    CHistEvtDataMuAllPhi muall_phidraw;
    ModEvtDraw( &muall_phidraw, useCutParameter, bPtCut, ptLow, ptHigh );
    muall_phi.Execute < EventVector & > ( g_eventsDataset, &muall_phidraw );

    // jet1 eta with cut
    for (int i = 0; i < 3; ++i)
    {
        std::stringstream sname ;
        sname << "jet" << (i+1);
        CHistDrawBase jet1_eta( sname.str() + "_eta_" + algoName+ sPostfix,
                                pFileOut,
                                CHistEtaMod::DefaultModifiers());

        if ( i == 0 )
        {
            jet1_eta.AddModifier(new CModHorizontalLine( g_kCutLeadingJetEta ));
            jet1_eta.AddModifier(new CModHorizontalLine( - g_kCutLeadingJetEta ));
        }

        CHistEvtDataJetEta jet1_etadraw(i);
        ModEvtDraw( &jet1_etadraw, useCutParameter, bPtCut, ptLow, ptHigh );
        jet1_eta.Execute <  EventVector & > ( g_eventsDataset, &jet1_etadraw );

        // jet pt
        CHistDrawBase jet_pt( sname.str() + "_pt_" + algoName+ sPostfix,
                              pFileOut);

        CHistEvtDataJetPt jet_ptdraw(i);

        /*    if ( useCutParameter )
              jet_pt.AddModifier(new CModBinRange(ptLow - ( ptLow * 1.2f ), ptHigh + ( ptLow * .8f )));
            else*/
        jet_pt.AddModifier(new CModBinRange(0.0, 500.0));

        ModEvtDraw( &jet_ptdraw, useCutParameter, bPtCut, ptLow, ptHigh );
        jet_pt.Execute <  EventVector & > ( g_eventsDataset, &jet_ptdraw );

        // jet phi
        CHistDrawBase jet_phi( sname.str() + "_phi_" + algoName+ sPostfix,
                               pFileOut,
                               CHistPhiMod::DefaultModifiers());

        CHistEvtDataJetPhi jet_phidraw(i);
        ModEvtDraw( &jet_phidraw, useCutParameter, bPtCut, ptLow, ptHigh );
        jet_phi.Execute < EventVector & > ( g_eventsDataset, &jet_phidraw );
    }

    // 2nd Leading Jet to Z pt
/*    CHistDrawBase jet2toZ( "jet2toZ_" + algoName+ sPostfix,
                           pFileOut);
    jet2toZ.AddModifier(new CModHorizontalLine( g_kCut2ndJetToZPt ));
    jet2toZ.AddModifier(new CModBinRange(0.0, 2.0));

    CHistEvtData2ndJetToZPt jet2toZ_draw;
    ModEvtDraw( &jet2toZ_draw, useCutParameter, bPtCut, ptLow, ptHigh );
    jet2toZ.Execute < EventVector & > ( g_eventsDataset, &jet2toZ_draw );
*/
    // zpt - jet1 pt
    CHistDrawBase jetPtzPt( "z_pt_minus_jet1_pt_" + algoName+ sPostfix,
                            pFileOut);
    jetPtzPt.AddModifier(new CModBinRange(-80.0, 80.0));
    CHistEvtDataJetPtMinusZPt jetPtzPt_draw(0);
    ModEvtDraw( &jetPtzPt_draw, useCutParameter, bPtCut, ptLow, ptHigh );
    jetPtzPt.Execute < EventVector & > ( g_eventsDataset, &jetPtzPt_draw );


    // back to back
    CHistDrawBase back2back( "back2back_" + algoName+ sPostfix,
                             pFileOut,
                             CHistEtaMod::DefaultModifiers());
    back2back.AddModifier(new CModHorizontalLine( g_kCutBackToBack));
    back2back.AddModifier(new CModBinRange(0.0, 3.5));

    CHistEvtDataBack2Back back2back_draw;
    ModEvtDraw( &back2back_draw, useCutParameter, bPtCut, ptLow, ptHigh );
    back2back.Execute <  EventVector & > ( g_eventsDataset, &back2back_draw );

    // Jet Response binned as z.pt()
    CHistDrawBase jetresp( "jetresp_" + algoName+ sPostfix,
                           pFileOut);
    jetresp.AddModifier(new CModBinRange(0.0, 2.0));

    CHistEvtDataJetResponse jetresp_draw;
    ModEvtDraw( &jetresp_draw, useCutParameter, bPtCut, ptLow, ptHigh );
    jetresp.Execute <  EventVector & > ( g_eventsDataset, &jetresp_draw );

    CHistDrawBase recovert( "recovertices_" + algoName+ sPostfix,
                           pFileOut);
    recovert.AddModifier(new CModBinRange(-0.5, 14.5));

    CHistEvtDataRecoVertices recovert_draw;
    ModEvtDraw( &recovert_draw, useCutParameter, bPtCut, ptLow, ptHigh );
    recovert.AddModifier(new CModBinCount(15));
    recovert.Execute <  EventVector & > ( g_eventsDataset, &recovert_draw );
    
    if ( g_plotCutEff )
    {
        CGrapErrorDrawBase < EventVector &,
        CGraphDrawJetResponseCutEff<PtBinEventSelector> ,
        PtBinEventSelector >  JetRespCuttEff_draw(
            "CutEffOverJetResponse_" + algoName+ sPostfix, pFileOut);
        JetRespCuttEff_draw.Execute( g_eventsDataset,
                                     PtBinEventSelector( false, // we want ALL events for this plot !!
                                                         bPtCut,
                                                         ptLow,
                                                         ptHigh  ) );

        if (! bPtCut )
        {
            int cutsCount = 10;

            for ( int i = 0; i < cutsCount; i++ )
            {
                unsigned long curId = (unsigned long) pow( 2, i );
                EventCutBase<EventResult *> * currCut = g_cutHandler.GetById(  curId );

                if ( currCut != NULL )
                {
                    CGrapErrorDrawBase < EventVector &,
                    CGraphDrawZPtCutEff<PassAllEventSelector> ,
                    PassAllEventSelector >  ZptEff_draw(
                        "CutEffOverZPt_" + algoName+ sPostfix + "_" + currCut->GetCutShortName(),
                        pFileOut);
                    ZptEff_draw.m_tdraw.m_cutBitmask = curId;
                    ZptEff_draw.Execute( g_eventsDataset,
                                         PassAllEventSelector( ));
                }
            }

            CGrapErrorDrawBase < EventVector &,
            CGraphDrawZPtCutEff<PassAllEventSelector> ,
            PassAllEventSelector >  ZptEff_draw(
                "CutEffOverZPt_" + algoName+ sPostfix+ "_overall"  , pFileOut);

            ZptEff_draw.Execute( g_eventsDataset, PassAllEventSelector());
        }
    }
    /*
    CGrapErrorDrawBase < EventVector &, CGraphDrawEvtMap< CPlotL2Corr > >  l2corr_draw( "l2corr_" + algoName+ sPostfix, pFileOut);
    l2corr_draw.Execute( g_eventsDataset );
    */
    // Jet Response binned as jet1.pt() here ??
    /*    CGrapErrorDrawBase < EventVector &, CGraphDrawEvtMap< CPlotL2Corr > >  l2corr_draw( "l2corr_" + algoName+ sPostfix, pFileOut);

        l2corr_draw.Execute( g_eventsDataset );
      */
}

void loadTrackedEventsFromFile()
{
    ReadCsv csv( "trackedEvents.txt" );
    std::vector< std::string> sRunNum = csv.ReadColumn(0);
    std::vector< std::string> sLumi = csv.ReadColumn(1);
    std::vector< std::string> sEvtNum = csv.ReadColumn(2);

    std::vector< std::string>::iterator iterRun;
    std::vector< std::string>::iterator iterLumi;
    std::vector< std::string>::iterator iterEvtNum;

    std::cout << g_trackedEvents.size() << std::endl;

    iterEvtNum = sEvtNum.begin();
    iterLumi = sLumi.begin();

    for (iterRun = sRunNum.begin();
            iterRun != sRunNum.end();
            iterRun++)
    {
        EventId evtId(	atoi( (*iterRun).c_str() ),
                       atoi( (*iterLumi).c_str() ),
                       atoi( (*iterEvtNum).c_str() ));
        g_trackedEvents.insert(evtId );
        std::cout << "Added to tracked Events " << evtId.ToString() << std::endl;
        iterEvtNum++;
        iterLumi++;
    }
    std::cout << g_trackedEvents.size() << std::endl;
}
/*
void DrawMcEventCount( std::string sAlgoName, TFile * pFileOut )
{
    // Jet Response
    CHistDrawBase evtCount( "eventcount_" + sAlgoName + "_mc",
                            pFileOut);

    CHistEvtCount evtCount_draw;
    evtCount.Execute < PtBinWeighter * > ( &g_mcWeighter, &evtCount_draw );
}*/

void drawHistoBins( std::string sName,
                    std::string tags,
                    TFile * pFileOut,
                    bool bUseCut)
{
    // cut / nocut , ptBins
    BOOST_FOREACH( PtBin & bin, g_newPtBins )
    {
        std::stringstream newTags (stringstream::in| stringstream::out);
        newTags << tags << std::setprecision(0) << std::fixed << "_Pt" << bin.m_fLowestPt << "to" << bin.m_fHighestPt;
        //tags

        DrawHistoSet( sName, newTags.str(), pFileOut , bUseCut, true,
                      bin.m_fLowestPt, bin.m_fHighestPt );
    }

    // cut / nocut without bins...
    std::stringstream newTags (stringstream::in| stringstream::out);
    DrawHistoSet( sName,tags, pFileOut, bUseCut, false );

}

void ResetExcludedEvents()
{
    // check if this is an excluded event
    BOOST_FOREACH( ExcludedEvent * pEx, g_mcExcludedEvents)
    {
        pEx->m_eventFound = false;
    }
}

void processAlgo( std::string sName )
{
    (*g_logFile) << "Processing " << sName << std::endl;
    (*g_logFile) << "uncorrected jets " << std::endl;

    CompleteJetCorrector jetCorr;

    if ( g_doL2Correction )
        jetCorr.AddCorrection( new L2Corr( TString(sName.c_str()), g_l2CorrFiles));
    if ( g_doL3Correction)
        jetCorr.AddCorrection( new L3Corr( TString(sName.c_str()), g_l3CorrFiles));
    if ( g_doL3CorrectionFormula)
        jetCorr.AddCorrection( new TF1Corr( g_l3Formula, g_l3FormulaParams) );



    ResetExcludedEvents();
    g_eventsDataset.clear();

    // reset weighting class here
    g_mcWeighter.ResetEntryCount();
    // mc
    g_pChain = getChain(sName, &g_ev, g_sSource );

    std::string sPrefix;

    if ( g_doMc )
    {
        sPrefix = "_mc";
        importEvents( false, true, g_mcExcludedEvents, false, &jetCorr );

        g_mcWeighter.Print();
    }
    if (g_doData)
    {
        sPrefix = "_data";
        importEvents( true, false, std::vector< ExcludedEvent *>(), false, &jetCorr );
    }

    delete g_pChain;

//	std::cout << "MC Weighting Report" << std::endl;
//	DrawMcEventCount( sName, g_resFileMc);

    // RAW
    drawHistoBins(sName, sPrefix , g_resFile.get(),  true);
    if ( g_plotNoCuts)
        drawHistoBins(sName, sPrefix + "_nocut", g_resFile.get(), false);

    PrintCutReport( std::cout );
    PrintCutReport( *g_logFile );

    WriteSelectedEvents(sName, sPrefix, g_eventsDataset, g_resFile.get() );
    if (g_doData)
    {        
        PrintEventsReport(std::cout, true);
        PrintEventsReport(*g_logFile, true);
    }

    // turn on l2 corr
    if  ( g_doL2Correction )
    {
        (*g_logFile) << "l2 corrected jets " << std::endl;

        for ( EventVector::iterator iter = g_eventsDataset.begin();
                !(iter == g_eventsDataset.end());
                ++iter)
        {
            iter->m_bUseL2 = true;
        }

        ReapplyCut(g_doData);

        drawHistoBins(sName, sPrefix + "_l2corr", g_resFile.get(),  true);

        if ( g_plotNoCuts)
            drawHistoBins(sName, sPrefix + "_l2corr_nocut", g_resFile.get(), false);

        PrintCutReport( std::cout );
        PrintCutReport( *g_logFile );

	WriteSelectedEvents(sName, sPrefix + "_l2corr", g_eventsDataset, g_resFile.get() );
        if (g_doData)
        {            
            PrintEventsReport(std::cout, true);
            PrintEventsReport(*g_logFile, true);
        }
    }

    // turn on l3 corr
    if  ( g_doL3Correction || g_doL3CorrectionFormula )
    {
        (*g_logFile) << "l3 corrected jets " << std::endl;

        // important: reapply cuts without the l3 correction in order to have the same data base as the l3 calculation
        ReapplyCut(g_doData);

        for ( EventVector::iterator iter = g_eventsDataset.begin();
                !(iter == g_eventsDataset.end());
                ++iter)
        {
            // l2 is already on
            iter->m_bUseL3 = true;
        }
        drawHistoBins(sName, sPrefix + "_l3corr", g_resFile.get(),  true);
        if ( g_plotNoCuts)
            drawHistoBins(sName, sPrefix + "_l3corr_nocut", g_resFile.get(), false);

        PrintCutReport( std::cout );
        PrintCutReport( *g_logFile );

	WriteSelectedEvents(sName, sPrefix + "_l3corr", g_eventsDataset, g_resFile.get() );
        if (g_doData)
        {            
            PrintEventsReport(std::cout, true);
            PrintEventsReport(*g_logFile, true);
        }
    }
//    drawJetResponsePlots( sName, g_resFile.get() );
    WriteCuts( sName, g_resFile.get() );


}

void CreateWeightBins()
{
    /*
     * Fall10 MC  */
    g_mcWeighter.AddBin( PtBin(0.0, 15.0 ), 4.281e+03  );
    g_mcWeighter.AddBin( PtBin(15.0, 20.0 ), 1.451e+02  );
    g_mcWeighter.AddBin( PtBin(20.0, 30.0 ), 1.305e+02  );
    g_mcWeighter.AddBin( PtBin(30.0, 50.0 ), 8.398e+01  );
    g_mcWeighter.AddBin( PtBin(50.0, 80.0 ), 3.224e+01  );
    g_mcWeighter.AddBin( PtBin(80.0, 120.0 ), 9.984e+00  );
    g_mcWeighter.AddBin( PtBin(120.0, 170.0 ), 2.734e+00  );
    g_mcWeighter.AddBin( PtBin(170.0, 230.0 ), 7.207e-01  );
    g_mcWeighter.AddBin( PtBin(230.0, 300.0 ), 1.939e-01  );
    g_mcWeighter.AddBin( PtBin(300.0, 999999.0 ), 7.586e-02  );

    /*
     * Spring10 MC
        g_mcWeighter.AddBin( PtBin(0.0, 15.0 ), 4.434e+03  );
        g_mcWeighter.AddBin( PtBin(15.0, 20.0 ), 1.454e+02  );
        g_mcWeighter.AddBin( PtBin(20.0, 30.0 ), 1.318e+02  );
        g_mcWeighter.AddBin( PtBin(30.0, 50.0 ), 8.438e+01  );
        g_mcWeighter.AddBin( PtBin(50.0, 80.0 ), 3.235e+01  );
        g_mcWeighter.AddBin( PtBin(80.0, 120.0 ), 9.981e+00  );
        g_mcWeighter.AddBin( PtBin(120.0, 170.0 ), 2.760e+00  );
        g_mcWeighter.AddBin( PtBin(170.0, 230.0 ), 7.241e-01  );
        g_mcWeighter.AddBin( PtBin(230.0, 300.0 ), 1.946e-01  );
        g_mcWeighter.AddBin( PtBin(300.0, 999999.0 ), 7.627e-02  );
    */
    /*
    0to15        m_ptbin_xs[0]=4.434e+03;
    15to20        m_ptbin_xs[1]=1.454e+02;
    20to30        m_ptbin_xs[2]=1.318e+02;
    30to50        m_ptbin_xs[3]=8.438e+01;
    50to80        m_ptbin_xs[4]=3.235e+01;
    80to120        m_ptbin_xs[5]=9.981e+00;
    120to170        m_ptbin_xs[6]=2.760e+00;
    170to230        m_ptbin_xs[7]=7.241e-01;
    230to300        m_ptbin_xs[8]=1.946e-01;
    300toInf        m_ptbin_xs[9]=7.627e-02;
     */
}

void resp_cuts( std::set < std::string > algoList, std::string sOutputFileName)
{
    // removes the old file
    g_resFile.reset( new TFile (sOutputFileName.c_str(), "RECREATE") );
    g_resFile->Close();

    BOOST_FOREACH( std::string algName, algoList )
    {
        g_resFile.reset( new TFile (sOutputFileName.c_str(), "UPDATE") );
        processAlgo(algName);
        g_resFile->Close();
    }
}

void fail( std::string sReason)
{
    std::cout << "Fail: " << sReason << std::endl;
    exit(02);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " python_config_file.py [VerboseLevel]\n";
        return 1;
    }

    MinimalParser p(argv[1]);
    p.setVerbose(false);

    if (argc==3) {
        int verbosityi=atoi(argv[2]);
        if (verbosityi==1) p.setVerbose(true);
    }

    TString secname("general");
//
// Section general
//
    g_customBinning = p.getvDouble(secname + ".custom_binning");
    for ( int i = 1; i < g_customBinning.size(); i++ )
    {
        g_newPtBins.push_back ( new PtBin( g_customBinning[ i-1], g_customBinning[i]));
    }


    CreateWeightBins();

    /*    for ( int i = 1; i < argc; ++i)
        {
            if ( std::string( argv[i]) == "--L3-correction" )
            {
    	  g_doL3Correction = true;
            }

        }
    */
    if ((bool) p.getInt( secname + ".fixed_weighting" ))
    {
        g_mcWeighter.Reset();
        g_mcWeighter.AddBin( PtBin(0.0, 999999.0 ), 1300  );
    }

    if ((bool) p.getInt( secname + ".use_event_weight" ))
    {
        g_useEventWeight = true;
    }


    g_writeEventsSetting = NoEvents;

    if (  p.getString(secname + ".write_events" ) == "incut" )
        g_writeEventsSetting = OnlyInCutEvents;
    if (  p.getString(secname + ".write_events" ) == "all" )
        g_writeEventsSetting = AllEvents;


    g_doL2Correction = (bool) p.getInt( secname + ".do_l2_correction" );
    g_doL3Correction = (bool) p.getInt( secname + ".do_l3_correction" );
    g_doL3CorrectionFormula = (bool) p.getInt( secname + ".do_l3_correction_formula" );

    g_plotNoCuts = (bool) p.getInt( secname + ".plot_nocuts" );
    g_plotCutEff = (bool) p.getInt( secname + ".plot_cuteff" );

    g_useHLT = (bool) p.getInt( secname + ".use_hlt" );

    g_l3Formula = p.getString( secname + ".l3_formula" );
    g_l3FormulaParams = p.getvDouble( secname + ".l3_formula_params" );


    g_doData = (bool) p.getInt( secname + ".is_data" );
    g_doMc = ! g_doData;

    g_sOutputPath = p.getString(secname + ".output_path");
    g_sSource = p.getString(secname + ".tchain_file_path");

    std::string sLogFileName;
    sLogFileName =  g_sOutputPath + ".log";

    //g_logFile = ofstream(  sLogFileName.c_str(), ios_base::trunc );
    g_logFile.reset (new ofstream(  sLogFileName.c_str(), ios_base::trunc ));

    // insert config into log file
    TString cfgName = p.getConfigFileName();

    std::ifstream cfgfile ( cfgName.Data(), std::fstream::in);
    char c;
    (*g_logFile) << "Configuration file " << cfgName.Data() << std::endl << std::endl;
    while (cfgfile.good())     // loop while extraction from file is possible
    {
        c = cfgfile.get();       // get character from file
        if (cfgfile.good())
            (*g_logFile) << c;
    }
    cfgfile.close();

    if (g_doData) {
        g_sJsonFile = p.getString(secname + ".json_file");
        g_json.reset( new Json_wrapper( g_sJsonFile.c_str() ));

        if ( ! g_json->isValid() )
            fail( "JSON File " + g_sJsonFile + " could not be loaded" );
    }

    (*g_logFile) << std::endl << std::endl;

    std::set < std::string > myAlgoList;
    vString algoList = p.getvString(secname + ".process_algos");

    BOOST_FOREACH( TString sAlgo, algoList )
    {
        myAlgoList.insert( (  sAlgo + "Jets_Zplusjet" ).Data() );
    }

    g_l2CorrFiles = p.getvString(secname + ".l2_correction_data");
    g_l3CorrFiles = p.getvString(secname + ".l3_correction_data");

    // init cuts
    g_cutHandler.AddCut( new JsonCut( g_json));
    g_cutHandler.AddCut( new MuonPtCut());
    g_cutHandler.AddCut( new MuonEtaCut());
    g_cutHandler.AddCut( new LeadingJetEtaCut());
    g_cutHandler.AddCut( new SecondLeadingToZPtCut( p.getDouble( secname + ".cut_2jet" )));
    g_cutHandler.AddCut( new BackToBackCut());
    g_cutHandler.AddCut( new ZMassWindowCut());
    g_cutHandler.AddCut( new ZPtCut(p.getDouble( secname + ".cut_zpt" )));

    resp_cuts(myAlgoList, g_sOutputPath + ".root");

    g_logFile->close();

    return 0;
}
