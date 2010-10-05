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


/* BASIC CONFIGURATION */

// DP made the variables not const to set them by command line args

//const TString g_sDataSource("/scratch/hh/lustre/cms/user/hauth/Zplusjet/data_job*.root");
//const std::string g_sDataSource = "/local/scratch/hauth/data/Zplusjet_2010_08_16/data_job*.root";
std::string g_sSource ("");
//const TString g_sDataSource("/local/scratch/hauth/data/Zplusjet_2010_08_16/mc_job*.root");
bool g_doMc = true;
bool g_doData = true;

//bool g_doL1Correction = true;
bool g_doL2Correction = true;
bool g_doL3Correction = false;
vString g_l2CorrFiles;
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

TString g_sOutFolder("out/");
boost::scoped_ptr<Json_wrapper>  g_json;

evtData g_ev;
TChain * g_pChain;

EventSet g_trackedEvents;

//EventSet g_eventsInCut;
EventVector g_eventsDataset;

const double g_kZmass = 91.19;

const double g_kCutZmassWindow = 20.0; // +/-
const double g_kCutMuPt = 15.0; // Mu.Pt() > 15 !
const double g_kCutMuEta = 2.3;
const double g_kCutLeadingJetEta = 1.3;
const double g_kCut2ndJetToZPt = 0.2; // 2nd leading jet to Z pt
const double g_kCutBackToBack = 0.2; // 2nd leading jet to Z pt

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

