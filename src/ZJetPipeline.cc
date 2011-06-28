#include "ZJetPipeline.h"
#include "ZJetCuts.h"

using namespace CalibFW;

void ZJetPipelineInitializer::InitPipeline(ZJetPipeline * pLine, ZJetPipelineSettings * pset)
{
	// load filter from the Settings and add them
	stringvector fvec = pset->GetFilter();
	BOOST_FOREACH( std::string sid, fvec )
	{		// make this more beatiful :)

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
			CALIB_LOG_FATAL( "Filter " << sid << " not found." )
	}

	// enable  various cuts
	pset->SetCutEnabledBitmask( 0 );
	stringvector cuts = pset->GetCuts();
	BOOST_FOREACH( std::string sid, cuts)
	{
		BOOST_FOREACH( ZJetCutBase * pCut, g_ZJetCuts )
		{
			if ( pCut->GetCutShortName() == sid)
			{
				pset->SetCutEnabledBitmask( pset->GetCutEnabledBitmask() | pCut->GetId() );
			}
		}
	}


}
