/*

 CalibFW resp_cuts.cpp:

 HOWTO:

 # Add a new Plot:

 - DrawBase.h
 1) Use the IMPL_HIST2D_MOD1 to generate a Consumer class which plots a histogramm. If your plotting requirement is
 more complex, derive from the class DrawHist1dConsumerBase<EventResult> (for 1d Histo) and implement
 ProcessFilteredEvent.

 - resp_cuts.h: CreateDefaultPipeline
 2) Use the PLOT_HIST2D macro to add your histogram the default plotting pipeline and give a name which is used to store
 the histo in the root file.

 > done
 */


#include "RootTools/RootIncludes.h"

#include <stdio.h>
#include <stdlib.h>

#include <execinfo.h>
#include <signal.h>


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
#include <boost/algorithm/string.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>




#ifdef USE_PERFTOOLS
#include <google/profiler.h>
#endif
//#include <google/heap-profiler.h>


//#include "Toolbox/libToolbox.h"
//#include "RootTools/libKRootTools.h"

#include "Misc/OpenMP-Support.h"

/*
 #include <iostream>
 #include <hash_map>
 */
//#include "Json_wrapper.h"

#include "Pipeline/JetTools.h"

#include "ZJet/ZJetPipeline.h"

#include "ZJet/ZJetEventProvider.h"


#include "ZJet/Consumer/GenericProfileConsumer.h"
#include "ZJet/Consumer/ZJetDrawConsumer.h"
#include "ZJet/MetaDataProducer/ZJetMetaDataProducer.h"
#include "ZJet/MetaDataProducer/ZJetCuts.h"
#include "ZJet/MetaDataProducer/PuReweightingProducer.h"
#include "ZJet/MetaDataProducer/CorrJetProducer.h"
#include "ZJet/MetaDataProducer/JetSorter.h"
#include "ZJet/MetaDataProducer/HltSelector.h"
#include "ZJet/MetaDataProducer/JetMatcher.h"


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

PipelineSettingsVector g_pipeSettings;


void AddGlobalMetaProducer( std::vector< std::string > const& producer,
		EventPipelineRunner<ZJetPipeline, ZJetGlobalMetaDataProducerBase> & runner,
		boost::property_tree::ptree & globalSettings)
{
	// extend here, if you want to provide a new global meta producer
	for ( std::vector< std::string >::const_iterator it = producer.begin();
			it != producer.end(); ++ it )
	{
		if ( ValidMuonProducer::Name() == (*it) )
			runner.AddGlobalMetaProducer( new ValidMuonProducer());
		else if ( ZProducer::Name() == (*it) )
			runner.AddGlobalMetaProducer( new ZProducer());
		else if ( PuReweightingProducer::Name() == (*it))
			runner.AddGlobalMetaProducer( new PuReweightingProducer());
		else if ( ValidJetProducer::Name() == (*it) )
			runner.AddGlobalMetaProducer( new ValidJetProducer());
		else if ( CorrJetProducer::Name() == (*it) )
			runner.AddGlobalMetaProducer( new CorrJetProducer( globalSettings.get<std::string> ("JecBase"),
                                     globalSettings.get<std::string> ("L1Correction"),
                                     PropertyTreeSupport::GetAsStringList(&globalSettings,"GlobalAlgorithms")));
		else if ( JetSorter::Name() == (*it))
			runner.AddGlobalMetaProducer( new JetSorter());
		else if ( HltSelector::Name() == (*it))
			runner.AddGlobalMetaProducer( new HltSelector( PropertyTreeSupport::GetAsStringList( &globalSettings, "HltPaths", true ) ));
		else if ( JetMatcher::Name() == (*it))
                         runner.AddGlobalMetaProducer( new JetMatcher(PropertyTreeSupport::GetAsStringList(&globalSettings,"GlobalAlgorithms")) );
		else
			CALIB_LOG_FATAL( "Global MetaData producer of name " << (*it) << " not found")
	}
}
/* did not work
void handler(int sig) {
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, 2);
	exit(1);
}*/


