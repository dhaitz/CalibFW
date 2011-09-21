#include "ZJet/ZJetPipeline.h"

#include "ZJet/MetaDataProducer/ZJetCuts.h"
#include "ZJet/MetaDataProducer/PuReweightingProducer.h"
#include "ZJet/MetaDataProducer/ZJetMetaDataProducer.h"

#include "ZJet/Consumer/ZJetDrawConsumer.h"
#include "ZJet/Consumer/CutStatistics.h"
#include "ZJet/Consumer/GenericProfileConsumer.h"
#include "ZJet/Consumer/JetRespConsumer.h"

#include "ZJet/Filter/ValidZFilter.h"
#include "ZJet/Filter/InCutFilter.h"
#include "ZJet/Filter/PtWindowFilter.h"


using namespace CalibFW;

ZJetPipeline::MetaDataProducerVector ZJetPipeline::GetSupportedCuts()
{
	/// TODO: this pointer must be released at some point !! -> memleak cries !

	ZJetPipeline::MetaDataProducerVector cuts;



	cuts.push_back( new MuonEtaCut() );
	cuts.push_back( new MuonPtCut() );

	cuts.push_back( new ZMassWindowCut() );

	cuts.push_back( new LeadingJetEtaCut() );

	cuts.push_back( new SecondLeadingToZPtCut() );
	cuts.push_back( new BackToBackCut() );
	//cuts.push_back( new ZPtCut() );

	return cuts;
}

void ZJetPipelineInitializer::InitPipeline(EventPipeline<ZJetEventData, ZJetMetaData, ZJetPipelineSettings> * pLine,
		ZJetPipelineSettings const& pset) const
{

	if ( pset.GetLevel() == 1 )
	{
		// load filter from the Settings and add them
		stringvector fvec = pset.GetFilter();
		BOOST_FOREACH( std::string sid, fvec )
		{		// make this more beatiful :)

			if ( sid == PtWindowFilter().GetFilterId())
				pLine->AddFilter( new PtWindowFilter);
			else if ( sid == InCutFilter().GetFilterId())
				pLine->AddFilter( new InCutFilter);
			else if ( sid == ValidZFilter().GetFilterId())
				pLine->AddFilter( new ValidZFilter);
			else if ( sid == ValidJetFilter().GetFilterId())
				pLine->AddFilter( new ValidJetFilter);
			else
				CALIB_LOG_FATAL( "Filter " << sid << " not found." )

				/*
		if ( sid == CutSelectionFilter().GetFilterId())
			pLine->AddFilter( new CutSelectionFilter);
		else if ( sid == PtWindowFilter().GetFilterId())
			pLine->AddFilter( new PtWindowFilter);
		else if ( sid == InCutFilter().GetFilterId())
			pLine->AddFilter( new InCutFilter);
		else if ( sid == RecoVertFilter().GetFilterId())
			pLine->AddFilter( new RecoVertFilter);
		else if ( sid == JetEtaFilter().GetFilterId())
			pLine->AddFilter( new JetEtaFilter);
		else if ( sid == SecondJetRatioFilter().GetFilterId())
			pLine->AddFilter( new SecondJetRatioFilter);
		else
			CALIB_LOG_FATAL( "Filter " << sid << " not found." )*/
		}


		//pLine->AddMetaDataProducer( new	ValidMuonProducer());



		fvec = pset.GetCuts();
		BOOST_FOREACH( std::string sid, fvec )
		{		// make this more beautiful :)
			if ( sid ==  LeadingJetEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer( new LeadingJetEtaCut() );

			else if ( sid == SecondLeadingToZPtCut().GetCutShortName())
				pLine->AddMetaDataProducer( new SecondLeadingToZPtCut() );

			else if ( sid == MuonPtCut().GetCutShortName())
				pLine->AddMetaDataProducer( new MuonPtCut() );

			else if ( sid == MuonEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer( new MuonEtaCut() );

			else if ( sid == ZMassWindowCut().GetCutShortName())
				pLine->AddMetaDataProducer( new ZMassWindowCut() );

			else if ( sid == BackToBackCut().GetCutShortName())
				pLine->AddMetaDataProducer( new BackToBackCut() );

			else if ( sid == ZPtCut().GetCutShortName())
				pLine->AddMetaDataProducer( new ZPtCut() );

			else
				CALIB_LOG_FATAL( "MetaDataProducer " << sid << " not found." )
		}
	}

	// add consumer
	std::cout << pset.GetSettingsRoot() << std::endl;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
			pset.GetPropTree()->get_child( pset.GetSettingsRoot() + ".Consumer") )
	{
		std::string sName = v.second.get<std::string>("Name");
		std::string consPath = pset.GetSettingsRoot() + ".Consumer." + v.first.data();

		// 1st Level
		if (sName == BinResponseConsumer::GetName())
			pLine->AddConsumer( new BinResponseConsumer( pset.GetPropTree(), consPath ) );

		else if (sName == GenericProfileConsumer::GetName())
			pLine->AddConsumer( new GenericProfileConsumer( pset.GetPropTree(), consPath ) );

		else if (sName == CutStatisticsConsumer::GetName())
			pLine->AddConsumer( new CutStatisticsConsumer());

		else if (sName == FilterStatisticsConsumer::GetName())
			pLine->AddConsumer( new FilterStatisticsConsumer());


		// 2nd Level
		else if( sName == JetRespConsumer::GetName() )
			pLine->AddConsumer( new JetRespConsumer( pset.GetPropTree(), consPath ) );
		else
			CALIB_LOG_FATAL( "Consumer " << sName << " not found." )
	}

}
