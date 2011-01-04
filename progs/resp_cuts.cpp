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
#include "EventStorer.h"

using namespace CalibFW;

/* BASIC CONFIGURATION */

// DP made the variables not const to set them by command line args


std::string g_sSource("");

vString g_l2CorrFiles;
vString g_l3CorrFiles;

// TODO: delete this pointer when done
PipelineSettings g_defaultSettings;

TString g_l3Formula;
vdouble g_l3FormulaParams;

vdouble g_customBinning;

std::string g_sCurAlgo;
int g_iAlgoOverallCount;
int g_iCurAlgoCount;

bool g_useWeighting;
bool g_useEventWeight;


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

#define PLOT_GRAPHERRORS( PIPELINE, DRAW_CONSUMER, QUANTITY_NAME) \
{ GraphErrors * hist_##DRAW_CONSUMER = new GraphErrors; \
DRAW_CONSUMER * object_##DRAW_CONSUMER = new  DRAW_CONSUMER(); \
object_##DRAW_CONSUMER->m_sQuantityName = #QUANTITY_NAME; \
object_##DRAW_CONSUMER->m_graph = hist_##DRAW_CONSUMER; \
PIPELINE->m_consumer.push_back(object_##DRAW_CONSUMER); }

#define PLOT_GRAPHERRORS_COND1( PIPELINE, DRAW_CONSUMER, QUANTITY_NAME, CONST_PARAMS) \
{ GraphErrors * hist_##DRAW_CONSUMER = new GraphErrors; \
DRAW_CONSUMER * object_##DRAW_CONSUMER = new  DRAW_CONSUMER( CONST_PARAMS); \
object_##DRAW_CONSUMER->m_sQuantityName = #QUANTITY_NAME; \
object_##DRAW_CONSUMER->m_graph = hist_##DRAW_CONSUMER; \
PIPELINE->m_consumer.push_back(object_##DRAW_CONSUMER); }

#define PLOT_GRAPHERRORS_CONST( PIPELINE, DRAW_CONSUMER, QUANTITY_NAME, CONST) \
{ GraphErrors * hist_##DRAW_CONSUMER = new GraphErrors; \
DRAW_CONSUMER * object_##DRAW_CONSUMER = new  CONST; \
object_##DRAW_CONSUMER->m_sQuantityName = QUANTITY_NAME; \
object_##DRAW_CONSUMER->m_graph = hist_##DRAW_CONSUMER; \
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
		if (it->GetSettings()->GetLevel() == 1)
		{
			g_cutHandler.ConfigureCuts(it->GetSettings());
			g_cutHandler.ApplyCuts(&event);

			// don't run event if it was not accepted by JSON file.
			// this events can contain "unphysical" results due to measurement errors

			// IsValidEvent checks if in JSON and in HLT selection
			if ( g_cutHandler.IsValidEvent( &event ) )
				it->RunEvent(event);
		}
	}
}

void RunPipelines(int level)
{
	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)
	{
		if (it->GetSettings()->GetLevel() == level)
		{
			it->Run();
		}
	}
}

EventPipeline * CreateLevel2Pipeline()
{
	EventPipeline * pline = new EventPipeline();

	PLOT_GRAPHERRORS_COND1( pline, DrawJetRespGraph, jetresp, "jetresp" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetRespGraph, mpfresp, "mpfresp" )

	return pline;
}

void AddConsumerToPipeline( EventPipeline * pline, std::string consumerName)
{
	if ( consumerName == EventStorerConsumer().GetId())
	{
		pline->m_consumer.push_back( new EventStorerConsumer() );
	}

	if ( consumerName == CutStatisticsConsumer().GetId())
	{
		pline->m_consumer.push_back( new CutStatisticsConsumer() );
	}
}

void AddConsumersToPipeline( EventPipeline * pline, std::vector<std::string> consList )
{
	BOOST_FOREACH( std::string s, consList )
	{
		CALIB_LOG( "Adding consumer " << s)
		AddConsumerToPipeline( pline, s);
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

	// Jet Delta Eta wrt to Z
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaEtaConsumer, jet1_deltaeta_z, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaEtaConsumer, jet2_deltaeta_z, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaEtaConsumer, jet3_deltaeta_z, 2)

	// Jet Delta Phi wrt to Z
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaPhiConsumer, jet1_deltaphi_z, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaPhiConsumer, jet2_deltaphi_z, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaPhiConsumer, jet3_deltaphi_z, 2)

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

	for ( CutHandler::CutVector::iterator it = g_cutHandler.GetCuts().begin();
			!( it == g_cutHandler.GetCuts().end()); it++)
	{
		GraphErrors * hist_CutEff = new GraphErrors;
		DrawCutEffGraph<GraphXProviderZpt>  * object_consumer = new DrawCutEffGraph<GraphXProviderZpt>( it->GetId() );
		object_consumer->m_sQuantityName = "cuteff_" + it->GetCutShortName() + "_zpt";
		object_consumer->m_graph = hist_CutEff;
		pline->m_consumer.push_back(object_consumer);

		hist_CutEff = new GraphErrors;
		DrawCutEffGraph<GraphXProviderRecoVert>  * object_consumerReco = new DrawCutEffGraph<GraphXProviderRecoVert>( it->GetId() );
		object_consumerReco->m_sQuantityName = "cuteff_" + it->GetCutShortName() + "_nrv";
		object_consumerReco->m_graph = hist_CutEff;
		pline->m_consumer.push_back(object_consumerReco);
	}

	// event count
	Hist1D * hist_evCount = new Hist1D;
	DrawEventCount * object_eVconsumer = new DrawEventCount;
	object_eVconsumer->m_sQuantityName = "eventcount";
	object_eVconsumer->m_hist = hist_evCount;
	pline->m_consumer.push_back(object_eVconsumer);

	// Jet 1 Eta Phi map
	Hist2D * hist_ePhiMapJet = new Hist2D;
	DrawEtaPhiJetMapConsumer * object_ePhiMapJetconsumer = new DrawEtaPhiJetMapConsumer(0);
	object_ePhiMapJetconsumer->m_sQuantityName = "etaphi_jet1_to_z";
	object_ePhiMapJetconsumer->m_hist = hist_ePhiMapJet;
	pline->m_consumer.push_back(object_ePhiMapJetconsumer);

	// Jet 2 Eta Phi map
	Hist2D * hist_ePhiMapJet2 = new Hist2D;
	DrawEtaPhiJetMapConsumer * object_ePhiMapJetconsumer2 = new DrawEtaPhiJetMapConsumer(1);
	object_ePhiMapJetconsumer2->m_sQuantityName = "etaphi_jet2_to_z";
	object_ePhiMapJetconsumer2->m_hist = hist_ePhiMapJet2;
	pline->m_consumer.push_back(object_ePhiMapJetconsumer2);


	PLOT_GRAPHERRORS( pline, DrawDeltaPhiRange, deltaphi_test )


	GraphErrors * hist_DrawJetPt = new GraphErrors;
	DrawJetPt<GraphXProviderJetPhiDeltaZ<0> >  * object_DrawJetPt = new DrawJetPt<GraphXProviderJetPhiDeltaZ<0> >( 0 );
	object_DrawJetPt->m_sQuantityName = "jet1pt_deltaphi";
	object_DrawJetPt->m_graph = hist_DrawJetPt;
	pline->m_consumer.push_back(object_DrawJetPt);

	hist_DrawJetPt = new GraphErrors;
	DrawJetPt<GraphXProviderJetPhiDeltaZ<1> >  * object_DrawJet2Pt = new DrawJetPt<GraphXProviderJetPhiDeltaZ<1> >( 1 );
	object_DrawJet2Pt->m_sQuantityName = "jet2pt_deltaphi";
	object_DrawJet2Pt->m_graph = hist_DrawJetPt;
	pline->m_consumer.push_back(object_DrawJet2Pt);


	//PLOT_GRAPHERRORS( pline, DrawJetRespBase, jetresp )
	/*	Hist2D * hist = new Hist2D;
	 DrawZMassConsumer * massc = new DrawZMassConsumer();
	 massc->m_sQuantityName = "zmass";
	 massc->m_hist = hist;

	 pline->m_consumer.push_back(massc);
	 */

	return pline;
}

void importEvents(bool bUseJson,
		std::vector<ExcludedEvent *> exludeEventsByValue,
		bool bDiscardOutOfCutEvents, CompleteJetCorrector * correction) // can be null)
{
	int entries = g_pChain->GetEntries();

	TString sNewFile = "";
	long lProcEvents = 0;
	g_lOverallNumberOfProcessedEvents = 0;

	// TODO dont do this if we have no weighting to analyze or the weighting is in the events themself
	if ( !g_useEventWeight && g_useWeighting)
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
		if ((*it)->GetLevel() == 1)
		{
			EventPipeline * pLine = CreateDefaultPipeline();

			AddConsumersToPipeline( pLine,(*it)->GetAdditionalConsumer());

			// set the algo used for this run
			(*it)->SetAlgoName(g_sCurAlgo);

			pLine->InitPipeline(*it);
			g_pipelines.push_back(pLine);
		}
	}

	CALIB_LOG_FILE( "Running " << g_pipelines.size() << " Pipeline(s) on events")
	CALIB_LOG_FILE( "Processing " << entries << " events ...")
	CALIB_LOG_FILE( "algo " << g_sCurAlgo)
	for (Long_t ievt = 0; ievt < entries; ++ievt)
	{
		g_pChain->GetEntry(ievt);

		EventResult * res = new EventResult;
		res->m_pData = &g_ev;
		
		// the weight of data events can be strange when read from root file. better reset here
		res->m_weight = 1.0f;
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



		if (((ievt % 5000) == 0) || (ievt == (entries - 1)))
		{
			float localPercent = floor( 100.0f * (float)(ievt +1)/(float)entries);
			float overallPercent =  floor( ( (float)g_iCurAlgoCount / (float)g_iAlgoOverallCount
					+ (localPercent * 0.01f) * (1.0f) / (float)g_iAlgoOverallCount ) *100.0f);

			CALIB_LOG( (ievt + 1) << " of " << entries << " done [ this algo " << std::fixed <<  std::setprecision(0)
			<< localPercent << " % ] [ overall " << overallPercent << " % ]" )

		}
	}

	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)
	{
		it->FinishPipeline();
	}

	CALIB_LOG_FILE("All level 1 pipelines done")

	CALIB_LOG_FILE("Running level 2 pipelines")

	// cloning of a pipeline ?? goes here maybe
	// clone default pipeline for the number of settings we have
	g_pipelines.clear();

	for (PipelineSettingsVector::iterator it = g_pipeSettings.begin(); !(it
			== g_pipeSettings.end()); it++)
	{
		if ((*it)->GetLevel() == 2)
		{
			EventPipeline * pLine = CreateLevel2Pipeline();

			AddConsumersToPipeline( pLine,(*it)->GetAdditionalConsumer());

			// set the algo used for this run
			(*it)->SetAlgoName(g_sCurAlgo);

			pLine->InitPipeline(*it);
			g_pipelines.push_back(pLine);
		}
	}

	RunPipelines(2);
	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)
	{
		it->FinishPipeline();
	}
	CALIB_LOG_FILE("All level 2 pipelines done")
}

TChain * getChain(TString sName, evtData * pEv, std::string sRootfiles)
{
	TChain * mychain = new TChain(sName + "_Zplusjet");

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

	g_iAlgoOverallCount = algoList.size();
	g_iCurAlgoCount = 0;

	BOOST_FOREACH( std::string sAlgo, algoList)
{	g_pChain = getChain(sAlgo, &g_ev, g_sSource);
	g_sCurAlgo = sAlgo;
	/*
	 if (g_doMc)
	 {
	 sPrefix = "_mc";
	 importEvents(false, true, g_mcExcludedEvents, false, &jetCorr);

	 g_mcWeighter.Print();
	 }

	 if (g_doData)
	 {*/
	importEvents(true, std::vector<ExcludedEvent *>(), false,
			&jetCorr);
	//}

	g_iCurAlgoCount++;

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

	//Todo: close file to free memory of already written histos
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
	g_cutHandler.AddCut(new HltCut());
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

