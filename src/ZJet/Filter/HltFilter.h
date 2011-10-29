#pragma once

#include <memory>

#include "RootTools/RunLumiReader.h"

#include "ZJetFilterBase.h"
#include "KappaTools/RootTools/HLTTools.h"

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
		HLTTools ht ( event.m_lumimetadata );

		bool bPathFound = false;
		std::string curName;

		for ( std::vector< std::string >::iterator it = m_hltnames.begin();
				it != m_hltnames.end();
				++ it )
		{
			curName = ht.getHLTName( *it );

			if ( ht.isAvailable(curName) )
			{
				if (! ht.isPrescaled(curName))
					bPathFound = true;
			}
		}

		if ( ! bPathFound )
		{
			CALIB_LOG_FATAL("No unprescaled trigger found for Run " << event.m_eventmetadata->nRun << " Lumisection " << event.m_eventmetadata->nLumi );
		}

		std::cout << "using trigger " << curName << std::endl;
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