int main(int argc, char** argv)
{
	// install signal
	// did not work
	//signal(SIGSEGV, handler);   // install our handler


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

	// input files

	// hast GC the file list ?
	char * pPath;
	pPath = getenv ("FILE_NAMES");
	if (pPath!=NULL)
	{
		boost::split( g_sourcefiles, pPath, boost::is_any_of(" "), boost::token_compress_on );
		// SplitVec == { "hello abc","ABC","aBc goodbye" }
	}
	else
	{
		CALIB_LOG_FILE("Getting file list from env-variable FILE_NAMES" )
		g_sourcefiles = PropertyTreeSupport::GetAsStringList(&g_propTree,
			"InputFiles");

		if (g_sourcefiles.size() == 0)
		{
			CALIB_LOG_FATAL("No Kappa input files specified")
		}

		BOOST_FOREACH(std::string s, g_sourcefiles)
		{
			CALIB_LOG_FILE("Input File " << s)
		}
	}
	FileInterface finterface(g_sourcefiles);


	// setup Global Settings
	ZJetGlobalSettings gset;

	gset.SetEnablePuReweighting( g_propTree.get<bool> ("EnablePuReweighting", false) );
	gset.SetEnable2ndJetReweighting( g_propTree.get<bool> ("Enable2ndJetReweighting", false) );
	gset.SetMuonID2011(g_propTree.get<bool>("MuonID2011", false));

	//std::vector<std::string> sJetNames = fi.GetNames<KDataJet> (true);

	if ( g_propTree.get<std::string> ("InputType", "mc") == "data")
	{
		gset.SetJsonFile( g_propTree.get< std::string > ("JsonFile") );

		g_inputType = DataInput;
	}
	else
	{
		gset.m_recovertWeight = PropertyTreeSupport::GetAsDoubleList(&g_propTree, "RecovertWeight");
		gset.m_2ndJetWeight = PropertyTreeSupport::GetAsDoubleList(&g_propTree, "2ndJetWeight");

		g_inputType = McInput;
	}

	gset.SetInputType ( g_inputType );
	//sJetNames = fi.GetNames<KVertexSummary>(true);

	ZJetEventProvider evtProvider( finterface, g_inputType, g_propTree.get<bool> ("UseMETPhiCorrection") );

	// removes the old file
	std::string sRootOutputFilename = (g_sOutputPath + ".root");

	//Todo: close file to free memory of already written histos
	g_resFile = new TFile(sRootOutputFilename.c_str(), "RECREATE");
	CALIB_LOG_FILE("Writing to the root file " << sRootOutputFilename)

	// insert config into log file
	CALIB_LOG_FILE( "Configuration file " << jsonConfig << " dump:" );
	boost::property_tree::json_parser::write_json(*g_logFile, g_propTree);


	ZJetPipelineInitializer plineInit;

	ZJetPipelineSettings * pset = NULL;

	EventPipelineRunner<ZJetPipeline, ZJetGlobalMetaDataProducerBase> pRunner;

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

	stringvector globalProducer = PropertyTreeSupport::GetAsStringList(&g_propTree,
									"GlobalProducer");

	AddGlobalMetaProducer( globalProducer, pRunner, g_propTree );

	for (PipelineSettingsVector::iterator it = g_pipeSettings.begin(); !(it
			== g_pipeSettings.end()); it++)
	{
		(*it)->m_globalSettings = & gset;

		if ((*it)->GetLevel() == 1)
		{
			ZJetPipeline * pLine = new ZJetPipeline;//CreateDefaultPipeline();

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

	// delete the pipeline settings
		for (PipelineSettingsVector::iterator it = g_pipeSettings.begin(); !(it
			== g_pipeSettings.end()); it++)
	{
		delete (*it);
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

