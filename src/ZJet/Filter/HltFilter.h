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
	    // no hlt found
	    if ( metaData.GetSelectedHlt() == "" )
            return false;

		// TODO: Report that we changed the HLT, if we did
		//std::cout << "using trigger " << curName << std::endl;
		return event.m_eventmetadata->hltFired( metaData.GetSelectedHlt(), event.m_lumimetadata );
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
