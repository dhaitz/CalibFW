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

#include "EventPipeline/JetTools.h"
#include "EventPipeline/EventPipelineRunner.h"
#include "ZJetEventPipeline/Pipeline.h"
#include "ZJetEventPipeline/EventProvider.h"
#include "ZJetEventPipeline/PipelineInitializer.h"
#include "ZJetEventPipeline/Factory.h"

#include "KappaTools/RootTools/HLTTools.h"

using namespace Artus;

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


int main(int argc, char** argv)
{
	long long nevents = 0;

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
	LOG_FILE("Configuration file: " << jsonConfig);
	boost::property_tree::json_parser::write_json(*g_logFile, g_propTree);


	// The input are read from the shell variable FILE_NAMES, which is set by grid control.
	// In case this variable is not set (for normal, local use), the list of
	// files is taken from the settings
	char* pPath;
	pPath = getenv("FILE_NAMES");
	if (pPath != NULL)
	{
		boost::split(g_sourcefiles, pPath, boost::is_any_of(" "), boost::token_compress_on);
	}
	else
	{
		g_sourcefiles = PropertyTreeSupport::GetAsStringList(&g_propTree, "InputFiles");
		if (g_sourcefiles.size() == 0)
		{
			LOG_FATAL("No Kappa input files specified.");
		}
		else
		{
			LOG_FILE("Input files (" << g_sourcefiles.size() << "):");
		}
	}

	std::string outputFilename = g_sOutputPath + ".root";
	// The curly brackets here are necessary to prevent the final status line to be printed twice
	{
		FileInterface2 finterface(g_sourcefiles);
		LOG_FILE("Output file: " << outputFilename);

		// setup Global Settings
		ZJetGlobalSettings gset;

		gset.SetEnablePuReweighting(g_propTree.get<bool>("EnablePuReweighting", false));
		gset.SetPileupWeights(g_propTree.get<std::string>("PileupWeights", "not_found"));
		gset.SetEnableTriggerReweighting(g_propTree.get<bool>("EnableTriggerReweighting", false));
		gset.SetEnable2ndJetReweighting(g_propTree.get<bool>("Enable2ndJetReweighting", false));
		gset.SetEnableSampleReweighting(g_propTree.get<bool>("EnableSampleReweighting", false));
		gset.SetEnableLumiReweighting(g_propTree.get<bool>("EnableLumiReweighting", false));
		gset.SetXSection(g_propTree.get<double>("XSection", -1));
		gset.SetEnableMetPhiCorrection(g_propTree.get<bool>("EnableMetPhiCorrection", false));
		gset.SetMuonID2011(g_propTree.get<bool>("MuonID2011", false));
		gset.SetHcalCorrection(g_propTree.get<double>("HcalCorrection", 0.0));
		gset.SetVetoPileupJets(g_propTree.get<bool>("VetoPileupJets", false));
		gset.SetNEvents(g_propTree.get<long long>("NEvents", -1));
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
				LOG_FATAL("Sample reweighting is enabled but no weights given!");
			if (gset.GetEnableLumiReweighting() && gset.GetNEvents() < 0)
				LOG_FATAL("Lumi reweighting is enabled but number of events missing!");

			g_inputType = McInput;
		}

		gset.SetInputType(g_inputType);

		ZJetEventProvider evtProvider(finterface, g_inputType, g_propTree);
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

			std::string pipelineName = v.first.data();
			pset->SetName(pipelineName);
			pset->SetRootOutFile(g_resFile);

			g_pipeSettings.push_back(pset);
		}

		// get pointers to the interesting collections
		typedef std::map<std::string, KDataPFJets*> PfMap;
		PfMap pfJets;

		stringvector globalProducer = PropertyTreeSupport::GetAsStringList(
										  &g_propTree, "GlobalProducer");

		AddGlobalMetaProducers(globalProducer, pRunner, g_propTree);

		for (PipelineSettingsVector::iterator it = g_pipeSettings.begin();
			 it != g_pipeSettings.end(); it++)
		{
			(*it)->m_globalSettings = &gset;
			ZJetPipeline* pLine = new ZJetPipeline;
			pLine->InitPipeline(**it, plineInit);
			pRunner.AddPipeline(pLine);
		}

		// delete the pipeline settings
		for (PipelineSettingsVector::iterator it = g_pipeSettings.begin();
			 it != g_pipeSettings.end(); it++)
		{
			delete *it;
		}

		// move to config read function
		if (gset.GetEnablePuReweighting())
			LOG_FILE(blue << "Pile-up reweighting enabled." << reset);
		if (gset.GetEnableTriggerReweighting())
			LOG_FILE(blue << "Trigger reweighting enabled." << reset);
		if (gset.GetEnableSampleReweighting())
			LOG_FILE(blue << "Sample reweighting enabled." << reset);
		if (gset.GetEnableLumiReweighting())
			LOG_FILE(blue << "Lumi reweighting enabled." << reset);
		if (gset.GetEnable2ndJetReweighting())
			LOG_FILE(blue << "2nd jet reweighting enabled." << reset);
		if (gset.GetHcalCorrection())
			LOG_FILE(blue << "HCAL correction enabled." << reset);
		if (gset.GetEnableMetPhiCorrection())
			LOG_FILE(blue << "MET phi correction enabled." << reset);
		if (g_propTree.get<bool>("MuonRadiationCorrection", false))
			LOG_FILE(red << "Shift muon scale by 0.5% - this should not be used!" << reset);
		ZJetPipelineSettings settings;
		settings.m_globalSettings = &gset;
		LOG_FILE("");

		// removes the old file
		g_resFile = new TFile(outputFilename.c_str(), "RECREATE");

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

	// Write some stuff from the config into the root file
	// TODO if we need this more often, turn into dedicated function
	TObjString type(g_propTree.get<std::string> ("InputType").c_str());
	TObjString channel(g_propTree.get<std::string> ("Channel").c_str());
	TObjString year(g_propTree.get<std::string> ("Run").c_str());
	g_resFile->cd();
	type.Write("Type");
	channel.Write("Channel");
	year.Write("Year");


	g_resFile->Close();
	g_logFile->close();

	std::cout << std::endl;
	// TODO: determine nc
	// int nc = 0;
	LOG_FILE("Events read: " << nevents); // << ", events in cut: unknown > " << nc << " (>" << (nc * 100. / nevents) << "%)");
	LOG_FILE("Output file " << outputFilename << " closed.");
	std::cout.flush();
	delete g_logFile;

	return 0;
}

