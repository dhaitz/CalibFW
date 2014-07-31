#include "ZJetProducer/MetadataProducer.h"
#include "ZJetProducer/Cuts.h"
#include "ZJetProducer/WeightProducer.h"
#include "ZJetProducer/PileupTruthProducer.h"
#include "ZJetProducer/JetCorrector.h"
#include "ZJetProducer/MuonCorrector.h"
#include "ZJetProducer/JetSorter.h"
#include "ZJetProducer/HltSelector.h"
#include "ZJetProducer/JetMatcher.h"
#include "ZJetProducer/TypeIMETProducer.h"
#include "ZJetProducer/UnclusteredEnergyProducer.h"
#include "ZJetProducer/FlavourProducer.h"
#include "ZJetProducer/LeadingJetUncertaintyProducer.h"
#include "ZJetProducer/ZeeProducer.h"
#include "ZJetProducer/LHEProducer.h"
#include "ZJetProducer/ElectronSFProducer.h"
#include "ZJetProducer/ElectronCorrector.h"


using namespace Artus;


void AddGlobalMetaProducers(std::vector<std::string> const& producer,
							EventPipelineRunner<ZJetPipeline, ZJetGlobalMetaDataProducerBase>& runner,
							boost::property_tree::ptree& globalSettings)
{
	// extend here, if you want to provide a new global meta producer
	for (std::vector<std::string>::const_iterator it = producer.begin();
		 it != producer.end(); ++it)
	{
		std::string mucorr = globalSettings.get<std::string>("MuonCorrectionParameters", "missing");
		if (ValidMuonProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ValidMuonProducer());
		else if (ZProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ZProducer());
		else if (WeightProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new WeightProducer(globalSettings.get<std::string>("PileupWeights")));
		else if (PileupTruthProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new PileupTruthProducer(
											 globalSettings.get<std::string>("PileupTruth"),
											 globalSettings.get<double>("MinBiasXS")));
		else if (MuonCorrector::Name() == *it)
			runner.AddGlobalMetaProducer(new MuonCorrector(
											 mucorr,
											 globalSettings.get<std::string>("MuonCorrectionParametersRunD", mucorr),
											 globalSettings.get<bool>("MuonSmearing", false),
											 globalSettings.get<bool>("MuonRadiationCorrection", false)));
		else if (ValidJetProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ValidJetProducer(
											 globalSettings.get<bool>("Tagged"),
											 globalSettings.get<bool>("VetoPileupJets")));
		else if (ValidJetEEProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ValidJetEEProducer(
											 globalSettings.get<bool>("Tagged"),
											 globalSettings.get<bool>("VetoPileupJets")));
		else if (JetCorrector::Name() == *it)
			runner.AddGlobalMetaProducer(new JetCorrector(
											 globalSettings.get<std::string>("Jec"),
											 globalSettings.get<std::string>("L1Correction"),
											 PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms"),
											 globalSettings.get<bool>("RC")));
		else if (JetSorter::Name() == *it)
			runner.AddGlobalMetaProducer(new JetSorter());
		else if (HltSelector::Name() == *it)
			runner.AddGlobalMetaProducer(new HltSelector(PropertyTreeSupport::GetAsStringList(&globalSettings, "HltPaths", true)));
		else if (JetMatcher::Name() == *it)
			runner.AddGlobalMetaProducer(new JetMatcher(PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));
		else if (GenMetProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new GenMetProducer());
		else if (GenProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new GenProducer());
		else if (GenBalanceProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new GenBalanceProducer());
		else if (GenDibalanceProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new GenDibalanceProducer());
		else if (TypeIMETProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new TypeIMETProducer(
											 globalSettings.get<bool>("EnableMetPhiCorrection"),
											 PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms"),
											 globalSettings.get<bool>("RC")));

		else if (UnclusteredEnergyProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new UnclusteredEnergyProducer(
											 PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));
		else if (FlavourProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new FlavourProducer());
		else if (LeadingJetUncertaintyProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new LeadingJetUncertaintyProducer(
											 globalSettings.get<std::string>("Jec"),
											 PropertyTreeSupport::GetAsStringList(&globalSettings, "GlobalAlgorithms")));
		else if (ZEEProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ZEEProducer());
		else if (ValidElectronProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ValidElectronProducer(
											 globalSettings.get<std::string>("ElectronID"),
											 globalSettings.get<bool>("ExcludeECALGap")));
		else if (ElectronSFProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new ElectronSFProducer(globalSettings.get<std::string>("ScaleFactors"),
										 globalSettings.get<std::string>("ElectronID")));
		else if (LHEProducer::Name() == *it)
			runner.AddGlobalMetaProducer(new LHEProducer());
		else if (ElectronCorrector::Name() == *it)
			runner.AddGlobalMetaProducer(new ElectronCorrector());
		else
			LOG_FATAL("Global MetaData producer of name " << *it << " not found");
	}
}
