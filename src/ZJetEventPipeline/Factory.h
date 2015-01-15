#include "ZJetProducer/GenProducer.h"
#include "ZJetProducer/ZProducer.h"
#include "ZJetProducer/ValidMuonProducer.h"
#include "ZJetProducer/ValidJetProducer.h"
#include "ZJetFilter/Cuts.h"
#include "ZJetProducer/WeightProducer.h"
#include "ZJetProducer/PileupTruthProducer.h"
#include "ZJetProducer/JetCorrector.h"
#include "ZJetProducer/MuonCorrector.h"
#include "ZJetProducer/JetSorter.h"
#include "ZJetProducer/HltSelector.h"
#include "ZJetProducer/JetMatcher.h"
#include "ZJetProducer/TypeIMETProducer.h"
#include "ZJetProducer/FlavourProducer.h"
#include "ZJetProducer/LeadingJetUncertaintyProducer.h"
#include "ZJetProducer/ZeeProducer.h"
#include "ZJetProducer/ValidElectronProducer.h"
#include "ZJetProducer/LHEProducer.h"
#include "ZJetProducer/ElectronSFProducer.h"
#include "ZJetProducer/ElectronCorrector.h"
#include "ZJetProducer/L5Producer.h"


using namespace Artus;


void AddGlobalMetaProducers(std::vector<std::string> const& producer,
							EventPipelineRunner<ZJetPipeline, ZJetGlobalProductProducerBase>& runner,
							boost::property_tree::ptree& globalSettings)
{
	// extend here, if you want to provide a new global meta producer
	for (std::vector<std::string>::const_iterator it = producer.begin();
		 it != producer.end(); ++it)
	{
		std::string mucorr = globalSettings.get<std::string>("MuonCorrectionParameters", "missing");
		if (ValidMuonProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ValidMuonProducer());
		else if (ZProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ZProducer());
		else if (WeightProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new WeightProducer(globalSettings.get<std::string>("PileupWeights")));
		else if (PileupTruthProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new PileupTruthProducer(
											 globalSettings.get<std::string>("PileupTruth"),
											 globalSettings.get<double>("MinBiasXS")));
		else if (MuonCorrector::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new MuonCorrector(
											 mucorr,
											 globalSettings.get<std::string>("MuonCorrectionParametersRunD", mucorr),
											 globalSettings.get<bool>("MuonSmearing", false),
											 globalSettings.get<bool>("MuonRadiationCorrection", false)));
		else if (ValidJetProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ValidJetProducer(
											 globalSettings.get<bool>("Tagged"),
											 globalSettings.get<bool>("VetoPileupJets")));
		else if (ValidJetEEProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ValidJetEEProducer(
											 globalSettings.get<bool>("Tagged"),
											 globalSettings.get<bool>("VetoPileupJets")));
		else if (JetCorrector::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new JetCorrector(
											 globalSettings.get<std::string>("Jec"),
											 globalSettings.get<std::string>("L1Correction"),
											 PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms"),
											 globalSettings.get<bool>("RC"),
											 globalSettings.get<bool>("FlavourCorrections")));
		else if (JetSorter::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new JetSorter());
		else if (HltSelector::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new HltSelector(PropertyTreeSupport::GetAsStringList(&globalSettings, "HltPaths", true)));
		else if (JetMatcher::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new JetMatcher(
											 PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms"),
											 globalSettings.get<bool>("FlavourCorrections")));
		else if (GenMetProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new GenMetProducer());
		else if (GenProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new GenProducer());
		else if (GenBalanceProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new GenBalanceProducer());
		else if (GenDibalanceProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new GenDibalanceProducer());
		else if (TypeIMETProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new TypeIMETProducer(
											 globalSettings.get<bool>("EnableMetPhiCorrection"),
											 PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms"),
											 globalSettings.get<bool>("RC"),
											 globalSettings.get<std::string>("InputType") == "data",
											 globalSettings.get<bool>("FlavourCorrections")));
		else if (FlavourProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new FlavourProducer());
		else if (LeadingJetUncertaintyProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new LeadingJetUncertaintyProducer(
											 globalSettings.get<std::string>("Jec"),
											 PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));
		else if (ZEEProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ZEEProducer());
		else if (ZEMuProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ZEMuProducer());
		else if (ValidElectronProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ValidElectronProducer(
											 globalSettings.get<std::string>("ElectronID"),
											 globalSettings.get<bool>("ExcludeECALGap")));
		else if (ElectronSFProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ElectronSFProducer(globalSettings.get<std::string>("ScaleFactors"),
										 globalSettings.get<std::string>("ElectronID")));
		else if (LHEProducer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new LHEProducer());
		else if (ElectronCorrector::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new ElectronCorrector());
		else if (L5Producer::Name() == *it)
			runner.AddGlobalMetaProducer(*it, new L5Producer(PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));
		else
			LOG_FATAL("Global product producer of name " << *it << " not found");
	}
}
