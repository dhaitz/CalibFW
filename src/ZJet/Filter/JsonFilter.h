#pragma once

#include <memory>

#include "RootTools/RunLumiReader.h"

#include "ZJetFilterBase.h"

namespace CalibFW
{

class JsonFilter: public ZJetFilterBase
{

public:

	JsonFilter()
	{

	}

	JsonFilter( std::string jsonFileName)
	{
		m_lumiSelector = std::unique_ptr<RunLumiSelector> (
				new RunLumiSelector( jsonFileName)
				);
	}

	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		KEventMetadata * p =  metaData.GetKappaMetaData( event, settings );

		return m_lumiSelector->accept( p->nRun, p->nLumi );
	}

	virtual std::string GetFilterId()
	{
		return "json";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Json Filter";
	}
private:

	std::unique_ptr<RunLumiSelector> m_lumiSelector;
};

}
