/*

 CalibFW resp_cuts.cpp:

 HOWTO:

 # Add a new Plot:

 - DrawBase.h
 1) Use the IMPL_HIST2D_MOD1 to generate a Consumer class which plots a histogramm. If your plotting requirement is
 more complex, derive from the class  DrawHist1dConsumerBase<EventResult> ( for 1d Histo ) and implement
 ProcessFilteredEvent.

 - resp_cuts.h: CreateDefaultPipeline
 2) Use the PLOT_HIST2D macro to add your histogram the default plotting pipeline and give a name which is used to store
 the histo in the root file.

 > done


 */
#include "RootIncludes.h"

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

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

#include "CutStatistics.h"
#include "DrawBase.h"
#include "PtBinWeighter.h"
#include "CutHandler.h"
#include "EventPipeline.h"

using namespace CalibFW;

/* BASIC CONFIGURATION */

// DP made the variables not const to set them by command line args

//const TString g_sDataSource("/scratch/hh/lustre/cms/user/hauth/Zplusjet/data_job*.root");
//const std::string g_sDataSource = "/local/scratch/hauth/data/Zplusjet_2010_08_16/data_job*.root";
std::string g_sSource("");
//const TString g_sDataSource("/local/scratch/hauth/data/Zplusjet_2010_08_16/mc_job*.root");
bool g_doMc = true;
bool g_doData = true;

// if true, the number in Event.weight is used for MC weigting
bool g_useWeighting = false;
bool g_useEventWeight = false;

// check if an event has certain hltrigger
bool g_useHLT = false;

// if true, all plots are done one time without cuts applied
bool g_plotNoCuts = false;
bool g_plotCutEff = false;

bool g_doL2Correction = true;
bool g_doL3CorrectionFormula = false;
bool g_doL3Correction = false;

bool g_useGeometricTopology = false;

vString g_l2CorrFiles;
vString g_l3CorrFiles;

// TODO: delete this pointer when done
PipelineSettings g_defaultSettings;

TString g_l3Formula;
vdouble g_l3FormulaParams;

vdouble g_customBinning;

std::string g_sCurAlgo;

enum WriteEventsEnum
{
	NoEvents, OnlyInCutEvents, AllEvents
};
WriteEventsEnum g_writeEventsSetting;

//const TString g_sJsonFile("Cert_139779-140159_7TeV_July16thReReco_Collisions10_JSON.txt");
std::string g_sJsonFile("not set");
std::string g_sOutputPath = "default_zjetres";
std::string g_sTrackedEventsFile;

boost::property_tree::ptree g_propTree;

long g_lOverallNumberOfProcessedEvents = 0;

std::map<std::string, std::string> g_l2CorrData;

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

