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

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// this include will overload comparison headers automatically
#include <utility>
using namespace std::rel_ops;

#include "MinimalParser.h"

#include "OpenMP-Support.h"

#include "CompleteJetCorrector.h"

/*
 #include <iostream>
 #include <hash_map>
 */
#include "Json_wrapper.h"
#include "read_csv.h"
#include "EventData.h"

#include "CutStatistics.h"

#include "PtBinWeighter.h"

#include "ZJetPipeline.h"
#include "ZJetEventStorer.h"
#include "ZJetDrawConsumer.h"
#include "ZJetCuts.h"

using namespace CalibFW;

/* BASIC CONFIGURATION */

// DP made the variables not const to set them by command line args


std::string g_sSource("");

vdouble g_customBinning;

std::string g_sCurAlgo;
int g_iAlgoOverallCount;
int g_iCurAlgoCount;

// weighting related settings
bool g_useWeighting;
bool g_useEventWeight;

bool g_useGlobalWeightBin;
double g_globalXSection;
bool g_eventReweighting;

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

#define PLOT_HIST2D_CONST1( PIPELINE, DRAW_CONSUMER, QUANTITY_NAME, CONST_PARAMS) \
{ Hist2D * hist_##DRAW_CONSUMER = new Hist2D; \
DRAW_CONSUMER * object_##DRAW_CONSUMER = new  DRAW_CONSUMER( CONST_PARAMS ); \
object_##DRAW_CONSUMER->m_sQuantityName = #QUANTITY_NAME; \
object_##DRAW_CONSUMER->m_hist = hist_##DRAW_CONSUMER; \
PIPELINE->m_consumer.push_back(object_##DRAW_CONSUMER); }

#define PLOT_HIST2D( PIPELINE, DRAW_CONSUMER, QUANTITY_NAME) \
{ Hist2D * hist_##DRAW_CONSUMER = new Hist2D; \
DRAW_CONSUMER * object_##DRAW_CONSUMER = new  DRAW_CONSUMER(); \
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

#define PLOT_GRAPHERRORS_COND2( PIPELINE, DRAW_CONSUMER, QUANTITY_NAME, CONST_PARAM1, CONST_PARAM2) \
{ GraphErrors * hist_##DRAW_CONSUMER = new GraphErrors; \
DRAW_CONSUMER * object_##DRAW_CONSUMER = new  DRAW_CONSUMER( CONST_PARAM1, CONST_PARAM2); \
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
std::auto_ptr<Json_wrapper> g_json;

evtData g_ev;
TChain * g_pChain;

EventDataVector g_trackedEvents;

std::auto_ptr<ZJetCutHandler> g_cutHandler;

PtBinWeighter g_mcWeighter;

typedef std::vector<ZJetPipelineSettings *> PipelineSettingsVector;
typedef boost::ptr_vector<ZJetPipeline> PipelineVector;

PipelineSettingsVector g_pipeSettings;
PipelineVector g_pipelines;

// set via config file
boost::ptr_vector<PtBin> g_newPtBins;

void calcJetEnergyCorrection(EventResult * res, CompleteJetCorrector * pJetCorr)
{/*
 for (int i = 0; i < 3; i++)
 {
 pJetCorr->CalcCorrectionForEvent(res);
 }*/

	/*
	 not supported any more, use the CompleteJetCorrector
	 if ( g_corrWithFormula )
	 {

	 res->m_l3CorrPtJets[0] = g_corrFormula->Eval( res->GetCorrectedJetPt( 0 ));
	 res->m_l3CorrPtJets[1] = g_corrFormula->Eval( res->GetCorrectedJetPt( 1 ));
	 res->m_l3CorrPtJets[2] = g_corrFormula->Eval( res->GetCorrectedJetPt( 2 ));
	 res->m_bUseL3 = true;
	 }
	 */

}

double GetEventXSection(EventResult & evt)
{
	double xsec = 1.0f;
	if (g_globalXSection > 0.0f)
		xsec = g_globalXSection;
	else
		xsec = evt.m_pData->xsection;

	return xsec;
}

void RunPipelinesForEvent(EventResult & event)
{
	// experimental: this section has been paralelized

	unsigned int i = 0;
#pragma omp parallel for

/*	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)*/
	for (i = 0; i < g_pipelines.size(); i++)
	{
		ZJetPipeline * pline = &g_pipelines[i];

		if (pline->GetSettings()->GetLevel() == 1)
		{
			//		    HIER GEHTS WEITER
			//event.PipelineSettings = it->GetSettings();

			g_cutHandler->ApplyCuts(&event, pline->GetSettings());

			// don't run event if it was not accepted by JSON file.
			// this events can contain "unphysical" results due to measurement errors
			// IsValidEvent checks if in JSON and in HLT selection
			if (event.IsValidEvent())
				pline->RunEvent(event);
		}
	}
}

void RunPipelines(int level)
{
	if (level == 1)
		CALIB_LOG_FATAL( "This method can not be used for leve1 pipelines, use RunPipelinesForEvent instead" )

	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)
	{
		if (it->GetSettings()->GetLevel() == level)
		{

			it->Run();
		}
	}
}

ZJetPipeline * CreateLevel2Pipeline()
{
	ZJetPipeline * pline = new ZJetPipeline();

	PLOT_GRAPHERRORS_COND1( pline, DrawJetRespGraph, jetresp, "jetresp" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetRespGraph, mpfresp, "mpfresp" )

	PLOT_GRAPHERRORS_COND1( pline, DrawJetCorrGraph, balance_jetcorr, "jetresp" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetCorrGraph, mpf_jetcorr, "mpfresp" )

	PLOT_GRAPHERRORS_COND1( pline, DrawJetChargedHadronEnergy, jet1_chargedhadronenergy_fraction, "jet1_chargedhadronenergy_fraction" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetNeutralHadronEnergy, jet1_neutralhadronenergy_fraction, "jet1_neutralhadronenergy_fraction" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetChargedEmEnergy, jet1_chargedemenergy_fraction, "jet1_chargedemenergy_fraction" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetNeutralEmEnergy, jet1_neutralemenergy_fraction, "jet1_neutralemenergy_fraction" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetElectronEnergy, jet1_electronenergy_fraction, "jet1_electronenergy_fraction" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetMuonEnergy, jet1_muonenergy_fraction, "jet1_muonenergy_fraction" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetPhotonEnergy, jet1_photonenergy_fraction, "jet1_photonenergy_fraction" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetChargedHadronMultiplicity, jet1_chargedhadronmultiplicity, "jet1_chargedhadronmultiplicity" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetNeutralHadronMultiplicity, jet1_neutralhadronmultiplicity, "jet1_neutralhadronmultiplicity" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetChargedMultiplicity, jet1_chargedmultiplicity, "jet1_chargedmultiplicity" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetNeutralMultiplicity, jet1_neutralmultiplicity, "jet1_neutralmultiplicity" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetElectronMultiplicity, jet1_electronmultiplicity, "jet1_electronmultiplicity" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetMuonMultiplicity, jet1_muonmultiplicity, "jet1_muonmultiplicity" )
	PLOT_GRAPHERRORS_COND1( pline, DrawJetPhotonMultiplicity, jet1_photonmultiplicity, "jet1_photonmultiplicity" )
	PLOT_GRAPHERRORS_COND1( pline, DrawMatchAvgCaloJetPtRatio, calo_pf_avg_ratio_vs_pf_pt, "jet1_calo_match_ptratio" )
	PLOT_GRAPHERRORS_COND1( pline, DrawConstituents, jet1_constituents, "jet1_constituents" )
	PLOT_GRAPHERRORS_COND2( pline, DrawMatchAvgAvgCaloJetPtRatio, calo_avg_pf_avg_ratio_vs_z_pt , "jet1_calo_match_pt", "jet1_pt" )

	// Matched Z
	PLOT_GRAPHERRORS_COND1( pline, DrawJetRespGraph, matchedZ_ptratio, "matchedZ_ptratio" )

	return pline;
}

void AddConsumerToPipeline(ZJetPipeline * pline, std::string consumerName)
{
	if (consumerName == EventStorerConsumer().GetId())
	{
		pline->m_consumer.push_back(new EventStorerConsumer());
	}

	 if (consumerName == CutStatisticsConsumer(g_cutHandler.get()).GetId())
	 {
	 pline->m_consumer.push_back(new CutStatisticsConsumer(g_cutHandler.get()));
	 }
}

void AddConsumersToPipeline(ZJetPipeline * pline,
		std::vector<std::string> consList)
{
	BOOST_FOREACH( std::string s, consList )
	{
		CALIB_LOG( "Adding consumer " << s)
		AddConsumerToPipeline( pline, s);
	}
}

// Generates the default pipeline which is run on all events.
// insert new Plots here if you want a new plot
ZJetPipeline * CreateDefaultPipeline()
{
	ZJetPipeline * pline = new ZJetPipeline();
	//	pline->m_consumer.push_back(new EventDump());

	/*	for (int i = 0; i < 400; i++ )
	 {
	 std::stringstream sname;
	 sname << i << "jetresp";
	 */

	PLOT_HIST1D(pline, DrawPartonFlavourConsumer, partonflavour )

	PLOT_HIST1D(pline, DrawZMassConsumer, zmass)
	PLOT_HIST1D(pline, DrawZPtConsumer, z_pt)

	// Jet Pt
	PLOT_HIST1D_CONST1(pline, DrawJetPtConsumer, jet1_pt, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetPtConsumer, jet2_pt, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetPtConsumer, jet3_pt, 2)

	PLOT_HIST1D(pline, Draw2ndJetPtDivZPtConsumer, jet2_pt_over_z_pt)

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

	// Jet Delte Phi/Eta/R wrt to Jet1
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaRWrtJet1Consumer, jet2_delta_r_wrt_jet1, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaEtaWrtJet1Consumer, jet2_delta_eta_wrt_jet1, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetDeltaPhiWrtJet1Consumer, jet2_delta_phi_wrt_jet1, 1)

	PLOT_HIST1D(pline, DrawZEtaConsumer, z_eta)
	PLOT_HIST1D(pline, DrawZPhiConsumer, z_phi)

	// matched calo jets
	PLOT_HIST1D_CONST1(pline, DrawMatchCaloJetPtRatioConsumer, jet1_calo_match_ptratio, 0)
	PLOT_HIST1D_CONST1(pline, DrawMatchCaloJetPtRatioConsumer, jet2_calo_match_ptratio, 1)
	PLOT_HIST1D_CONST1(pline, DrawMatchCaloJetPtRatioConsumer, jet3_calo_match_ptratio, 2)

	PLOT_HIST1D_CONST1(pline, DrawMatchCaloJetPtConsumer, jet1_calo_match_pt, 0)
	PLOT_HIST1D_CONST1(pline, DrawMatchCaloJetPtConsumer, jet2_calo_match_pt, 1)
	PLOT_HIST1D_CONST1(pline, DrawMatchCaloJetPtConsumer, jet3_calo_match_pt, 2)

	// All Energy comparison (test if ==1)
	PLOT_HIST1D_CONST1(pline, DrawJetAllEnergyFractionPtConsumer, jet1_allenergy_fraction, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetAllEnergyFractionPtConsumer, jet2_allenergy_fraction, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetAllEnergyFractionPtConsumer, jet3_allenergy_fraction, 2)

	// ChargedHadron Energy
	PLOT_HIST1D_CONST1(pline, DrawJetChargedHadronEnergyFractionPtConsumer, jet1_chargedhadronenergy_fraction, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetChargedHadronEnergyFractionPtConsumer, jet2_chargedhadronenergy_fraction, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetChargedHadronEnergyFractionPtConsumer, jet3_chargedhadronenergy_fraction, 2)

	// NeutralHadron Energy
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralHadronEnergyFractionPtConsumer, jet1_neutralhadronenergy_fraction, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralHadronEnergyFractionPtConsumer, jet2_neutralhadronenergy_fraction, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralHadronEnergyFractionPtConsumer, jet3_neutralhadronenergy_fraction, 2)

	// ChargedEm Energy
	PLOT_HIST1D_CONST1(pline, DrawJetChargedEmEnergyFractionPtConsumer, jet1_chargedemenergy_fraction, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetChargedEmEnergyFractionPtConsumer, jet2_chargedemenergy_fraction, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetChargedEmEnergyFractionPtConsumer, jet3_chargedemenergy_fraction, 2)

	// NeutralEm Energy
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralEmEnergyFractionPtConsumer, jet1_neutralemenergy_fraction, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralEmEnergyFractionPtConsumer, jet2_neutralemenergy_fraction, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralEmEnergyFractionPtConsumer, jet3_neutralemenergy_fraction, 2)

	// Electron Energy
	PLOT_HIST1D_CONST1(pline, DrawJetElectronEnergyFractionPtConsumer, jet1_electronenergy_fraction, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetElectronEnergyFractionPtConsumer, jet2_electronenergy_fraction, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetElectronEnergyFractionPtConsumer, jet3_electronenergy_fraction, 2)

	// Muon Energy
	PLOT_HIST1D_CONST1(pline, DrawJetMuonEnergyFractionPtConsumer, jet1_muonenergy_fraction, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetMuonEnergyFractionPtConsumer, jet2_muonenergy_fraction, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetMuonEnergyFractionPtConsumer, jet3_muonenergy_fraction, 2)

	// Photon Energy
	PLOT_HIST1D_CONST1(pline, DrawJetPhotonEnergyFractionPtConsumer, jet1_photonenergy_fraction, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetPhotonEnergyFractionPtConsumer, jet2_photonenergy_fraction, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetPhotonEnergyFractionPtConsumer, jet3_photonenergy_fraction, 2)

	// Matched Z
	PLOT_HIST1D(pline, DrawZMatchConsumer, matchedZ_ptratio)

	// ChargedHadron Multiplicity
	PLOT_HIST1D_CONST1(pline, DrawJetChargedHadronMultiplicityConsumer, jet1_chargedhadronmultiplicity, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetChargedHadronMultiplicityConsumer, jet2_chargedhadronmultiplicity, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetChargedHadronMultiplicityConsumer, jet3_chargedhadronmultiplicity, 2)

	// NeutralHadron Multiplicity
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralHadronMultiplicityConsumer, jet1_neutralhadronmultiplicity, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralHadronMultiplicityConsumer, jet2_neutralhadronmultiplicity, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralHadronMultiplicityConsumer, jet3_neutralhadronmultiplicity, 2)

	// Charged Multiplicity
	PLOT_HIST1D_CONST1(pline, DrawJetChargedMultiplicityConsumer, jet1_chargedmultiplicity, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetChargedMultiplicityConsumer, jet2_chargedmultiplicity, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetChargedMultiplicityConsumer, jet3_chargedmultiplicity, 2)

	// Neutral Multiplicity
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralMultiplicityConsumer, jet1_neutralmultiplicity, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralMultiplicityConsumer, jet2_neutralmultiplicity, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetNeutralMultiplicityConsumer, jet3_neutralmultiplicity, 2)

	// Electron Multiplicity
	PLOT_HIST1D_CONST1(pline, DrawJetElectronMultiplicityConsumer, jet1_electronmultiplicity, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetElectronMultiplicityConsumer, jet2_electronmultiplicity, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetElectronMultiplicityConsumer, jet3_electronmultiplicity, 2)

	// Muon Multiplicity
	PLOT_HIST1D_CONST1(pline, DrawJetMuonMultiplicityConsumer, jet1_muonmultiplicity, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetMuonMultiplicityConsumer, jet2_muonmultiplicity, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetMuonMultiplicityConsumer, jet3_muonmultiplicity, 2)

	// Photon Multiplicity
	PLOT_HIST1D_CONST1(pline, DrawJetPhotonMultiplicityConsumer, jet1_photonmultiplicity, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetPhotonMultiplicityConsumer, jet2_photonmultiplicity, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetPhotonMultiplicityConsumer, jet3_photonmultiplicity, 2)

	// Constituents
	PLOT_HIST1D_CONST1(pline, DrawJetConstituentsConsumer, jet1_constituents, 0)
	PLOT_HIST1D_CONST1(pline, DrawJetConstituentsConsumer, jet2_constituents, 1)
	PLOT_HIST1D_CONST1(pline, DrawJetConstituentsConsumer, jet3_constituents, 2)

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

	for (ZJetCutHandler::CutVector::iterator it =
			g_cutHandler->GetCuts().begin(); !(it
			== g_cutHandler->GetCuts().end()); it++)
	{
		GraphErrors * hist_CutIneff = new GraphErrors;
		DrawCutIneffGraph<GraphXProviderZpt> * object_consumer =
				new DrawCutIneffGraph<GraphXProviderZpt> ((*it)->GetId());
		object_consumer->m_sQuantityName = "cut_ineff_"
				+ (*it)->GetCutShortName() + "_zpt";
		object_consumer->m_graph = hist_CutIneff;
		pline->m_consumer.push_back(object_consumer);

		hist_CutIneff = new GraphErrors;
		DrawCutIneffGraph<GraphXProviderRecoVert> * object_consumerReco =
				new DrawCutIneffGraph<GraphXProviderRecoVert> ((*it)->GetId());
		object_consumerReco->m_sQuantityName = "cut_ineff_"
				+ (*it)->GetCutShortName() + "_nrv";
		object_consumerReco->m_graph = hist_CutIneff;
		pline->m_consumer.push_back(object_consumerReco);
	}

	// event count
	PLOT_HIST1D(pline, DrawEventCount, eventcount)

	// Jet 1 Eta Phi map
	PLOT_HIST2D_CONST1(pline, DrawEtaPhiJetMapConsumer, etaphi_jet1_to_z, 0)
	// Jet 2 Eta Phi map
	PLOT_HIST2D_CONST1(pline, DrawEtaPhiJetMapConsumer, etaphi_jet2_to_z, 1)

	// Jet 2 Delta R to Jet1 map
	PLOT_HIST2D_CONST1(pline, DrawDeltaRJetMapConsumer, deltar_jet2_to_jet1_jet2_pt, 1)
	PLOT_HIST2D_CONST1(pline, DrawDeltaRJetRatioJetMapConsumer, deltar_jet2_to_jet1_ratiojet, 1)
	PLOT_HIST2D_CONST1(pline, DrawDeltaRJetRatioZMapConsumer, deltar_jet2_to_jet1_ratioz, 1)

	PLOT_HIST2D(pline, DrawPhiJet2PtConsumer, deltaphi_jet2_to_jet1_jet2_pt)
	PLOT_HIST2D(pline, DrawPhiJet2RatioConsumer, deltaphi_jet2_to_jet1_ratioz)
	PLOT_HIST2D(pline, DrawEtaJet2PtConsumer, deltaeta_jet2_to_jet1_jet2_pt)
	PLOT_HIST2D(pline, DrawEtaJet2RatioConsumer, deltaeta_jet2_to_jet1_ratioz)

	// sec / third jet activity ...
	PLOT_HIST2D(pline, DrawJetActivityRecoVertMapConsumer, secondary_jet_activity)
