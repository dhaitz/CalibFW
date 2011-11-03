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
#include "RootTools/RootIncludes.h"

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <iostream>
#include <ios>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#ifdef USE_PERFTOOLS
#include <google/profiler.h>
#endif
//#include <google/heap-profiler.h>

#include "DataFormats/interface/Kappa.h"
#include "DataFormats/interface/KDebug.h"
//#include "Toolbox/libToolbox.h"
//#include "RootTools/libKRootTools.h"

#include "Misc/OpenMP-Support.h"

/*
 #include <iostream>
 #include <hash_map>
 */
//#include "Json_wrapper.h"

#include "ZJet/ZJetPipeline.h"

#include "ZJet/Consumer/GenericProfileConsumer.h"
#include "ZJet/Consumer/ZJetDrawConsumer.h"
#include "ZJet/MetaDataProducer/ZJetMetaDataProducer.h"
#include "ZJet/MetaDataProducer/ZJetCuts.h"
#include "ZJet/MetaDataProducer/PuReweightingProducer.h"
#include "ZJet/MetaDataProducer/CorrJetProducer.h"
#include "ZJet/MetaDataProducer/JetSorter.h"
#include "ZJet/MetaDataProducer/HltInfoProducer.h"


#include "KappaTools/RootTools/HLTTools.h"

#include "ZJet/ZJetPipelineInitializer.h"
#include "Pipeline/EventPipelineRunner.h"

using namespace CalibFW;

/* BASIC CONFIGURATION */

// DP made the variables not const to set them by command line args


stringvector g_sourcefiles;


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

InputTypeEnum g_inputType;

long g_lOverallNumberOfProcessedEvents = 0;

std::map<std::string, std::string> g_l2CorrData;

TFile * g_resFile;

typedef std::vector<ZJetPipelineSettings *> PipelineSettingsVector;
typedef boost::ptr_vector<ZJetPipeline> PipelineVector;

PipelineSettingsVector g_pipeSettings;
PipelineVector g_pipelines;

class ZJetEventProvider: public EventProvider<ZJetEventData>
{
public:
	ZJetEventProvider(FileInterface & fi, InputTypeEnum inpType) :
		m_prevRun(-1), m_prevLumi(-1), m_inpType(inpType), m_fi(fi)
		{


		// setup pointer to collections
		m_event.m_eventmetadata = fi.Get<KEventMetadata> ();

		if (inpType == McInput)
		{
			m_event.m_geneventmetadata = fi.Get<KGenEventMetadata> ();
		}

		// open Kappa issue, disable the check and it will work
		m_event.m_vertexSummary = fi.Get<KVertexSummary> (
				"offlinePrimaryVerticesSummary", false);
		m_event.m_jetArea = fi.Get<KJetArea> ("KT6Area");

		//InitPFJets(m_event, "AK5PFJets");
		// dont load corrected jet here, we will do this offline
		InitPFJets(m_event, "AK5PFJets");
		InitPFJets(m_event, "AK7PFJets");

		m_event.m_muons = fi.Get<KDataMuons> ("muons");
		m_event.m_pfMet = fi.Get<KDataPFMET> ("PFMET");

		m_event.m_fi = &fi;

		fi.SpeedupTree();

		m_mon.reset(new ProgressMonitor(GetOverallEventCount()));
		}

	virtual bool GotoEvent(long long lEvent, std::shared_ptr< HLTTools > & hltInfo )
	{
		m_mon->Update();
		m_fi.eventdata.GetEntry(lEvent);

		if (m_prevRun != m_event.m_eventmetadata->nRun)
		{
			m_prevRun = m_event.m_eventmetadata->nRun;
			m_prevLumi = -1;
		}

		if (m_prevLumi != m_event.m_eventmetadata->nLumi)
		{
			m_prevLumi = m_event.m_eventmetadata->nLumi;

			// load the correct lumi information
			if (m_inpType == McInput)
			{
				m_event.m_lumimetadata = m_fi.Get<KGenLumiMetadata> (
						m_event.m_eventmetadata->nRun,
						m_event.m_eventmetadata->nLumi);
			}
			if (m_inpType == DataInput)
			{
				m_event.m_lumimetadata = m_fi.Get<KDataLumiMetadata> (
						m_event.m_eventmetadata->nRun,
						m_event.m_eventmetadata->nLumi);
			}

			// reload the HLT information associated with this lumi
			hltInfo->setLumiMetadata( m_event.m_lumimetadata );
		}

		/*	if ( lEvent > 5 )
		 exit(0);*/

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
	void InitPFJets(ZJetEventData & event, std::string algoName)
	{
		event.m_pfJets[algoName] = m_fi.Get<KDataPFJets> (algoName);
	}
	void InitGenJets(ZJetEventData & event, std::string algoName)
	{
		event.m_genJets[algoName] = m_fi.Get<KLV> (algoName);
	}

	long m_prevRun, m_prevLumi;
	ZJetEventData m_event;
	InputTypeEnum m_inpType;
	boost::scoped_ptr<ProgressMonitor> m_mon;
private:

	FileInterface & m_fi;
};


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
	g_logFile = new ofstream(sLogFileName.c_str(), std::ios_base::trunc);

	// openmp setup
	//omp_set_num_threads(g_propTree.get<int> ("ThreadCount", 1));
	//	CALIB_LOG_FILE( "Running with " << omp_get_max_threads() << " thread(s)" )

	// input files
	g_sourcefiles = PropertyTreeSupport::GetAsStringList(&g_propTree,
					"InputFiles");

