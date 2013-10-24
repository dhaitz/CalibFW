#pragma once

#include <boost/scoped_ptr.hpp>

#include "KappaTools/RootTools/RunLumiReader.h"

#include "FilterBase.h"

namespace Artus
{

class JsonFilter: public ZJetFilterBase
{
public:
	JsonFilter() {}

	JsonFilter(std::string jsonFileName)
	{
		m_lumiSelector.reset(new RunLumiSelector(jsonFileName));
	}

	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetMetaData const& metaData,
							   ZJetPipelineSettings const& settings)
	{
		KEventMetadata* p =  metaData.GetKappaMetaData(event, settings);

		return m_lumiSelector->accept(p->nRun, p->nLumi);
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
	boost::scoped_ptr<RunLumiSelector> m_lumiSelector;
};

}