//PLOT_HIST2D(pline, Draw2ndJetCutNRVMapConsumer, secondary_jet_cut_over_nrv)

	PLOT_GRAPHERRORS( pline, DrawDeltaPhiRange, deltaphi_test )

	GraphErrors * hist_DrawJetPt = new GraphErrors;
	DrawJetPt<GraphXProviderJetPhiDeltaZ<0> > * object_DrawJetPt =
			new DrawJetPt<GraphXProviderJetPhiDeltaZ<0> > (0);
	object_DrawJetPt->m_sQuantityName = "jet1_pt_deltaphi";
	object_DrawJetPt->m_graph = hist_DrawJetPt;
	pline->m_consumer.push_back(object_DrawJetPt);

	hist_DrawJetPt = new GraphErrors;
	DrawJetPt<GraphXProviderJetPhiDeltaZ<1> > * object_DrawJet2Pt =
			new DrawJetPt<GraphXProviderJetPhiDeltaZ<1> > (1);
	object_DrawJet2Pt->m_sQuantityName = "jet2_pt_deltaphi";
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

	if (entries == 0)
	{
		CALIB_LOG_FATAL( "No input events in root files. Exiting ... ")
	}

	TString sNewFile = "";
	long lProcEvents = 0;
	unsigned long lOverallNumberOfProcessedEvents = 0;

	bool globalBinInitDone = false;
	// TODO dont do this if we have no weighting to analyze or the weighting is in the events themself
	//	if ( !g_useEventWeight && g_useWeighting)
	{
		CALIB_LOG_FILE( "Analyzing Events for weighting and Overall Event number" )
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
				lOverallNumberOfProcessedEvents += lProcEvents;

				double xsec = g_ev.xsection;
				if (g_globalXSection > 0.0f)
					xsec = g_globalXSection;

				if (g_useGlobalWeightBin && (!globalBinInitDone))
				{
					CALIB_LOG_FILE("Initializing global weighting bin with xsection " << xsec )
					g_mcWeighter.Reset();
					g_mcWeighter.AddBin(PtBin(0.0, 999999.0), xsec);
					globalBinInitDone = true;
				}

				g_mcWeighter.IncreaseCountByXSection(xsec, TMath::Nint(
						pH->GetMean()));
			}
		}
	}

	if (g_useWeighting)
		g_mcWeighter.Print();

	// cloning of a pipeline ?? goes here maybe
	// clone default pipeline for the number of settings we have
	g_pipelines.clear();

	ZJetPipelineInitializer plineInit;

	for (PipelineSettingsVector::iterator it = g_pipeSettings.begin(); !(it
			== g_pipeSettings.end()); it++)
	{
		if ((*it)->GetLevel() == 1)
		{
			ZJetPipeline * pLine = CreateDefaultPipeline();

			AddConsumersToPipeline(pLine, (*it)->GetAdditionalConsumer());

			// set the algo used for this run
			(*it)->SetAlgoName(g_sCurAlgo);
			(*it)->SetOverallNumberOfProcessedEvents(
					lOverallNumberOfProcessedEvents);

			pLine->InitPipeline(*it, plineInit);

			g_pipelines.push_back(pLine);
		}
	}

	CALIB_LOG_FILE( "Running " << g_pipelines.size() << " Pipeline(s) on events")
	CALIB_LOG_FILE( "Processing " << entries << " events ...")
	CALIB_LOG_FILE( "algo " << g_sCurAlgo)

	double fStartTime = omp_get_wtime();

	for (Long_t ievt = 0; ievt < entries; ++ievt)
	{
		g_pChain->GetEntry(ievt);

		EventResult * res = new EventResult;
		res->m_pData = &g_ev;

		// the weight of data events can be strange when read from root file. better reset here
		res->SetWeight(1.0f);
		res->m_bEventReweighting = g_eventReweighting;
		if (g_useWeighting)
		{
			if (g_useEventWeight)
			{
				res->SetWeight(res->m_pData->weight);
			}
			else
			{
				res->SetWeight(g_mcWeighter.GetWeightByXSection(
						GetEventXSection(*res)));
			}
		}

		calcJetEnergyCorrection(res, correction);
		RunPipelinesForEvent(*res);

		delete res;

		if (((ievt % 5000) == 0) || (ievt == (entries - 1)))
		{
			float localPercent = floor(100.0f * (float) (ievt + 1)
					/ (float) entries);
			float overallPercent = floor(((float) g_iCurAlgoCount
					/ (float) g_iAlgoOverallCount + (localPercent * 0.01f)
					* (1.0f) / (float) g_iAlgoOverallCount) * 100.0f);

			CALIB_LOG( (ievt + 1) << " of " << entries << " done [ this algo " << std::fixed << std::setprecision(0)
					<< localPercent << " % ] [ overall " << overallPercent << " % ]" )

		}
	}

	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)
	{
		it->FinishPipeline();
	}

	double fRuntime = omp_get_wtime() - fStartTime;
	CALIB_LOG_FILE(" ---- ")
	CALIB_LOG_FILE("All level 1 pipelines done. Overall time: " << fRuntime << " s")
	CALIB_LOG_FILE("Time per Event: " << std::setprecision(3) << ( (fRuntime / (double)entries) * 1000.0f ) << " ms" )
	CALIB_LOG_FILE("Time per Event and Pipeline: " << std::setprecision(3) <<
			( (fRuntime / (double)(entries * g_pipelines.size() )) * 1000.0f ) << " ms" )
	CALIB_LOG_FILE(" ---- ")
	CALIB_LOG_FILE("Running level 2 pipelines")

	// cloning of a pipeline ?? goes here maybe
	// clone default pipeline for the number of settings we have
	g_pipelines.clear();

	for (PipelineSettingsVector::iterator it = g_pipeSettings.begin(); !(it
			== g_pipeSettings.end()); it++)
	{
		if ((*it)->GetLevel() == 2)
		{
			ZJetPipeline * pLine = CreateLevel2Pipeline();

			AddConsumersToPipeline(pLine, (*it)->GetAdditionalConsumer());

			// set the algo used for this run
			(*it)->SetAlgoName(g_sCurAlgo);

			pLine->InitPipeline(*it, plineInit);
			g_pipelines.push_back(pLine);
		}
	}

	RunPipelines(2);
	for (PipelineVector::iterator it = g_pipelines.begin(); !(it
			== g_pipelines.end()); it++)
	{
		it->FinishPipeline();
	}

	g_pipelines.clear();
	CALIB_LOG_FILE("All level 2 pipelines done")
}

