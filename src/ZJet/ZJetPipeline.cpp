#include "ZJet/ZJetPipeline.h"

#include "ZJet/MetaDataProducer/ZJetCuts.h"
#include "ZJet/MetaDataProducer/PuReweightingProducer.h"
#include "ZJet/MetaDataProducer/ZJetMetaDataProducer.h"

#include "ZJet/Consumer/ZJetDrawConsumer.h"
#include "ZJet/Consumer/CutStatistics.h"
#include "ZJet/Consumer/FilterStatistics.h"
#include "ZJet/Consumer/GenericProfileConsumer.h"
#include "ZJet/Consumer/JetRespConsumer.h"


#include "ZJet/Filter/ValidZFilter.h"
#include "ZJet/Filter/InCutFilter.h"
#include "ZJet/Filter/PtWindowFilter.h"
#include "ZJet/Filter/JsonFilter.h"

using namespace CalibFW;

void ZJetPipeline::GetSupportedCuts(ZJetPipeline::MetaDataProducerVector & cuts)
{
	cuts.clear();
	cuts.push_back( new MuonEtaCut() );
	cuts.push_back( new MuonPtCut() );

	cuts.push_back( new ZMassWindowCut() );

	cuts.push_back( new LeadingJetEtaCut() );

	cuts.push_back( new SecondLeadingToZPtCut() );
	cuts.push_back( new BackToBackCut() );
	//cuts.push_back( new ZPtCut() );


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
			else if ( sid == JsonFilter().GetFilterId())
				pLine->AddFilter( new JsonFilter( pset.Global()->GetJsonFile()));
			else if ( sid == InCutFilter().GetFilterId())
				pLine->AddFilter( new InCutFilter);
			else if ( sid == ValidZFilter().GetFilterId())
				pLine->AddFilter( new ValidZFilter);
			else if ( sid == ValidJetFilter().GetFilterId())
				pLine->AddFilter( new ValidJetFilter);
			else
				CALIB_LOG_FATAL( "Filter " << sid << " not found." )
		}


		//pLine->AddMetaDataProducer( new	ValidMuonProducer());


		// Cuts
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

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
			pset.GetPropTree()->get_child( pset.GetSettingsRoot() + ".Consumer") )
	{
		std::string sName = v.second.get<std::string>("Name");
		std::string consPath = pset.GetSettingsRoot() + ".Consumer." + v.first.data();

		if (sName == "quantities_all" )
		{
			pLine->AddConsumer( new DataZConsumer( pset.GetJetAlgorithm() ));

			pLine->AddConsumer( new DataMuonConsumer(+1, pset.GetJetAlgorithm()));
			pLine->AddConsumer( new DataMuonConsumer(-1, pset.GetJetAlgorithm()));

			pLine->AddConsumer( new ValidMuonsConsumer());
			pLine->AddConsumer( new ValidJetsConsumer());

			if ( JetType::IsPF( pset.GetJetAlgorithm() ))
			{
				pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(), 0));
				pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(), 1));
				pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(), 2));
			}

			if ( pset.IsMC() && JetType::IsGen( pset.GetJetAlgorithm() ) )
			{
				// add gen jets plots
				pLine->AddConsumer( new DataGenJetConsumer( pset.GetJetAlgorithm(), 0,
															pset.GetJetAlgorithm() ));
				pLine->AddConsumer( new DataGenJetConsumer( pset.GetJetAlgorithm(), 1,
															pset.GetJetAlgorithm() ));
				pLine->AddConsumer( new DataGenJetConsumer( pset.GetJetAlgorithm(), 2,
															pset.GetJetAlgorithm() ));

			}

			if (  pset.IsMC() )
			{
				pLine->AddConsumer( new GenMetadataConsumer( ) );
			}
			else
			{
				pLine->AddConsumer( new MetadataConsumer( ) );
			}
		}

		else if (sName == "quantities_basic" )
		{
			pLine->AddConsumer( new DataZConsumer( pset.GetJetAlgorithm() ));

			if ( JetType::IsPF( pset.GetJetAlgorithm() ))
			{
				pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(),
						0,
						"", // means to use the default jet algo
						true ));
			}

			if ( pset.IsMC() && JetType::IsGen( pset.GetJetAlgorithm() ) )
			{
				// add gen jets plots
				pLine->AddConsumer( new DataGenJetConsumer( pset.GetJetAlgorithm(), 0,
															pset.GetJetAlgorithm() ));
			}
		}


		// optional 1st Level Producer
		else if (sName == BinResponseConsumer::GetName())
			pLine->AddConsumer( new BinResponseConsumer( pset.GetPropTree(), consPath ) );

		else if (sName == GenericProfileConsumer::GetName())
			pLine->AddConsumer( new GenericProfileConsumer( pset.GetPropTree(), consPath ) );

		else if (sName == CutStatisticsConsumer::GetName())
			pLine->AddConsumer( new CutStatisticsConsumer());

		else if (sName == FilterStatisticsConsumer::GetName())
			pLine->AddConsumer( new FilterStatisticsConsumer());

		// 2nd Level Producer
		else if( sName == JetRespConsumer::GetName() )
			pLine->AddConsumer( new JetRespConsumer( pset.GetPropTree(), consPath ) );
		else
			CALIB_LOG_FATAL( "Consumer " << sName << " not found." )
	}

}
