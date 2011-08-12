#include "ZJetPipeline.h"
#include "ZJetCuts.h"

using namespace CalibFW;

void ZJetPipelineInitializer::InitPipeline(ZJetPipeline * pLine, ZJetPipelineSettings const& pset) const
{

	// load filter from the Settings and add them
	stringvector fvec = pset.GetFilter();
	BOOST_FOREACH( std::string sid, fvec )
	{		// make this more beatiful :)

		if ( sid == PtWindowFilter().GetFilterId())
			pLine->AddFilter( new PtWindowFilter);
		else if ( sid == InCutFilter().GetFilterId())
			pLine->AddFilter( new InCutFilter);
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

		else
			CALIB_LOG_FATAL( "MetaDataProducer " << sid << " not found." )
	}

	// enable  various cuts
	//pset.SetCutEnabledBitmask( 0 );
	/*
	stringvector cuts = pset.GetCuts();
	BOOST_FOREACH( std::string sid, cuts)
	{
		BOOST_FOREACH( ZJetCutBase * pCut, g_ZJetCuts )
		{
			if ( pCut.GetCutShortName() == sid)
			{
				pset.SetCutEnabledBitmask( pset.GetCutEnabledBitmask() | pCut.GetId() );
			}
		}
	}

*/
}