TChain * getChain(TString sName, evtData * pEv, std::string sRootfiles)
{
	TChain * mychain = new TChain(sName + "_Zplusjet");

	pEv->Z = new TParticle();
	pEv->jets[0] = new TParticle();
	pEv->jets[1] = new TParticle();
	pEv->jets[2] = new TParticle();

	pEv->matched_calo_jets[0] = new TParticle();
	pEv->matched_calo_jets[1] = new TParticle();
	pEv->matched_calo_jets[2] = new TParticle();

	pEv->pfProperties[0] = new PFProperties();
	pEv->pfProperties[1] = new PFProperties();
	pEv->pfProperties[2] = new PFProperties();

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
	mychain->SetBranchAddress("matched_Z", &pEv->matched_Z);
	mychain->SetBranchAddress("jet", &pEv->jets[0]);
	mychain->SetBranchAddress("jet2", &pEv->jets[1]);
	mychain->SetBranchAddress("jet3", &pEv->jets[2]);
	mychain->SetBranchAddress("mu_plus", &pEv->mu_plus);
	mychain->SetBranchAddress("mu_minus", &pEv->mu_minus);

	mychain->SetBranchAddress("met", &pEv->met);
	mychain->SetBranchAddress("tcmet", &pEv->tcmet);

	mychain->SetBranchAddress("matrix_element_flavour", &pEv->partonFlavour);

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

	mychain->SetBranchAddress("matched_calo_jet", &pEv->matched_calo_jets[0]);
	mychain->SetBranchAddress("matched_calo_jet2", &pEv->matched_calo_jets[1]);
	mychain->SetBranchAddress("matched_calo_jet3", &pEv->matched_calo_jets[2]);

	mychain->SetBranchAddress("pfjet_part1_properties", pEv->pfProperties[0]);
	mychain->SetBranchAddress("pfjet_part2_properties", pEv->pfProperties[1]);
	mychain->SetBranchAddress("pfjet_part3_properties", pEv->pfProperties[2]);

	return mychain;
}
/*
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
 */
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

