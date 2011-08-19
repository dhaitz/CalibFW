#include "ZJetPipeline.h"
#include "ZJetCuts.h"

#include "ZJetMetaDataProducer.h"
#include "ZJetDrawConsumer.h"

using namespace CalibFW;

void ZJetPipelineInitializer::InitPipeline(ZJetPipeline * pLine, ZJetPipelineSettings const& pset) const
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
			else if ( sid == ValidMuonsFilter().GetFilterId())
				pLine->AddFilter( new ValidMuonsFilter);

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

		fvec = pset.GetCuts();
		BOOST_FOREACH( std::string sid, fvec )
		{		// make this more beatiful :)
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

		//	pLine->AddMetaDataProducer( new	ValidNPVProducer());
		pLine->AddMetaDataProducer( new	ValidMuonProducer());
		pLine->AddMetaDataProducer( new	ZProducer());

	}


	// add consumer
	std::cout << pset.GetSettingsRoot() << std::endl;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
			pset.GetPropTree()->get_child( pset.GetSettingsRoot() + ".Consumer") )
	{
		std::string sName = v.second.get<std::string>("Name");
		std::string consPath = pset.GetSettingsRoot() + ".Consumer." + v.first.data();

		// 1st Leve
		if (sName == BinResponseConsumer::GetName())
			pLine->AddConsumer( new BinResponseConsumer() );

		// 2nd Level
		else if( sName == JetRespConsumer::GetName() )
			pLine->AddConsumer( new JetRespConsumer( pset.GetPropTree(), consPath ) );
		else
			CALIB_LOG_FATAL( "Consumer " << sName << " not found." )
	}

}
