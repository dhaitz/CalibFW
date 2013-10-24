/** The main program Artus */

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
//#include "Misc/OpenMP-Support.h"

#include "EventPipeline/JetTools.h"
#include "EventPipeline/EventPipelineRunner.h"
#include "ZJetEventPipeline/Pipeline.h"
#include "ZJetEventPipeline/EventProvider.h"
#include "ZJetProducer/MetadataProducer.h"
#include "ZJetProducer/Cuts.h"
#include "ZJetProducer/WeightProducer.h"
#include "ZJetProducer/CorrJetProducer.h"
#include "ZJetProducer/JetSorter.h"
#include "ZJetProducer/HltSelector.h"
#include "ZJetProducer/JetMatcher.h"
#include "ZJetProducer/TypeIMETProducer.h"
#include "ZJetProducer/UnclusteredEnergyProducer.h"
#include "ZJetProducer/FlavourProducer.h"
#include "ZJetEventPipeline/PipelineInitializer.h"

#include "KappaTools/RootTools/HLTTools.h"

using namespace CalibFW;

/* BASIC CONFIGURATION */

// DP made the variables not const to set them by command line args
stringvector g_sourcefiles;
std::string g_sJsonFile("not set");
std::string g_sOutputPath = "default_zjetres";
std::string g_sTrackedEventsFile;

boost::property_tree::ptree g_propTree;

InputTypeEnum g_inputType;

long g_lOverallNumberOfProcessedEvents = 0;

std::map<std::string, std::string> g_l2CorrData;

TFile* g_resFile;

typedef std::vector<ZJetPipelineSettings*> PipelineSettingsVector;

PipelineSettingsVector g_pipeSettings;


void AddGlobalMetaProducer(std::vector<std::string> const& producer,
		EventPipelineRunner<ZJetPipeline, ZJetGlobalMetaDataProducerBase> & runner,
		boost::property_tree::ptree & globalSettings)
{
	// extend here, if you want to provide a new global meta producer
	for (std::vector<std::string>::const_iterator it = producer.begin();
		 it != producer.end(); ++it)
	{
		if (ValidMuonProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ValidMuonProducer());
		else if (ZProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ZProducer());
		else if (WeightProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new WeightProducer());
		else if (ValidJetProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ValidJetProducer(
					globalSettings.get<bool>("Tagged")));
		else if (CorrJetProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new CorrJetProducer(
					globalSettings.get<std::string>("Jec"),
					globalSettings.get<std::string>("L1Correction"),
					PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));
		else if (JetSorter::Name() == *it)
			runner.AddGlobalMetaProducer(new JetSorter());
		else if (HltSelector::Name() == *it)
			runner.AddGlobalMetaProducer(new HltSelector(PropertyTreeSupport::GetAsStringList(&globalSettings, "HltPaths", true)));
		else if (JetMatcher::Name() == *it)
			runner.AddGlobalMetaProducer(new JetMatcher(PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));
		else if (GenProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new GenProducer());
		else if (GenBalanceProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new GenBalanceProducer());
		else if (GenDibalanceProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new GenDibalanceProducer());
		else if (TypeIMETProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new TypeIMETProducer(
					globalSettings.get<bool>("EnableMetPhiCorrection"),
					PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));

		else if (UnclusteredEnergyProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new UnclusteredEnergyProducer(
					PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));
		else if (FlavourProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new FlavourProducer());
		else
			CALIB_LOG_FATAL("Global MetaData producer of name " << *it << " not found");
	}
}