ZJetPipelineSettings * CreateDefaultSettings(std::string sAlgoName,
		InputTypeEnum inpType)
{
	ZJetPipelineSettings * psetting = new ZJetPipelineSettings();
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

	TIMING_INIT

	TIMING_START( "config load" )




	std::string jsonConfig = argv[1];
	boost::property_tree::json_parser::read_json(jsonConfig, g_propTree);

	g_sOutputPath = g_propTree.get<std::string> ("OutputPath");
	std::string sLogFileName = g_sOutputPath + ".log";
	g_logFile = new ofstream(sLogFileName.c_str(), ios_base::trunc);

	CreateWeightBins();

	// openmp setup
	omp_set_num_threads(g_propTree.get<int> ("ThreadCount", 1));
	CALIB_LOG_FILE( "Running with " <<  omp_get_max_threads() << " thread(s)" )

	// input files
	g_sSource = g_propTree.get<std::string> ("InputFiles");
	CALIB_LOG_FILE("Using InputFiles " << g_sSource)

	// removes the old file
	std::string sRootOutputFilename = (g_sOutputPath + ".root");

	//Todo: close file to free memory of already written histos
	g_resFile = new TFile(sRootOutputFilename.c_str(), "RECREATE");
	CALIB_LOG_FILE("Writing to the root file " << sRootOutputFilename)

	// insert config into log file
	CALIB_LOG_FILE( "Configuration file " << jsonConfig << " dump:" );
	boost::property_tree::json_parser::write_json(*g_logFile, g_propTree);


	/*
	 g_sTrackedEventsFile = p.getString(secname + ".tracked_events");
	 if (g_sTrackedEventsFile.length() > 0)
	 loadTrackedEventsFromFile(g_sTrackedEventsFile);
	 */

	// weighting settings
	g_useEventWeight = g_propTree.get<bool> ("UseEventWeight", false);
	g_useWeighting = g_propTree.get<bool> ("UseWeighting", false);

	g_useGlobalWeightBin = g_propTree.get<bool> ("UseGlobalWeightBin", false);
	g_globalXSection = g_propTree.get<double> ("GlobalXSection", 0.0f);

	g_eventReweighting = g_propTree.get<bool> ("EventReweighting", false);

	if (g_eventReweighting)
		CALIB_LOG_FILE( "\n\n --------> reweightin events for # reco !!\n\n" )

	if ( g_propTree.get("JsonFile", "") != "" )
		g_json.reset(new Json_wrapper(g_propTree.get("JsonFile", "").c_str()));

	/*
	 g_l2CorrFiles = p.getvString(secname + ".l2_correction_data");
	 g_l3CorrFiles = p.getvString(secname + ".l3_correction_data");
	 */

	g_cutHandler.reset(new ZJetCutHandler());

	// init cuts
	// values are set for each Pipeline individually

	// technical cuts
	 g_ZJetCuts.push_back(new JsonCut(g_json.get()));
	 g_ZJetCuts.push_back(new HltCut());

	 // muon cuts
	 g_ZJetCuts.push_back(new MuonEtaCut());
	 g_ZJetCuts.push_back(new MuonPtCut() );
	 g_ZJetCuts.push_back(new ZMassWindowCut());
	 g_ZJetCuts.push_back(new ZPtCut());

	 // jet cuts
	 g_ZJetCuts.push_back(new LeadingJetEtaCut());
	 g_ZJetCuts.push_back(new JetPtCut());

	 // topology cuts
	 //g_ZJetCuts.push_back(new SecondLeadingToZPtCutDir());
	 g_ZJetCuts.push_back(new SecondLeadingToZPtCut());
	 g_ZJetCuts.push_back(new BackToBackCut());


	BOOST_FOREACH( ZJetCutBase * pCut, g_ZJetCuts )
{	g_cutHandler->AddCut( pCut );
}

ZJetPipelineSettings * pset = NULL;

BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		g_propTree.get_child("Pipelines") )
{	pset = new ZJetPipelineSettings();
	pset->SetPropTree( &g_propTree );

	std::string sKeyName = v.first.data();
	pset->SetSettingsRoot("Pipelines." + sKeyName);
	pset->SetRootOutFile(g_resFile);

	g_pipeSettings.push_back( pset );
}

std::cout << TIMING_GET_RESULT_STRING << std::endl;

processAlgo();
g_resFile->Close();
g_logFile->close();

CALIB_LOG_FILE("Output file " << sRootOutputFilename << " closed.")

// todo this delete produces seg fault
delete g_logFile;

return 0;
}

