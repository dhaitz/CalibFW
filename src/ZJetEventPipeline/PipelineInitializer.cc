#include "EventPipeline/JetTools.h"

#include "ZJetEventPipeline/PipelineInitializer.h"

#include "ZJetFilter/Cuts.h"
#include "ZJetProducer/WeightProducer.h"
#include "ZJetProducer/HltSelector.h"

#include "ZJetFilter/ValidZFilter.h"
#include "ZJetFilter/InCutFilter.h"
#include "ZJetFilter/PtWindowFilter.h"
#include "ZJetFilter/JsonFilter.h"
#include "ZJetFilter/HltFilter.h"
#include "ZJetFilter/MetFilter.h"
#include "ZJetFilter/RunRangeFilter.h"
#include "ZJetFilter/NpvFilter.h"
#include "ZJetFilter/JetEtaFilter.h"
#include "ZJetFilter/ResponseFilter.h"
#include "ZJetFilter/DeltaEtaFilter.h"
#include "ZJetFilter/JetPtFilter.h"
#include "ZJetFilter/FlavourFilter.h"

#include "ZJetConsumer/NtupleConsumer.h"
#include "ZJetConsumer/NtupleValidJetConsumer.h"
#include "ZJetConsumer/NtupleValidElectronConsumer.h"
#include "ZJetConsumer/NtupleValidMuonConsumer.h"
#include "ZJetConsumer/NtupleGenParticleConsumer.h"
#include "ZJetConsumer/CutStatistics.h"

using namespace Artus;

//load filter, cuts and consumer from the settings and add them
void ZJetPipelineInitializer::InitPipeline(EventPipeline<ZJetEventData, ZJetProduct, ZJetPipelineSettings>* pLine,
		ZJetPipelineSettings const& pset) const
{
	// Filter
	stringvector list = pset.GetFilter();
	BOOST_FOREACH(std::string id, list)
	{
		if (id == PtWindowFilter().GetFilterId())
			pLine->AddFilter(new PtWindowFilter);
		else if (id == JsonFilter().GetFilterId())
			pLine->AddFilter(new JsonFilter(pset.Global()->GetJsonFile()));
		else if (id == MetFilter().GetFilterId())
			pLine->AddFilter(new MetFilter());
		else if (id == InCutFilter().GetFilterId())
			pLine->AddFilter(new InCutFilter);
		else if (id == ValidZFilter().GetFilterId())
			pLine->AddFilter(new ValidZFilter);
		else if (id == ValidJetFilter().GetFilterId())
			pLine->AddFilter(new ValidJetFilter);
		else if (id == NpvFilter().GetFilterId())
			pLine->AddFilter(new NpvFilter());
		else if (id == JetEtaFilter().GetFilterId())
			pLine->AddFilter(new JetEtaFilter());
		else if (id == HltFilter().GetFilterId())
			pLine->AddFilter(new HltFilter);
		else if (id == RunRangeFilter().GetFilterId())
			pLine->AddFilter(new RunRangeFilter);
		else if (id == ResponseFilter().GetFilterId())
			pLine->AddFilter(new ResponseFilter);
		else if (id == DeltaEtaFilter().GetFilterId())
			pLine->AddFilter(new DeltaEtaFilter);
		else if (id == JetPtFilter().GetFilterId())
			pLine->AddFilter(new JetPtFilter);
		else if (id == FlavourFilter().GetFilterId())
			pLine->AddFilter(new FlavourFilter);
		else
			LOG_FATAL("Filter " << id << " not found.");
	}

	// Cuts
	BOOST_FOREACH(std::string id, pset.GetCuts())
	{
		if (id == MuonEtaCut().GetCutShortName())
			pLine->AddProducer(new MuonEtaCut());
		else if (id == MuonPtCut().GetCutShortName())
			pLine->AddProducer(new MuonPtCut());

		else if (id == ElectronEtaCut().GetCutShortName())
			pLine->AddProducer(new ElectronEtaCut());
		else if (id == ElectronPtCut().GetCutShortName())
			pLine->AddProducer(new ElectronPtCut());

		else if (id == ZemuCuts().GetCutShortName())
			pLine->AddProducer(new ZemuCuts());

		else if (id == ZPtCut().GetCutShortName())
			pLine->AddProducer(new ZPtCut());
		else if (id == ZMassWindowCut().GetCutShortName())
			pLine->AddProducer(new ZMassWindowCut());

		else if (id == LeadingJetPtCut().GetCutShortName())
			pLine->AddProducer(new LeadingJetPtCut());
		else if (id == LeadingJetEtaCut().GetCutShortName())
			pLine->AddProducer(new LeadingJetEtaCut());

		else if (id == BackToBackCut().GetCutShortName())
			pLine->AddProducer(new BackToBackCut());
		else if (id == SecondLeadingToZPtCut().GetCutShortName())
			pLine->AddProducer(new SecondLeadingToZPtCut());
		else if (id == SecondLeadingToZPtRegionCut().GetCutShortName())
			pLine->AddProducer(new SecondLeadingToZPtRegionCut());

		// VBF
		else if (id == SecondJetPtCut().GetCutShortName())
			pLine->AddProducer(new SecondJetPtCut());
		else if (id == SecondJetEtaCut().GetCutShortName())
			pLine->AddProducer(new SecondJetEtaCut());
		else if (id == RapidityGapCut().GetCutShortName())
			pLine->AddProducer(new RapidityGapCut());
		else if (id == InvariantMassCut().GetCutShortName())
			pLine->AddProducer(new InvariantMassCut());
		else if (id == DeltaEtaCut().GetCutShortName())
			pLine->AddProducer(new DeltaEtaCut());
		else
			LOG_FATAL("Cut " << id << " not found.");
	}

	// Consumer
	BOOST_FOREACH(std::string id, pset.GetConsumer())
	{
		if (id == CutStatisticsConsumer().GetName())
			pLine->AddConsumer(new CutStatisticsConsumer());
		else if (id == NtupleConsumer().GetName())
			pLine->AddConsumer(new NtupleConsumer());
		else if (id == NtupleValidJetConsumer().GetName())
			pLine->AddConsumer(new NtupleValidJetConsumer());
		else if (id == NtupleValidElectronConsumer().GetName())
			pLine->AddConsumer(new NtupleValidElectronConsumer());
		else if (id == NtupleValidMuonConsumer().GetName())
			pLine->AddConsumer(new NtupleValidMuonConsumer());
		else if (id == NtupleGenParticleConsumer().GetName())
			pLine->AddConsumer(new NtupleGenParticleConsumer());
		else
			LOG_FATAL("Consumer " << id << " not found.");
	}

}



