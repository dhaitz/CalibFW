#pragma once

#include <memory>

#include "RootTools/RunLumiReader.h"

#include "ZJetFilterBase.h"


namespace CalibFW
{


class HltFilter: public ZJetFilterBase
{

public:

	HltFilter()
	{

	}

	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		// move this to a MetaDataProducer
		bool bPathFound = false;
		std::string curName;

		for ( std::vector< std::string >::iterator it = m_hltnames.begin();
				it != m_hltnames.end();
				++ it )
		{
			curName = metaData.m_hltInfo->getHLTName( *it );

			if ( metaData.m_hltInfo->isAvailable(curName) )
			{
				if (! metaData.m_hltInfo->isPrescaled(curName))
					bPathFound = true;
			}
		}

		if ( ! bPathFound )
		{
			CALIB_LOG_FATAL("No unprescaled trigger found for Run " << event.m_eventmetadata->nRun << " Lumisection " << event.m_eventmetadata->nLumi );
		}
		// TODO: Report that we changed the HLT, if we did
		//std::cout << "using trigger " << curName << std::endl;
		return event.m_eventmetadata->hltFired( curName, event.m_lumimetadata );

	}

	virtual std::string GetFilterId()
	{
		return "hlt";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Hlt Filter";
	}
private:
	std::vector< std::string > m_hltnames;
	//std::unique_ptr<RunLumiSelector> m_lumiSelector;
};

}
