#pragma once

#include "ZJetProducer/MetadataProducer.h"

namespace Artus
{

class HltSelector: public ZJetGlobalMetaDataProducerBase
{
public:

	HltSelector(stringvector paths) : m_hltpaths(paths), m_verbose(false)
	{
		m_bestHltName.clear();
	}

	virtual void PopulateMetaData(ZJetEventData const& data,
								  ZJetMetaData& metaData,
								  ZJetPipelineSettings const& m_pipelineSettings) const { }

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		if (m_verbose)
		{
			for (const auto & it : data.GetDataLumiMetadata()->hltNames)
			{
				LOG(it);
			}
		}

		// first, it is checked whether the m_bestHltName (set in a previous event)
		// is also available and unprescaled for the current event
		if (!m_bestHltName.empty()
			&& metaData.m_hltInfo->isAvailable(m_bestHltName)
			&& ! metaData.m_hltInfo->isPrescaled(m_bestHltName))
		{
			metaData.SetSelectedHlt(m_bestHltName);
			return true;
		}


		bool unprescaledPathFound = false;
		std::string curName;

		if (m_hltpaths.size() == 0)
			LOG_FATAL("No Hlt Trigger path list configured");

		// second, if m_bestHltName cant be used, the entire list of trigger paths
		// is checked for availability and prescale
		for (const auto & it : m_hltpaths)
		{
			curName = metaData.m_hltInfo->getHLTName(it);

			if (m_verbose)
				std::cout << it << " becomes " << curName << std::endl;

			if (!curName.empty())
			{
				m_bestHltName = curName;
				if (!metaData.m_hltInfo->isPrescaled(curName))
				{
					unprescaledPathFound = true;
					break;
				}
			}
		}

		if (!unprescaledPathFound)
		{
			LOG("Available Triggers:")

			for (const auto & it : data.GetDataLumiMetadata()->hltNames)
			{
				LOG(it << " prescale: " << metaData.m_hltInfo->getPrescale(it));
			}

			LOG_FATAL("No unprescaled trigger found for " << m_bestHltName << ", prescale: " << metaData.m_hltInfo->getPrescale(m_bestHltName) << ", event: " << data.m_eventmetadata->nRun);
		}

		if (m_verbose)
			LOG("selected " << m_bestHltName << " as best HLT, prescale: " << metaData.m_hltInfo->getPrescale(m_bestHltName));

		if (m_bestHltName.empty())
			LOG_FATAL("No HLT trigger path found at all!");

		metaData.SetSelectedHlt(m_bestHltName);
		return true;
	}

	static std::string Name()
	{
		return "hlt_selector";
	}

private:
	mutable std::string m_bestHltName;
	stringvector m_hltpaths;
	bool m_verbose;

};

}