#define PLOT_HIST1D( PIPELINE, DRAW_CONSUMER, QUANTITY_NAME) \
{ Hist1D * hist_##DRAW_CONSUMER = new Hist1D; \
DRAW_CONSUMER * object_##DRAW_CONSUMER = new  DRAW_CONSUMER(); \
object_##DRAW_CONSUMER->m_sQuantityName = #QUANTITY_NAME; \
object_##DRAW_CONSUMER->m_hist = hist_##DRAW_CONSUMER; \
PIPELINE->m_consumer.push_back(object_##DRAW_CONSUMER); }

#define PLOT_HIST1D_CONST1( PIPELINE, DRAW_CONSUMER, QUANTITY_NAME, CONST_PARAMS) \
{ Hist1D * hist_##DRAW_CONSUMER = new Hist1D; \
DRAW_CONSUMER * object_##DRAW_CONSUMER = new  DRAW_CONSUMER( CONST_PARAMS ); \
object_##DRAW_CONSUMER->m_sQuantityName = #QUANTITY_NAME; \
object_##DRAW_CONSUMER->m_hist = hist_##DRAW_CONSUMER; \
PIPELINE->m_consumer.push_back(object_##DRAW_CONSUMER); }

class ExcludedEvent
{
public:
	ExcludedEvent(double zpt, double jet1pt, double jet1phi, double jet2pt,
			double jet2phi)
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

	bool MatchesEvent(evtData * pRealEvent)
	{
		if (!FLOAT_COMPARE( pRealEvent->Z->Pt(), m_zpt ))
			return false;
		if (!FLOAT_COMPARE( pRealEvent->jets[0]->Pt(), m_jet1pt ))
			return false;
		if (!FLOAT_COMPARE( pRealEvent->jets[0]->Phi(), m_jet1phi ))
			return false;
		if (!FLOAT_COMPARE( pRealEvent->jets[1]->Pt(), m_jet2pt ))
			return false;
		if (!FLOAT_COMPARE( pRealEvent->jets[1]->Phi(), m_jet2phi ))
			return false;

		return true;
	}
};

vString g_lCorrFiles;
std::vector<ExcludedEvent *> g_mcExcludedEvents;

TFile * g_resFile;

TString g_sOutFolder("out/");
boost::scoped_ptr<Json_wrapper> g_json;

evtData g_ev;
TChain * g_pChain;

EventDataVector g_trackedEvents;

//EventSet g_eventsInCut;
EventVector g_eventsDataset;
PtBinWeighter g_mcWeighter;

typedef std::vector<PipelineSettings *> PipelineSettingsVector;
typedef boost::ptr_vector<EventPipeline> PipelineVector;

PipelineSettingsVector g_pipeSettings;
PipelineVector g_pipelines;

// set via config file
boost::ptr_vector<PtBin> g_newPtBins;

void calcJetEnergyCorrection(EventResult * res, CompleteJetCorrector * pJetCorr)
{
	for (int i = 0; i < 3; i++)
	{
		pJetCorr->CalcCorrectionForEvent(res);
	}
}

void RunPipelinesForEvent(EventResult & event)
{
	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)
	{
		g_cutHandler.ConfigureCuts(it->GetSettings());
		g_cutHandler.ApplyCuts(&event);
		it->RunEvent(event);
	}
}

// Generates the default pipeline which is run on all events.
// insert new Plots here if you want a new plot
EventPipeline * CreateDefaultPipeline()
{
	EventPipeline * pline = new EventPipeline();
	//	pline->m_consumer.push_back(new EventDump());

	/*	for (int i = 0; i < 400; i++ )
	 {
	 std::stringstream sname;
	 sname << i << "jetresp";
	 */

	PLOT_HIST1D(pline, DrawZMassConsumer, zmass)
	PLOT_HIST1D(pline, DrawZPtConsumer, z_pt)

	// Jet Pt
	PLOT_HIST1D_CONST1(pline, DrawJetPtConsumer, jet1_pt, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetPtConsumer, jet2_pt, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetPtConsumer, jet3_pt, 2)

	// Jet Phi
	PLOT_HIST1D_CONST1(pline, DrawJetPhiConsumer, jet1_phi, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetPhiConsumer, jet2_phi, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetPhiConsumer, jet3_phi, 2)

	// Jet Eta
	PLOT_HIST1D_CONST1(pline, DrawJetEtaConsumer, jet1_eta, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetEtaConsumer, jet2_eta, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetEtaConsumer, jet3_eta, 2)

	PLOT_HIST1D(pline, DrawZEtaConsumer, z_eta)
	PLOT_HIST1D(pline, DrawZPhiConsumer, z_phi)

	// Response
	PLOT_HIST1D(pline, DrawJetRespConsumer, jetresp)
	PLOT_HIST1D(pline, DrawMpfJetRespConsumer, mpfresp)

	PLOT_HIST1D(pline, DrawMuPlusPtConsumer, mu_plus_pt)
	PLOT_HIST1D(pline, DrawMuMinusPtConsumer, mu_minus_pt)
	PLOT_HIST1D(pline, DrawMuPlusEtaConsumer, mu_plus_eta)
	PLOT_HIST1D(pline, DrawMuMinusEtaConsumer, mu_minus_eta)
	PLOT_HIST1D(pline, DrawMuPlusPhiConsumer, mu_plus_phi)
	PLOT_HIST1D(pline, DrawMuMinusPhiConsumer, mu_minus_phi)

	PLOT_HIST1D(pline, DrawMuAllPtConsumer, mus_pt)
	PLOT_HIST1D(pline, DrawMuAllEtaConsumer, mus_eta)
	PLOT_HIST1D(pline, DrawMuAllPhiConsumer, mus_phi)

	PLOT_HIST1D(pline, DrawMetConsumer, met)
	PLOT_HIST1D(pline, DrawTcMetConsumer, tcmet)

	PLOT_HIST1D(pline, DrawRecoVertConsumer, recovert)

	/*	Hist2D * hist = new Hist2D;
	 DrawZMassConsumer * massc = new DrawZMassConsumer();
	 massc->m_sQuantityName = "zmass";
	 massc->m_hist = hist;

	 pline->m_consumer.push_back(massc);
	 */
	CutStatisticsConsumer *cs = new CutStatisticsConsumer();
	pline->m_consumer.push_back(cs);

	return pline;
}

bool IsEventHltAccepted(evtData & evt)
{
	TString hltName = "HLT_Mu9";

	/* 1 trigger approach */
	if (evt.cmsRun >= 147146)
		hltName = "HLT_Mu15_v1";

	/*
	 * 2 trigger approach old
	 if ( evt.cmsRun > 147196  )
	 hltName = "HLT_Mu11";
	 if ( evt.cmsRun > 148108 )
	 hltName = "HLT_Mu15_v1";
	 */
	const int nHLTriggers = evt.HLTriggers_accept->GetEntries();

	if (nHLTriggers == 0)
	{
		std::cout << "No HLT Trigger in Event! \n";
		exit(0);
	}

	TObjString *theHLTbit = NULL;
	//std::cout << "Checking HLT of Event " << std::endl;

	for (int i = 0; i < nHLTriggers; ++i)
	{

		theHLTbit = (TObjString*) evt.HLTriggers_accept->At(i);
		TString curName = theHLTbit->GetString();
		//std::cout << "HLT " << curName.Data() << " included" << std::endl;

		/*	    if ( res->GetCorrectedJetPt(1) < 5.0  )
		 res->m_pData->jets[1]->SetMomentum(0.0,0.0,0.0,0.0);
		 if ( res->GetCorrectedJetPt(2) < 5.0  )
		 res->m_pData->jets[2]->SetMomentum(0.0,0.0,0.0,0.0);
		 */
		if (hltName == curName)
		{
			//std::cout << "!! HLT trigger " << curName.Data() << " matched" << std::endl;
			return true;
		}
	}

	return false;
}

void importEvents(bool bUseJson,
		std::vector<ExcludedEvent *> exludeEventsByValue,
		bool bDiscardOutOfCutEvents, CompleteJetCorrector * correction) // can be null)
{
	int entries = g_pChain->GetEntries();

	TString sNewFile = "";
	bool bUseEvent;
	long lProcEvents = 0;
	g_lOverallNumberOfProcessedEvents = 0;

	// TODO dont do this if we have no weighting to analyze
	if (g_useEventWeight && g_useWeighting)
	{
		CALIB_LOG_FILE( "Analyzing Events for weighting " )
		for (Long_t ievt = 0; ievt < entries; ++ievt)
		{
			g_pChain->GetEntry(ievt);
			if (sNewFile != g_pChain->GetCurrentFile()->GetName())
			{
				//std::cout << "old file: " << sNewFile.Data() << std::endl;
				// new file opened, check the processed event
				// we assume all events in one root file have got the same xsection
				sNewFile = g_pChain->GetCurrentFile()->GetName();
				TH1F * pH = (TH1F *) g_pChain->GetCurrentFile()->Get(
						"number_of_processed_events");

				lProcEvents = TMath::Nint(pH->GetMean());
				CALIB_LOG_FILE( "new file: " << sNewFile.Data() << " number "
						<< lProcEvents )
				g_lOverallNumberOfProcessedEvents += lProcEvents;

				g_mcWeighter.IncreaseCountByXSection(g_ev.xsection,
						TMath::Nint(pH->GetMean()));
			}
		}
	}

	// cloning of a pipeline ?? goes here maybe
	// clone default pipeline for the number of settings we have
	g_pipelines.clear();

	for (PipelineSettingsVector::iterator it = g_pipeSettings.begin(); !(it
			== g_pipeSettings.end()); it++)
	{
		EventPipeline * pLine = CreateDefaultPipeline();

		// set the algo used for this run
		(*it)->SetAlgoName(g_sCurAlgo);

		pLine->InitPipeline(*it);
		g_pipelines.push_back(pLine);
	}

	CALIB_LOG_FILE( "Running " << g_pipelines.size() << " Pipeline(s) on events")
	CALIB_LOG_FILE( "Processing " << entries << " events ...")
	for (Long_t ievt = 0; ievt < entries; ++ievt)
	{

		bUseEvent = true;
		g_pChain->GetEntry(ievt);

		// check if this event matches our hlt trigger criteria
		/*        if ( bUseEvent && g_useHLT )
		 {
		 bUseEvent = IsEventHltAccepted( g_ev );
		 }
		 */
		if (bUseEvent)
		{
			EventResult * res = new EventResult;
			res->m_pData = &g_ev;

			if (g_useWeighting)
			{
				if (g_useEventWeight)
				{
					res->m_weight = res->m_pData->weight;
				}
				else
				{
					res->m_weight = g_mcWeighter.GetWeightByXSection(
							res->m_pData->xsection);
				}
			}

			calcJetEnergyCorrection(res, correction);
			RunPipelinesForEvent(*res);

			delete res;
			//g_eventsDataset.push_back( res );
		}

		if (((ievt % 5000) == 0) || (ievt == (entries - 1)))
			CALIB_LOG( (ievt + 1) << " of " << entries << " done [ " << floor( 100.0f * (float)(ievt +1)/(float)entries) << " % ]")

	}

	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)
	{
		it->FinishPipeline();
	}

	CALIB_LOG_FILE("All pipelines done")

	// sort by evt/run number
	g_eventsDataset.sort(CompareEventResult());
}

TChain * getChain(TString sName, evtData * pEv, std::string sRootfiles)
{
	TChain * mychain = new TChain(sName + "Jets_Zplusjet");

	pEv->Z = new TParticle();
	pEv->jets[0] = new TParticle();
	pEv->jets[1] = new TParticle();
	pEv->jets[2] = new TParticle();
	pEv->mu_minus = new TParticle();
	pEv->mu_plus = new TParticle();

	pEv->met = new TParticle();
	pEv->tcmet = new TParticle();

	pEv->HLTriggers_accept = new TClonesArray("TObjString");

	int addedfiles = 0;

	addedfiles = mychain->Add(sRootfiles.c_str());

	CALIB_LOG_FILE(addedfiles << " file(s) added to TChain" )

	// improves I/O
	// disable all branches
	// TParticles

	mychain->SetBranchAddress("Z", &pEv->Z);
	mychain->SetBranchAddress("jet", &pEv->jets[0]);
	mychain->SetBranchAddress("jet2", &pEv->jets[1]);
	mychain->SetBranchAddress("jet3", &pEv->jets[2]);
	mychain->SetBranchAddress("mu_plus", &pEv->mu_plus);
	mychain->SetBranchAddress("mu_minus", &pEv->mu_minus);

	mychain->SetBranchAddress("met", &pEv->met);
	mychain->SetBranchAddress("tcmet", &pEv->tcmet);

	// Triggers
	mychain->SetBranchAddress("HLTriggers_accept", &pEv->HLTriggers_accept);

	// Vertex Info
	mychain->SetBranchAddress("recoVertices", &pEv->recoVertices);
	mychain->SetBranchAddress("recoVerticesInfo", &pEv->recoVerticesInfo);
	mychain->SetBranchAddress("recoVerticesError", &pEv->recoVerticesError);

	// scalars
	mychain->SetBranchAddress("cmsEventNum", &pEv->cmsEventNum);
	mychain->SetBranchAddress("cmsRun", &pEv->cmsRun);
	mychain->SetBranchAddress("luminosityBlock", &pEv->luminosityBlock);
	mychain->SetBranchAddress("xsection", &pEv->xsection);
	mychain->SetBranchAddress("weight", &pEv->weight);

	mychain->SetBranchAddress("beamSpot", &pEv->beamSpot);

	return mychain;
}

inline void PrintEvent(EventResult & data, std::ostream & out,
		EventFormater * p = NULL, bool bAddNewline = true)
{
	EventFormater * pForm = p;

	if (pForm == NULL)
		pForm = new EventFormater();

	pForm->FormatEventResultCorrected(out, &data);

	if (bAddNewline)
		out << std::endl;

	if (p == NULL)
		// own formater created, delete it again
		delete pForm;
}

void ReapplyCut(bool bUseJson)
{
	for (EventVector::iterator iter = g_eventsDataset.begin(); !(iter
			== g_eventsDataset.end()); ++iter)
	{
		//        applyCut( &*iter, bUseJson );
		g_cutHandler.SetEnableCut(JsonCut::CudId, bUseJson);
		g_cutHandler.ApplyCuts(&*iter);
	}
}

void PrintCutReport(std::ostream & out)
{
	std::map<std::string, long> CountMap;

	for (EventVector::iterator iter = g_eventsDataset.begin(); !(iter
			== g_eventsDataset.end()); ++iter)
	{
		CountMap[iter->m_sCutUsed]++;
	}

	out << std::setprecision(3) << std::fixed;
	out << "--- Event Cut Report ---" << std::endl;
	out << std::setw(35) << "CutName" << std::setw(20) << "EvtsLeftRel [%]"
			<< std::setw(20) << "EvtsLeft" << std::setw(20)
			<< "EvtsDropRel [%]" << std::setw(20) << "EvtsDropAbs"
			<< std::setw(20) << std::endl;

	/// todo: actually use the processed events from root file

	long overallCountLeft = g_lOverallNumberOfProcessedEvents;
	//long refCount = g_eventsDataset.size();
	double droppedRel;

	out << std::setw(35) << "NumberOfProcessedEvents" << std::setw(20)
			<< overallCountLeft << std::endl;

	droppedRel = 1.0f - (double) (g_eventsDataset.size())
			/ (double) overallCountLeft;
	overallCountLeft = g_eventsDataset.size();
	out << std::setw(35) << "0) precuts" << std::setw(20)
			<< (1.0f - droppedRel) * 100.0f << std::setw(20)
			<< overallCountLeft << std::setw(20) << droppedRel * 100.0f
			<< std::setw(20) << (g_lOverallNumberOfProcessedEvents
			- g_eventsDataset.size()) << std::endl;

	for (std::map<std::string, long>::iterator iter = CountMap.begin(); !(iter
			== CountMap.end()); ++iter)
	{
		droppedRel = 1.0f - (double) (overallCountLeft - iter->second)
				/ (double) overallCountLeft;
		overallCountLeft -= iter->second;
		if (iter->first == "8) within cut")
			out << std::setw(35) << iter->first << std::setw(20)
					<< iter->second << std::endl;
		else
			out << std::setw(35) << iter->first << std::setw(20) << (1.0f
					- droppedRel) * 100.0f << std::setw(20) << overallCountLeft
					<< std::setw(20) << droppedRel * 100.0f << std::setw(20)
					<< iter->second << std::endl;
	}

}

void PrintTrackedEventsReport(bool bShort = false)
{
	EventFormater eFormat;

	eFormat.Header(std::cout);
	std::cout << std::endl;

	for (EventVector::iterator iter = g_eventsDataset.begin(); !(iter
			== g_eventsDataset.end()); iter++)
	{
		bool bFound = false;

		for (EventDataVector::iterator iterTracked = g_trackedEvents.begin(); !(iterTracked
				== g_trackedEvents.end()); iterTracked++)
		{
			{
				if ((iterTracked->cmsRun == iter->m_pData->cmsRun)
						&& (iterTracked->cmsEventNum
								== iter->m_pData->cmsEventNum))
				{
					if (iter->IsInCut())
						bFound = true;
				}
			}
		}

		if (!bFound)
		{
			std::cout << std::endl << "Event " << iter->m_pData->cmsRun << ":"
					<< iter->m_pData->cmsEventNum
					<< " not in tracked Events List";
		}
	}
}

void PrintEventsReport(std::ostream & out, bool bOnlyInCut)
{
	EventVector::iterator iterInCut;

	EventFormater eFormat;
	eFormat.Header(out);
	out << std::endl;

	out << "Events in Cut" << std::endl << std::endl;

	int i = 0;

	for (iterInCut = g_eventsDataset.begin(); !(iterInCut
			== g_eventsDataset.end()); iterInCut++)
	{
		if (iterInCut->IsInCut() || (!bOnlyInCut))
		{
			PrintEvent(*iterInCut, out, NULL, true);
			++i;
		}
	}

	cout << ">> " << i << " Events in Cut" << std::endl;
}

void WriteSelectedEvents(TString algoName, TString prefix,
		EventVector & events, TFile * pFileOut)
{/*
 if (g_writeEventsSetting == NoEvents)
 return;

 TTree* gentree = new TTree(algoName + prefix + "_events", algoName + prefix
 + "_events");

 evtData localData;
 Double_t l2corr = 1.0f;
 Double_t l2corrPtJet2 = 1.0f;
 Double_t l2corrPtJet3 = 1.0f;

 localData.jets[0] = new TParticle();
 localData.Z = new TParticle();

 // more data can go here
 gentree->Branch("Z", "TParticle", &localData.Z);
 gentree->Branch("jet1", "TParticle", &localData.jets[0]);
 gentree->Branch("jet2", "TParticle", &localData.jets[1]);
 gentree->Branch("jet3", "TParticle", &localData.jets[2]);
 gentree->Branch("l2corrJet", &l2corr, "l2corrJet/D");
 gentree->Branch("l2corrPtJet2", &l2corrPtJet2, "l2corrPtJet2/D");
 gentree->Branch("l2corrPtJet3", &l2corrPtJet3, "l2corrPtJet3/D");

 gentree->Branch("cmsEventNum", &localData.cmsEventNum, "cmsEventNum/L");
 gentree->Branch("cmsRun", &localData.cmsRun, "cmsRun/L");
 gentree->Branch("luminosityBlock", &localData.luminosityBlock, "cmsRun/L");
 //  gentree->Branch("xsection",&localData.xsection,"xsection/D");

 EventVector::iterator it;
 for (it = events.begin(); !(it == events.end()); ++it)
 {
 if (it->IsInCut() || (g_writeEventsSetting == AllEvents))
 {
 localData.Z = new TParticle(*it->m_pData->Z);
 localData.jets[0] = new TParticle(*it->m_pData->jets[0]);
 localData.jets[1] = new TParticle(*it->m_pData->jets[1]);
 localData.jets[2] = new TParticle(*it->m_pData->jets[2]);
 l2corr = it->m_l2CorrPtJets[0];
 l2corrPtJet2 = it->m_l2CorrPtJets[1];
 l2corrPtJet3 = it->m_l2CorrPtJets[2];

 localData.cmsEventNum = it->m_pData->cmsEventNum;
 localData.cmsRun = it->m_pData->cmsRun;
 localData.luminosityBlock = it->m_pData->luminosityBlock;

 gentree->Fill();
 }
 }

 pFileOut->cd();
 gentree->Write();*/
}

void DrawJetResponsePlots(TString algoName, TFile * pFileOut)
{
	// todo here
}

void DrawHistoSet(TString algoName, TString sPostfix, TFile * pFileOut,
		bool useCutParameter, bool bPtCut, double ptLow = 0.0, double ptHigh =
				0.0)
{
	std::cout << "Drawing Plots " << algoName << sPostfix << " Cuts: "
			<< useCutParameter << " PtCut-low: " << ptLow << " Pt-Cut-high:"
			<< ptHigh << std::endl;
	/*
	 // ZMass with cut/
	 ModifierList modList;
	 modList.push_back( new CModHorizontalLine( g_kZmass - g_kCutZmassWindow ));
	 modList.push_back( new CModHorizontalLine( g_kZmass + g_kCutZmassWindow));
	 modList.push_back( new CModTdrStyle());
	 */
	/*    CHistDrawBase zmass( "zmass_" + algoName + sPostfix,
	 pFileOut,
	 modList);

	 CHistEvtDataZMass zdraw;
	 ModEvtDraw( &zdraw, useCutParameter, bPtCut, ptLow, ptHigh );
	 zmass.Execute < EventVector > ( g_eventsDataset, &zdraw );
	 */
	/*
	 // ZPt
	 CHistDrawBase zPt( "zPt_" + algoName + sPostfix,
	 pFileOut);
	 CHistEvtDataZPt zPtdraw;
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
	 */
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
	/*    CHistDrawBase jetPtzPt( "z_pt_minus_jet1_pt_" + algoName+ sPostfix,
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
	 recovert.AddModifier(new CModBinCount(15));

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
	 }*/
	/*
	 CGrapErrorDrawBase < EventVector &, CGraphDrawEvtMap< CPlotL2Corr > >  l2corr_draw( "l2corr_" + algoName+ sPostfix, pFileOut);
	 l2corr_draw.Execute( g_eventsDataset );
	 */
	// Jet Response binned as jet1.pt() here ??
	/*    CGrapErrorDrawBase < EventVector &, CGraphDrawEvtMap< CPlotL2Corr > >  l2corr_draw( "l2corr_" + algoName+ sPostfix, pFileOut);

	 l2corr_draw.Execute( g_eventsDataset );
	 */
}

void loadTrackedEventsFromFile(std::string fileName)
{
	ReadCsv csv(fileName);
	std::vector<std::string> sRunNum = csv.ReadColumn(2);
	std::vector<std::string> sEvtNum = csv.ReadColumn(4);

	std::vector<std::string>::iterator iterRun;
	//    std::vector< std::string>::iterator iterLumi;
	std::vector<std::string>::iterator iterEvtNum;

	std::cout << g_trackedEvents.size() << std::endl;

	iterEvtNum = sEvtNum.begin();
	//    iterLumi = sLumi.begin();

	for (iterRun = sRunNum.begin(); iterRun != sRunNum.end(); iterRun++)
	{
		evtData * evt = new evtData;

		evt->cmsRun = atoi((*iterRun).c_str());
		evt->cmsEventNum = atoi((*iterEvtNum).c_str());

		g_trackedEvents.push_back(evt);
		std::cout << "Added to tracked Events " << std::endl;
		iterEvtNum++;
		//iterLumi++;
	}
	std::cout << g_trackedEvents.size() << std::endl;
}

void drawHistoBins(std::string sName, std::string tags, TFile * pFileOut,
		bool bUseCut)
{
	// cut / nocut , ptBins
	BOOST_FOREACH( PtBin & bin, g_newPtBins )
{	std::stringstream newTags (stringstream::in| stringstream::out);
	newTags << tags << std::setprecision(0) << std::fixed << "_Pt" << bin.m_fLowestPt << "to" << bin.m_fHighestPt;
	//tags

	DrawHistoSet( sName, newTags.str(), pFileOut , bUseCut, true,
			bin.m_fLowestPt, bin.m_fHighestPt );
}

// cut / nocut without bins...
std::stringstream newTags (stringstream::in| stringstream::out);
DrawHistoSet( sName,tags, pFileOut, bUseCut, false );

// Draw Histos which use other histos as input
//Draw2ndLevelHistoSet( sName,tags, pFileOut, bUseCut, false );

}

void ResetExcludedEvents()
{
	// check if this is an excluded event
	BOOST_FOREACH( ExcludedEvent * pEx, g_mcExcludedEvents)
{	pEx->m_eventFound = false;
}
}

void processAlgo()
{
	CompleteJetCorrector jetCorr;
	/*	(*g_logFile) << "Processing " << sName << std::endl;
	 (*g_logFile) << "uncorrected jets " << std::endl;

	 CompleteJetCorrector jetCorr;

	 if (g_doL2Correction)
	 jetCorr.AddCorrection(new L2Corr(TString(sName.c_str()), g_l2CorrFiles));
	 if (g_doL3Correction)
	 jetCorr.AddCorrection(new L3Corr(TString(sName.c_str()), g_l3CorrFiles));
	 if (g_doL3CorrectionFormula)
	 jetCorr.AddCorrection(new TF1Corr(g_l3Formula, g_l3FormulaParams));

	 ResetExcludedEvents();
	 g_eventsDataset.clear();
	 */
	// reset weighting class here
	g_mcWeighter.ResetEntryCount();

	// oha. we got to s
	stringvector algoList = PropertyTreeSupport::GetAsStringList(&g_propTree,
			"Algos");

	BOOST_FOREACH( std::string sAlgo, algoList)
{	g_pChain = getChain(sAlgo, &g_ev, g_sSource);
	g_sCurAlgo = sAlgo;
	std::string sPrefix;
	/*
	 if (g_doMc)
	 {
	 sPrefix = "_mc";
	 importEvents(false, true, g_mcExcludedEvents, false, &jetCorr);

	 g_mcWeighter.Print();
	 }

	 if (g_doData)
	 {*/
	sPrefix = "_data";
	importEvents(true, std::vector<ExcludedEvent *>(), false,
			&jetCorr);
	//}

	delete g_pChain;
}
/*
 //	std::cout << "MC Weighting Report" << std::endl;
 //	DrawMcEventCount( sName, g_resFileMc);

 // RAW
 drawHistoBins(sName, sPrefix , g_resFile.get(),  true);
 if ( g_plotNoCuts)
 drawHistoBins(sName, sPrefix + "_nocut", g_resFile.get(), false);

 PrintCutReport( std::cout );
 PrintCutReport( *g_logFile );

 WriteSelectedEvents(sName, sPrefix, g_eventsDataset, g_resFile.get() );

 if ( g_sTrackedEventsFile.length() > 0 )
 PrintTrackedEventsReport();

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

 if ( g_sTrackedEventsFile.length() > 0 )
 PrintTrackedEventsReport();


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

 if ( g_sTrackedEventsFile.length() > 0 )
 PrintTrackedEventsReport();

 if (g_doData)
 {
 PrintEventsReport(std::cout, true);
 PrintEventsReport(*g_logFile, true);
 }
 }
 //    drawJetResponsePlots( sName, g_resFile.get() );
 WriteCuts( sName, g_resFile.get() );
 */

}

void CreateWeightBins()
{
	/*
	 * Fall10 MC  */
	g_mcWeighter.AddBin(PtBin(0.0, 15.0), 4.281e+03);
	g_mcWeighter.AddBin(PtBin(15.0, 20.0), 1.451e+02);
	g_mcWeighter.AddBin(PtBin(20.0, 30.0), 1.305e+02);
	g_mcWeighter.AddBin(PtBin(30.0, 50.0), 8.398e+01);
	g_mcWeighter.AddBin(PtBin(50.0, 80.0), 3.224e+01);
	g_mcWeighter.AddBin(PtBin(80.0, 120.0), 9.984e+00);
	g_mcWeighter.AddBin(PtBin(120.0, 170.0), 2.734e+00);
	g_mcWeighter.AddBin(PtBin(170.0, 230.0), 7.207e-01);
	g_mcWeighter.AddBin(PtBin(230.0, 300.0), 1.939e-01);
	g_mcWeighter.AddBin(PtBin(300.0, 999999.0), 7.586e-02);
}

PipelineSettings * CreateDefaultSettings(std::string sAlgoName,
		InputTypeEnum inpType)
{
	PipelineSettings * psetting = new PipelineSettings();
	return psetting;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0]
				<< " json_config_file.json [VerboseLevel]\n";
		return 1;
	}

	std::string jsonConfig = argv[1];
	boost::property_tree::json_parser::read_json(jsonConfig, g_propTree);

	g_sOutputPath = g_propTree.get<std::string> ("OutputPath");
	std::string sLogFileName = g_sOutputPath + ".log";
	g_logFile.reset(new ofstream(sLogFileName.c_str(), ios_base::trunc));

	CreateWeightBins();
	/*
	 if ((bool) p.getInt(secname + ".fixed_weighting"))
	 {
	 g_mcWeighter.Reset();
	 g_mcWeighter.AddBin(PtBin(0.0, 999999.0), 1300);
	 }

	 if ((bool) p.getInt(secname + ".use_event_weight"))
	 {
	 g_useEventWeight = true;
	 }

	 g_writeEventsSetting = NoEvents;

	 if (p.getString(secname + ".write_events") == "incut")
	 g_writeEventsSetting = OnlyInCutEvents;
	 if (p.getString(secname + ".write_events") == "all")
	 g_writeEventsSetting = AllEvents;

	 g_doL2Correction = (bool) p.getInt(secname + ".do_l2_correction");
	 g_doL3Correction = (bool) p.getInt(secname + ".do_l3_correction");
	 g_doL3CorrectionFormula = (bool) p.getInt(secname
	 + ".do_l3_correction_formula");

	 g_plotNoCuts = (bool) p.getInt(secname + ".plot_nocuts");
	 g_plotCutEff = (bool) p.getInt(secname + ".plot_cuteff");

	 g_useHLT = (bool) p.getInt(secname + ".use_hlt");

	 g_l3Formula = p.getString(secname + ".l3_formula");
	 g_l3FormulaParams = p.getvDouble(secname + ".l3_formula_params");

	 g_doData = (bool) p.getInt(secname + ".is_data");
	 g_doMc = !g_doData;

	 g_sSource = p.getString(secname + ".tchain_file_path");
	 */

	// input files
	g_sSource = g_propTree.get<std::string> ("InputFiles");
	CALIB_LOG_FILE("Using InputFiles " << g_sSource)

	// removes the old file
	std::string sRootOutputFilename = (g_sOutputPath + ".root");
	g_resFile = new TFile(sRootOutputFilename.c_str(), "RECREATE");
	CALIB_LOG_FILE("Writing to root file " << sRootOutputFilename)

	// insert config into log file
	CALIB_LOG_FILE( "Configuration file " << jsonConfig << " dump:" );
	boost::property_tree::json_parser::write_json(*g_logFile, g_propTree);

	/*
	 g_sTrackedEventsFile = p.getString(secname + ".tracked_events");
	 if (g_sTrackedEventsFile.length() > 0)
	 loadTrackedEventsFromFile(g_sTrackedEventsFile);
	 */
	g_json.reset(new Json_wrapper(g_propTree.get("JsonFile", "").c_str()));

	// weighting settings
	g_useWeighting = g_propTree.get<bool> ("UseEventWeight");
	g_useEventWeight = g_propTree.get<bool> ("UseWeighting");

	/*
	 g_l2CorrFiles = p.getvString(secname + ".l2_correction_data");
	 g_l3CorrFiles = p.getvString(secname + ".l3_correction_data");
	 */

	// init cuts
	// values are set for each Pipeline individually
	g_cutHandler.AddCut(new JsonCut(&(*g_json)));
	g_cutHandler.AddCut(new MuonPtCut(0.0));
	g_cutHandler.AddCut(new MuonEtaCut());
	g_cutHandler.AddCut(new LeadingJetEtaCut());
	g_cutHandler.AddCut(new SecondLeadingToZPtCut(0.0));
	g_cutHandler.AddCut(new BackToBackCut(0.0));
	g_cutHandler.AddCut(new ZMassWindowCut(0.0));
	g_cutHandler.AddCut(new ZPtCut(0.0));

	PipelineSettings * pset = NULL;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
			g_propTree.get_child("Pipelines") )
{	pset = new PipelineSettings();
	pset->SetPropTree( &g_propTree );

	std::string sKeyName = v.first.data();
	pset->SetSettingsRoot("Pipelines." + sKeyName);
	pset->SetRootOutFile(g_resFile);

	g_pipeSettings.push_back( pset );
}
/*
 PipelineSettingsVector vSettings;

 vSettings.push_back(pset);
 pset = new PipelineSettings( *pset );
 pset->AddFilter("incut");
 vSettings.push_back(pset);

 vSettings = ExpandPtBins( vSettings, g_newPtBins );

 g_pipeSettings = vSettings;
 */
processAlgo();

g_resFile->Close();
g_logFile->close();

return 0;
}