	if (g_sourcefiles.size() == 0)
	{
		CALIB_LOG_FATAL("No Kappa input files specified")
	}

	BOOST_FOREACH( std::string s, g_sourcefiles)
	{	CALIB_LOG_FILE("Input File " << s)
	}

	FileInterface fi(g_sourcefiles);

	// setup Global Settings
	ZJetGlobalSettings gset;

	gset.SetEnablePuReweighting( g_propTree.get<bool> ("EnablePuReweighting", false) );

	std::vector<std::string> sJetNames = fi.GetNames<KDataJet> (true);

	if ( g_propTree.get<std::string> ("InputType", "mc") == "data")
	{
		gset.SetJsonFile( g_propTree.get< std::string > ("JsonFile") );

		g_inputType = DataInput;
	}
	else
	{
		gset.m_recovertWeight = PropertyTreeSupport::GetAsDoubleList(&g_propTree, "RecovertWeight");

		g_inputType = McInput;
	}

	gset.SetInputType ( g_inputType );

	sJetNames = fi.GetNames<KVertexSummary>(true);

	ZJetEventProvider evtProvider( fi, g_inputType );

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

	// cloning of a pipeline ?? goes here maybe
	// clone default pipeline for the number of settings we have
	g_pipelines.clear();

	ZJetPipelineInitializer plineInit;

	ZJetPipelineSettings * pset = NULL;

	EventPipelineRunner<ZJetPipeline, ZJetMetaDataProducerBase> pRunner;

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

	//pRunner.AddGlobalMetaProducer( new HltInfoProducer());
	pRunner.AddGlobalMetaProducer( new PuReweightingProducer());
	pRunner.AddGlobalMetaProducer( new ValidMuonProducer());
	pRunner.AddGlobalMetaProducer( new ZProducer());
	pRunner.AddGlobalMetaProducer( new ValidJetProducer());
	pRunner.AddGlobalMetaProducer( new CorrJetProducer( g_propTree.get<std::string> ("JecBase") ));


	pRunner.AddGlobalMetaProducer( new JetSorter());

	for (PipelineSettingsVector::iterator it = g_pipeSettings.begin(); !(it
			== g_pipeSettings.end()); it++)
	{
		(*it)->m_globalSettings = & gset;

		if ((*it)->GetLevel() == 1)
		{
			ZJetPipeline * pLine = new ZJetPipeline;//CreateDefaultPipeline();

			// does not work at the moment due to an error about wrong root dictionaries
			//pLine->AddConsumer(	new PrimaryVertexConsumer());


			pLine->AddConsumer( new DataZConsumer( (*it)->GetJetAlgorithm() ));

			pLine->AddConsumer( new DataMuonConsumer(+1, (*it)->GetJetAlgorithm()));
			pLine->AddConsumer( new DataMuonConsumer(-1, (*it)->GetJetAlgorithm()));

			pLine->AddConsumer( new ValidMuonsConsumer());
			pLine->AddConsumer( new ValidJetsConsumer());

			pLine->AddConsumer( new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 0));
			pLine->AddConsumer( new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 1));
			pLine->AddConsumer( new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 2));
			pLine->AddConsumer( new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 3));
			pLine->AddConsumer( new DataPFJetsConsumer( (*it)->GetJetAlgorithm(), 4));

			if ( g_inputType == McInput )
			{
				// add gen jets plots
				//pLine->AddConsumer( new DataLVsConsumer( (*it)->GetJetAlgorithm() + "_gen_", 0, (*it)->GetGenJetAlgorithm() ));
			}

			if ( g_inputType == McInput )
			{
				pLine->AddConsumer( new GenMetadataConsumer( ) );
			}
			else
			{
				pLine->AddConsumer( new MetadataConsumer( ) );
			}

			pLine->InitPipeline( *(*it), plineInit);
			pRunner.AddPipeline( pLine );
		}

		if ((*it)->GetLevel() == 2)
		{
			ZJetPipeline * pLine = new ZJetPipeline;//CreateDefaultPipeline();

			pLine->InitPipeline( *(*it), plineInit);
			pRunner.AddPipeline( pLine );
		}
	}

	// weighting settings
	g_useEventWeight = g_propTree.get<bool> ("UseEventWeight", false);
	g_useWeighting = g_propTree.get<bool> ("UseWeighting", false);

	g_useGlobalWeightBin = g_propTree.get<bool> ("UseGlobalWeightBin", false);
	g_globalXSection = g_propTree.get<double> ("GlobalXSection", 0.0f);

	g_eventReweighting = g_propTree.get<bool> ("EventReweighting", false);


	if (g_eventReweighting)
		CALIB_LOG_FILE( "\n\n --------> reweightin events for # reco !!\n\n" )

	ZJetPipelineSettings settings;
	settings.m_globalSettings = &gset;

#ifdef USE_PERFTOOLS
	ProfilerStart( "closure.prof");
#endif
	//HeapProfilerStart( "resp_cuts.heap");

	pRunner.RunPipelines<ZJetEventData, ZJetMetaData, ZJetPipelineSettings >( evtProvider, settings );

	//HeapProfilerStop();
#ifdef USE_PERFTOOLS
	ProfilerStop();
#endif

	g_resFile->Close();
	g_logFile->close();

	CALIB_LOG_FILE("Output file " << sRootOutputFilename << " closed.")

	// todo this delete produces seg fault
	delete g_logFile;

	return 0;
}