int main(int argc, char** argv)
{
	long long nevents = 0;
	// install signal
	// did not work
	//signal(SIGSEGV, handler);   // install our handler

	// usage
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0]
				  << " json_config_file.json [VerboseLevel]\n";
		return 1;
	}

	// read config and check
	std::string jsonConfig = argv[1];
	boost::property_tree::json_parser::read_json(jsonConfig, g_propTree);

	g_sOutputPath = g_propTree.get<std::string> ("OutputPath");
	std::string sLogFileName = g_sOutputPath + ".log";
	g_logFile = new std::ofstream(sLogFileName.c_str(), std::ios_base::trunc);

	// insert config into log file
	CALIB_LOG_FILE("Configuration file: " << jsonConfig);
	boost::property_tree::json_parser::write_json(*g_logFile, g_propTree);


	// input files

	char* pPath;
	pPath = getenv("FILE_NAMES");
	if (pPath != NULL)
	{
		boost::split(g_sourcefiles, pPath, boost::is_any_of(" "), boost::token_compress_on);
		// SplitVec == { "hello abc","ABC","aBc goodbye" }
	}
	else
	{
		g_sourcefiles = PropertyTreeSupport::GetAsStringList(&g_propTree, "InputFiles");
		if (g_sourcefiles.size() == 0)
		{
			CALIB_LOG_FATAL("No Kappa input files specified.");
		}
		else
		{
			CALIB_LOG_FILE("Input files (" << g_sourcefiles.size() << "):");
		}
	}
	// removes the old file
	std::string sRootOutputFilename = g_sOutputPath + ".root";
	g_resFile = new TFile(sRootOutputFilename.c_str(), "RECREATE");

	{
		FileInterface2 finterface(g_sourcefiles);
		CALIB_LOG_FILE("Output file: " << sRootOutputFilename);

		// setup Global Settings
		ZJetGlobalSettings gset;

		gset.SetEnablePuReweighting(g_propTree.get<bool>("EnablePuReweighting", false));
		gset.SetEnable2ndJetReweighting(g_propTree.get<bool>("Enable2ndJetReweighting", false));
		gset.SetEnableSampleReweighting(g_propTree.get<bool>("EnableSampleReweighting", false));
		gset.SetEnableLumiReweighting(g_propTree.get<bool>("EnableLumiReweighting", false));
		gset.SetXSection(g_propTree.get<double>("XSection", -1));
		gset.SetEnableMetPhiCorrection(g_propTree.get<bool>("EnableMetPhiCorrection", false));
		gset.SetMuonID2011(g_propTree.get<bool>("MuonID2011", false));
		gset.SetHcalCorrection(g_propTree.get<double>("HcalCorrection", 0.0));
		gset.SetSkipEvents(g_propTree.get<long long>("SkipEvents", 0));
		gset.SetEventCount(g_propTree.get<long long>("EventCount", -1));



		if (g_propTree.get<std::string>("InputType", "mc") == "data")
		{
			gset.SetJsonFile(g_propTree.get<std::string>("JsonFile"));
			g_inputType = DataInput;
		}
		else
		{
			gset.m_recovertWeight = PropertyTreeSupport::GetAsDoubleList(&g_propTree, "RecovertWeight");
			gset.m_2ndJetWeight = PropertyTreeSupport::GetAsDoubleList(&g_propTree, "2ndJetWeight");
			gset.m_sampleWeight = PropertyTreeSupport::GetAsDoubleList(&g_propTree, "SampleWeight");
			if (gset.GetEnableSampleReweighting() && gset.m_sampleWeight.size() == 0)
				CALIB_LOG_FATAL("Sample reweighting is enabled but no weights given!");

			g_inputType = McInput;
		}

		gset.SetInputType(g_inputType);

		ZJetEventProvider evtProvider(finterface, g_inputType, gset.GetEnableMetPhiCorrection(), g_propTree.get<bool>("Tagged"));
		gset.m_metphi = PropertyTreeSupport::GetAsDoubleList(&g_propTree, "MetPhiCorrectionParameters");



		// pipline settings
		ZJetPipelineInitializer plineInit;
		ZJetPipelineSettings* pset = NULL;

		EventPipelineRunner<ZJetPipeline, ZJetGlobalMetaDataProducerBase> pRunner;

		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
					  g_propTree.get_child("Pipelines"))
		{
			pset = new ZJetPipelineSettings();
			pset->SetPropTree(&g_propTree);

			std::string sKeyName = v.first.data();
			pset->SetSettingsRoot("Pipelines." + sKeyName);
			pset->SetRootOutFile(g_resFile);

			g_pipeSettings.push_back(pset);
		}

		// get pointers to the interesting collections
		typedef std::map<std::string, KDataPFJets*> PfMap;
		PfMap pfJets;

		stringvector globalProducer = PropertyTreeSupport::GetAsStringList(
										  &g_propTree, "GlobalProducer");

		AddGlobalMetaProducer(globalProducer, pRunner, g_propTree);

		for (PipelineSettingsVector::iterator it = g_pipeSettings.begin();
			 it != g_pipeSettings.end(); it++)
		{
			(*it)->m_globalSettings = &gset;

			if ((*it)->GetLevel() == 1)
			{
				ZJetPipeline* pLine = new ZJetPipeline; //CreateDefaultPipeline();

				pLine->InitPipeline(**it, plineInit);
				pRunner.AddPipeline(pLine);
			}
			/*
					if ((*it)->GetLevel() == 2)
					{
						ZJetPipeline* pLine = new ZJetPipeline; //CreateDefaultPipeline();

						pLine->InitPipeline(**it, plineInit);
						pRunner.AddPipeline(pLine);
					}
					*/
		}

		// delete the pipeline settings
		for (PipelineSettingsVector::iterator it = g_pipeSettings.begin();
			 it != g_pipeSettings.end(); it++)
		{
			delete *it;
		}

		// move to config read
		if (gset.GetEnablePuReweighting())
			CALIB_LOG_FILE(blue << "Pile-up reweighting enabled." << reset);
		if (gset.GetEnableSampleReweighting())
			CALIB_LOG_FILE(blue << "Sample reweighting enabled." << reset);
		if (gset.GetEnableLumiReweighting())
			CALIB_LOG_FILE(blue << "Lumi reweighting enabled." << reset);
		if (gset.GetEnable2ndJetReweighting())
			CALIB_LOG_FILE(blue << "2nd jet reweighting enabled." << reset);
		if (gset.GetHcalCorrection())
			CALIB_LOG_FILE(blue << "HCAL correction enabled." << reset);
		if (gset.GetEnableMetPhiCorrection())
			CALIB_LOG_FILE(blue << "MET phi correction enabled." << reset);
		ZJetPipelineSettings settings;
		settings.m_globalSettings = &gset;
		CALIB_LOG_FILE("");

#ifdef USE_PERFTOOLS
		ProfilerStart("artus.prof");
#endif
		//HeapProfilerStart( "resp_cuts.heap");
		nevents = pRunner.RunPipelines<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>(evtProvider, settings);

		//HeapProfilerStop();
#ifdef USE_PERFTOOLS
		ProfilerStop();
#endif
	}
	g_resFile->Close();
	g_logFile->close();

	std::cout << std::endl;
	int nc = 0;
	CALIB_LOG_FILE("Events read: " << nevents << ", events in cut: unknown > " << nc << " (>" << (nc * 100. / nevents) << "%)");
	CALIB_LOG_FILE("Output file " << sRootOutputFilename << " closed.");

	delete g_logFile;

	return 0;
}

