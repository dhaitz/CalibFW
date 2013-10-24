#include "EventPipeline/JetTools.h"

#include "ZJetEventPipeline/PipelineInitializer.h"

#include "ZJetProducer/Cuts.h"
#include "ZJetProducer/WeightProducer.h"
#include "ZJetProducer/MetadataProducer.h"
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

#include "ZJetConsumer/TreeConsumer.h"

using namespace Artus;

void ZJetPipelineInitializer::InitPipeline(EventPipeline<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>* pLine,
		ZJetPipelineSettings const& pset) const
{
	if (pset.GetLevel() == 1)
	{
		// load filter from the Settings and add them
		stringvector fvec = pset.GetFilter();
		BOOST_FOREACH(std::string sid, fvec)
		{
			// make this more beatiful :)
			if (sid == PtWindowFilter().GetFilterId())
				pLine->AddFilter(new PtWindowFilter);
			else if (sid == JsonFilter().GetFilterId())
				pLine->AddFilter(new JsonFilter(pset.Global()->GetJsonFile()));
			else if (sid == MetFilter().GetFilterId())
				pLine->AddFilter(new MetFilter());
			else if (sid == InCutFilter().GetFilterId())
				pLine->AddFilter(new InCutFilter);
			else if (sid == ValidZFilter().GetFilterId())
				pLine->AddFilter(new ValidZFilter);
			else if (sid == ValidJetFilter().GetFilterId())
				pLine->AddFilter(new ValidJetFilter);
			else if (sid == NpvFilter().GetFilterId())
				pLine->AddFilter(new NpvFilter());
			else if (sid == JetEtaFilter().GetFilterId())
				pLine->AddFilter(new JetEtaFilter());
			else if (sid == HltFilter().GetFilterId())
				pLine->AddFilter(new HltFilter);
			else if (sid == RunRangeFilter().GetFilterId())
				pLine->AddFilter(new RunRangeFilter);
			else if (sid == ResponseFilter().GetFilterId())
				pLine->AddFilter(new ResponseFilter);
			else if (sid == DeltaEtaFilter().GetFilterId())
				pLine->AddFilter(new DeltaEtaFilter);
			else if (sid == JetPtFilter().GetFilterId())
				pLine->AddFilter(new JetPtFilter);
			else if (sid == FlavourFilter().GetFilterId())
				pLine->AddFilter(new FlavourFilter);
			else
				CALIB_LOG_FATAL("Filter " << sid << " not found.");
		}

		// Cuts
		fvec = pset.GetCuts();
		BOOST_FOREACH(std::string sid, fvec)
		{
			// make this more beautiful :)
			if (sid == MuonEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer(new MuonEtaCut());
			else if (sid == MuonPtCut().GetCutShortName())
				pLine->AddMetaDataProducer(new MuonPtCut());

			else if (sid == ZPtCut().GetCutShortName())
				pLine->AddMetaDataProducer(new ZPtCut());
			else if (sid == ZMassWindowCut().GetCutShortName())
				pLine->AddMetaDataProducer(new ZMassWindowCut());

			else if (sid == LeadingJetPtCut().GetCutShortName())
				pLine->AddMetaDataProducer(new LeadingJetPtCut());
			else if (sid == LeadingJetEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer(new LeadingJetEtaCut());

			else if (sid == BackToBackCut().GetCutShortName())
				pLine->AddMetaDataProducer(new BackToBackCut());
			else if (sid == SecondLeadingToZPtCut().GetCutShortName())
				pLine->AddMetaDataProducer(new SecondLeadingToZPtCut());
			else if (sid == SecondLeadingToZPtRegionCut().GetCutShortName())
				pLine->AddMetaDataProducer(new SecondLeadingToZPtRegionCut());

			// VBF
			else if (sid == SecondJetPtCut().GetCutShortName())
				pLine->AddMetaDataProducer(new SecondJetPtCut());
			else if (sid == SecondJetEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer(new SecondJetEtaCut());
			else if (sid == RapidityGapCut().GetCutShortName())
				pLine->AddMetaDataProducer(new RapidityGapCut());
			else if (sid == InvariantMassCut().GetCutShortName())
				pLine->AddMetaDataProducer(new InvariantMassCut());
			else if (sid == DeltaEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer(new DeltaEtaCut());
			else
				CALIB_LOG_FATAL("Cut " << sid << " not found.");
		}
	}

	BOOST_FOREACH(boost::property_tree::ptree::value_type& v,
			pset.GetPropTree()->get_child(pset.GetSettingsRoot() + ".Consumer"))
	{
		std::string sName = v.second.get<std::string>("Name");
		std::string consPath = pset.GetSettingsRoot() + ".Consumer." + v.first.data();


		if (sName == "tree")
			pLine->AddConsumer(new TreeConsumer());
		else
			CALIB_LOG_FATAL("Consumer " << sName << " not found.");
	}

}



