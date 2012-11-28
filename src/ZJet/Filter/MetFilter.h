#pragma once

#include <memory>

#include "RootTools/RunLumiReader.h"
#include "ZJetFilterBase.h"


namespace CalibFW
{

class MetFilter: public ZJetFilterBase
{

public:
	MetFilter(): m_useFilters(false) {}

	MetFilter(std::vector<std::string> requiredFilters):
			m_filters(requiredFilters), m_useFilters(true)
	{
		m_useFilters = !(m_filters.size() == 0 || m_filters.size() > 0 &&
			(m_filters[0] == "" || m_filters[0] == "all"));
	}

	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		if (m_useFilters)
			return event.m_filter->passedFilters(m_filters, event.m_filtermetadata);
		else
			return event.m_filter->passedFilters();
	}

	virtual std::string GetFilterId() { return "metfilter"; }
	virtual std::string ToString(bool bVerbose = false) { return "MET Filter"; }

private:
	std::vector<std::string> m_filters;
	bool m_useFilters;
};

}