bool applyCut( EventResult * pEv, bool bUseJson = true)
{
    // check if json is valid
    if ((bUseJson) && (! g_json->has( pEv->m_pData->cmsRun,  pEv->m_pData->luminosityBlock)))
    {
        pEv->m_sCutResult = "json file";
        pEv->m_sCutUsed = "1) invalidated by json file";
        pEv->m_cutResult = NotInJson;
        return false;
    }

    // muon pt cut
    if ( !( (  pEv->m_pData->mu_plus->Pt() > g_kCutMuPt ) && ( pEv->m_pData->mu_minus->Pt() > g_kCutMuPt ) ))
    {
        pEv->m_sCutResult.Form( "muon pt cut, m_plus pt: %.4f m_minus pt: %.4f", pEv->m_pData->mu_plus->Pt(), pEv->m_pData->mu_minus->Pt() );
        pEv->m_sCutUsed = "2) muon pt cut";
        pEv->m_cutResult = NotInCutParameters;
        return false;
    }

    // muon eta cut
    if (!( (TMath::Abs( pEv->m_pData->mu_plus->Eta()) < g_kCutMuEta) && ( TMath::Abs( pEv->m_pData->mu_minus->Eta() ) <  g_kCutMuEta)))
    {
        pEv->m_sCutResult.Form( "muon eta cut, m_plus eta: %.4f  m_minus eta: %.4f",  pEv->m_pData->mu_plus->Eta(), pEv->m_pData->mu_minus->Eta() );
        pEv->m_sCutUsed = "3) muon eta cut";
        pEv->m_cutResult = NotInCutParameters;
        return false;
    }

    // leading jet eta cut
    if (!(TMath::Abs( pEv->m_pData->jets[0]->Eta()) < g_kCutLeadingJetEta))
    {
        pEv->m_sCutResult.Form( "jet1 eta cut, jet1 eta: %.4f",  pEv->m_pData->jets[0]->Eta() );
        pEv->m_sCutUsed = "4) leading jet eta cut";
        pEv->m_cutResult = NotInCutParameters;
        return false;
    }

    // 2nd leading jet to Z pt
    if (!(pEv->GetCorrectedJetPt(1)/pEv->m_pData->Z->Pt() < g_kCut2ndJetToZPt))
    {
        pEv->m_sCutResult.Form( "jet2 pt to z pt cut, jet2 pt: %.4f   Z pt: %.4f ; jet2/Z  %.4f",  pEv->GetCorrectedJetPt(1), pEv->m_pData->Z->Pt(), pEv->GetCorrectedJetPt(1)/pEv->m_pData->Z->Pt() );
        pEv->m_sCutUsed = "5) 2nd leading jet to Z pt";
        pEv->m_cutResult = NotInCutParameters;
        return false;
    }

    // back to back between jet and z
    if (!(TMath::Abs( TMath::Abs(pEv->m_pData->jets[0]->Phi() - pEv->m_pData->Z->Phi()) - TMath::Pi()) < g_kCutBackToBack))
    {
        pEv->m_sCutResult.Form( "back to back between jet1 and z; jet1 phi : %.4f   Z phi:: %.4f",  pEv->m_pData->jets[0]->Phi(), pEv->m_pData->Z->Phi() );
        pEv->m_sCutUsed = "6) back to back/jet to z";
        pEv->m_cutResult = NotInCutParameters;
        return false;
    }

    // M_zmeassured only deviates by 20 GeV from z rest mass
    if (!(TMath::Abs(pEv->m_pData->Z->GetCalcMass() - g_kZmass ) < g_kCutZmassWindow ))
    {
        pEv->m_sCutResult.Form( "Z Mass cut, Zmass: %.4f",  pEv->m_pData->Z->GetCalcMass() );
        pEv->m_sCutUsed = "7) z mass window";
        pEv->m_cutResult = NotInCutParameters;
        return false;
    }

    pEv->m_sCutResult = "within cut";
    pEv->m_sCutUsed = "8) within cut";
    pEv->m_cutResult = InCut;
    return true;
}

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
//	std::cout << "JetCorr " << jetcorr << std::endl;
            //res->m_pData->jets[i]-> = res->m_pData->jets[i]->Pt()*jetcorr;
        }

        if ( g_doL3Correction )
        {
            // more magic to come !
        }*/
    }
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
#include "../interface/CompleteJetCorrector.h"
                    std::cout << "Excluded Event found 2 times, this is usually *NOT* good." << std::endl;
                    exit(10);
                }
                pEx->m_eventFound = true;
                bUseEvent = false;
                std::cout << "Excluded Event due to exclusion list." << std::endl;

                break;
            }
        }

        if ( bUseEvent )
        {
            EventResult * res = new EventResult;

            res->m_pData = g_ev.Clone();


            // either keep it or kick it
            //if ( res->IsInCut() || ( ! bDiscardOutOfCutEvents ))
            //{
            calcJetEnergyCorrection(res, correction);
            applyCut( res, bUseJson );
	    g_eventsDataset.push_back( res );
	    
/*            }
            else
            {
                delete res;
            }*/
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
            iter->m_weight = g_mcWeighter.GetWeightByXSection( iter->m_pData->xsection );
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

    // scalars
    mychain->SetBranchAddress("cmsEventNum",&pEv->cmsEventNum);
    mychain->SetBranchAddress("cmsRun",&pEv->cmsRun);
    mychain->SetBranchAddress("luminosityBlock",&pEv->luminosityBlock);
    mychain->SetBranchAddress("xsection",&pEv->xsection);

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

void ReapplyCut( bool bUseJson, bool useL2Corr, bool useL3Corr)
{
  
    for ( EventVector::iterator iter = g_eventsDataset.begin();
            !(iter == g_eventsDataset.end());
            ++iter )
    {
      iter->m_bUseL2 = useL2Corr;
      iter->m_bUseL3 = useL3Corr;

      applyCut( &*iter, bUseJson );
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
(g_lOverallNumberOfProcessedEvents - g_eventsDataset.size())<< std::endl;

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

    TH1D* hCut2ndJetToZPt = new TH1D("cut_" +  algoName + "_Cut2ndJetToZPt",
                                     "cut_" +  algoName + "_Cut2ndJetToZPt",
                                     1, g_kCut2ndJetToZPt, g_kCut2ndJetToZPt);
    hCut2ndJetToZPt->Fill( g_kCut2ndJetToZPt );

    TH1D* hCutBackToBack = new TH1D("cut_" +  algoName + "_CutBackToBack",
                                    "cut_" +  algoName + "_CutBackToBack",
                                    1, g_kCutBackToBack, g_kCutBackToBack);
    hCutBackToBack->Fill( g_kCutBackToBack );

    pFileOut->cd();
    hCutZmassWindow->Write();
    hCutMuPt->Write( );
    hCutMuEta->Write(  );
    hCutLeadingJetEta->Write(  );
    hCut2ndJetToZPt->Write(  );
    hCutBackToBack->Write(  );

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
    CHistDrawBase jet2toZ( "jet2toZ_" + algoName+ sPostfix,
                           pFileOut);
    jet2toZ.AddModifier(new CModHorizontalLine( g_kCut2ndJetToZPt ));
    jet2toZ.AddModifier(new CModBinRange(0.0, 2.0));

    CHistEvtData2ndJetToZPt jet2toZ_draw;
    ModEvtDraw( &jet2toZ_draw, useCutParameter, bPtCut, ptLow, ptHigh );
    jet2toZ.Execute < EventVector & > ( g_eventsDataset, &jet2toZ_draw );

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

    // Jet Response binned as jet1.pt() here ??

    
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
    drawHistoBins(sName, sPrefix + "_nocut", g_resFile.get(), false);

    PrintCutReport( std::cout );
    PrintCutReport( *g_logFile );

    if (g_doData)
    {
        WriteSelectedEvents(sName, sPrefix, g_eventsDataset, g_resFile.get() );
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
	
	ReapplyCut(g_doData, true, false);
	
        drawHistoBins(sName, sPrefix + "_l2corr", g_resFile.get(),  true);
        drawHistoBins(sName, sPrefix + "_l2corr_nocut", g_resFile.get(), false);
	
	PrintCutReport( std::cout );
	PrintCutReport( *g_logFile );

	if (g_doData)
	{
	    WriteSelectedEvents(sName, sPrefix + "_l2corr", g_eventsDataset, g_resFile.get() );
	    PrintEventsReport(std::cout, true);
	    PrintEventsReport(*g_logFile, true);
	}	
    }

//    drawJetResponsePlots( sName, g_resFile.get() );
    WriteCuts( sName, g_resFile.get() );


}

void CreateWeightBins()
{
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

    // this events are excluded from monte carlo since they are on the border of bins and are
    // sorted in the wrong bin on re-binning
    // for MC dataset /ZmumuJet_PtXX/Summer10-START36_V9_S09-v1/GEN-SIM-RECO
    // two spikes which show up in zPt Plot but have no influence on jet response plots
    // therefore this lines are commented
//    g_mcExcludedEvents.push_back( new ExcludedEvent(73.62, 49.91, 3.452, 7.6321, 0.6349 ));
//    g_mcExcludedEvents.push_back( new ExcludedEvent(129.2, 43.37, 4.349, 25.64, 4.564 ));

    // this events have a very bad influence on jet response plots
// DP
//      g_mcExcludedEvents.push_back( new ExcludedEvent(47.29, 71.39, 0.1809, 4.469,1.463 ));
//      g_mcExcludedEvents.push_back( new ExcludedEvent(28.9, 43.92, 0.9386, 3.48, 1.973 ));
    /*
     algoList.insert("ak5PFJets_Zplusjet");
     algoList.insert("ak7PFJets_Zplusjet");
     algoList.insert("kt4PFJets_Zplusjet");
     algoList.insert("kt6PFJets_Zplusjet");
     algoList.insert("iterativeCone5PFJets_Zplusjet");

     // GEN
     algoList.insert("kt4GenJets_Zplusjet");
     algoList.insert("kt6GenJets_Zplusjet");
     algoList.insert("ak5GenJets_Zplusjet");
     algoList.insert("ak7GenJets_Zplusjet");
     algoList.insert("iterativeCone5GenJets_Zplusjet");

    // GEN
/*    algoList.insert("kt4GenJets_Zplusjet");
    algoList.insert("kt6GenJets_Zplusjet");
    algoList.insert("ak5GenJets_Zplusjet");
    algoList.insert("ak7GenJets_Zplusjet");
    algoList.insert("iterativeCone5GenJets_Zplusjet");
*/
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

void fail()
{
    std::cout << "Fail " << std::endl;
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
    
    g_writeEventsSetting = NoEvents;
    
    if (  p.getString(secname + ".write_events" ) == "incut" )
      g_writeEventsSetting = OnlyInCutEvents;
    if (  p.getString(secname + ".write_events" ) == "all" )
      g_writeEventsSetting = AllEvents;
    

    g_doL2Correction = (bool) p.getInt( secname + ".do_l2_correction" );
    g_doL3Correction = (bool) p.getInt( secname + ".do_l3_correction" );

    g_doData = (bool) p.getInt( secname + ".is_data" );
    g_doMc = ! g_doData;

    g_sOutputPath = p.getString(secname + ".output_path");
    g_sSource = p.getString(secname + ".tchain_file_path");

    std::string sLogFileName;
    sLogFileName =  g_sOutputPath + ".log";

    //g_logFile = ofstream(  sLogFileName.c_str(), ios_base::trunc );
    g_logFile.reset (new ofstream(  sLogFileName.c_str(), ios_base::trunc ));

    if (g_doData) {
        g_sJsonFile = p.getString(secname + ".json_file");
        g_json.reset( new Json_wrapper( g_sJsonFile.c_str() ));
    }

    std::set < std::string > myAlgoList;
    vString algoList = p.getvString(secname + ".process_algos");

    BOOST_FOREACH( TString sAlgo, algoList )
    {
        myAlgoList.insert( (  sAlgo + "Jets_Zplusjet" ).Data() );
    }

    g_l2CorrFiles = p.getvString(secname + ".l2_correction_data");

    resp_cuts(myAlgoList, g_sOutputPath + ".root");

    g_logFile->close();

    return 0;
}
