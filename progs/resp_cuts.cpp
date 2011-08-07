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
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "DataFormats/interface/Kappa.h"
#include "DataFormats/interface/KDebug.h"
//#include "Toolbox/libToolbox.h"
//#include "RootTools/libKRootTools.h"

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
#include "EventPipelineRunner.h"

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

vString g_lCorrFiles;

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

void AddConsumerToPipeline(ZJetPipeline * pline, std::string consumerName)
{/*
 if (consumerName == EventStorerConsumer().GetId())
 {
 pline->m_consumer.push_back(new EventStorerConsumer());
 }

 if (consumerName == CutStatisticsConsumer(g_cutHandler.get()).GetId())
 {
 pline->m_consumer.push_back(new CutStatisticsConsumer(g_cutHandler.get()));
 }*/
}

void AddConsumersToPipeline(ZJetPipeline * pline,
		std::vector<std::string> consList)
{
	BOOST_FOREACH( std::string s, consList )
				{	CALIB_LOG( "Adding consumer " << s)
		AddConsumerToPipeline( pline, s);
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

class ZJetEventProvider: public EventProvider<ZJetEventData>
{
public:
	ZJetEventProvider(FileInterface & fi) :
		m_fi(fi)
	{
		// setup pointer to collections
		m_event.m_eventmetadata = fi.Get<KEventMetadata> ();

		InitPFJets( m_event, "AK5PFJets" );
		InitPFJets( m_event, "AK7PFJets" );
		InitPFJets( m_event, "KT4PFJets" );
		InitPFJets( m_event, "KT6PFJets" );
	}

	virtual bool GotoEvent(long long lEvent)
	{
		m_fi.eventdata.GetEntry(lEvent);

		//CALIB_LOG( "Event " << m_eventmetadata->nEvent << " Lumi " << m_eventmetadata->nLumi << " Run " << m_eventmetadata->nRun )

		/*CALIB_LOG( "Event " << m_eventmetadata->nEvent << " Lumi " << m_eventmetadata->nLumi
		 << " Run " << m_eventmetadata->nRun << "PF " << m_kPF->size())
		 */
		return true;
	}

	virtual ZJetEventData const& GetCurrentEvent() const
	{
		return m_event;
	}

	virtual long long GetOverallEventCount() const
	{
		return m_fi.eventdata.GetEntries();
	}

private:
	void InitPFJets( ZJetEventData & event, std::string algoName )
	{
		event.m_pfJets[algoName] = m_fi.Get<KDataPFJets>(algoName);
	}


	ZJetEventData m_event;
private:

	FileInterface & m_fi;
};

struct ConsumerConfig
{
public:
	int EventDataID;
};

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
	CALIB_LOG_FILE( "Running with " << omp_get_max_threads() << " thread(s)" )

	// input files
	g_sSource = g_propTree.get<std::string> ("InputFiles");
	CALIB_LOG_FILE("Using InputFiles " << g_sSource)

	FileInterface fi(vector<string> (1, g_sSource));

	std::vector<std::string> sJetNames = fi.GetNames<KDataJet> (true);
	BOOST_FOREACH( std::string s, sJetNames)
	{	std::cout << "KDataJet " << s << std::endl;
	}

	sJetNames = fi.GetNames<KDataPFJets>(true);

	BOOST_FOREACH( std::string s, sJetNames)
	{
		std::cout << "KDataLV " << s << std::endl;
	}

	ZJetEventProvider evtProvider( fi );

	/*KDataPFJets * myJets = fi.Get<KDataPFJets>("AK5PFJets");
 evtProvider.m_data.PF_jets = myJets;*/

	/*	typedef std::pair<std::string, ConsumerConfig > configpair;
 typedef std::vector<std::pair<std::string, ConsumerConfig > > configpairvector;

 configpairvector LVPlots(1,
 configpair( "jet1_ak5PF",
 ConsumerConfig()
 ));
	 */
	/*s.second.EventDataID =
 evtProvider.GetEventData().AddDataLV( fi.Get<KDataPFJets>( "AK5PFJets" )[0] );
	 */

	// removes the old file
	std::string sRootOutputFilename = (g_sOutputPath + ".root");

	//Todo: close file to free memory of already written histos
	g_resFile = new TFile(sRootOutputFilename.c_str(), "RECREATE");
	CALIB_LOG_FILE("Writing to the root file " << sRootOutputFilename)

	// insert config into log file
	CALIB_LOG_FILE( "Configuration file " << jsonConfig << " dump:" );
	boost::property_tree::json_parser::write_json(*g_logFile, g_propTree);

	EventPipelineRunner<ZJetPipeline> pRunner;

	// cloning of a pipeline ?? goes here maybe
	// clone default pipeline for the number of settings we have
	g_pipelines.clear();

	ZJetPipelineInitializer plineInit;

	ZJetPipelineSettings * pset = NULL;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
			g_propTree.get_child("Pipelines") )
	{
		pset = new ZJetPipelineSettings();
		pset->SetPropTree( &g_propTree );

		std::string sKeyName = v.first.data();
		pset->SetSettingsRoot("Pipelines." + sKeyName);
		pset->SetRootOutFile(g_resFile);

		g_pipeSettings.push_back( pset );
	}

	// get pointers to the interesting collections
	typedef std::map<std::string, KDataPFJets * > PfMap;
	PfMap pfJets;

	/*	pfJets["AK7PFJets"]= ( fi.Get<KDataPFJets>("AK7PFJets") );
	pfJets["KT4PFJets"]= ( fi.Get<KDataPFJets>("KT4PFJets") );
	pfJets["KT6PFJets"] =( fi.Get<KDataPFJets>("KT6PFJets") );*/

	for (PipelineSettingsVector::iterator it = g_pipeSettings.begin(); !(it
			== g_pipeSettings.end()); it++)
	{
		if ((*it)->GetLevel() == 1)
		{
			ZJetPipeline * pLine = new ZJetPipeline;//CreateDefaultPipeline();

			pLine->AddConsumer(	new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 0));
			pLine->AddConsumer(	new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 1));
			pLine->AddConsumer(	new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 2));
			pLine->AddConsumer(	new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 3));
			pLine->AddConsumer(	new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 4));

			pLine->InitPipeline(*it, plineInit);
			pRunner.AddPipeline( pLine );
		}
	}

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

/*	BOOST_FOREACH( ZJetCutBase * pCut, g_ZJetCuts )
	{
		g_cutHandler->AddCut( pCut );
	}*/

	std::cout << TIMING_GET_RESULT_STRING << std::endl;

	pRunner.RunPipelines<ZJetEventData>( evtProvider );

	g_resFile->Close();
	g_logFile->close();

	CALIB_LOG_FILE("Output file " << sRootOutputFilename << " closed.")

	// todo this delete produces seg fault
	delete g_logFile;

	return 0;
}

