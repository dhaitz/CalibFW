#pragma once

#include "ZJetProducer/MetadataProducer.h"

namespace CalibFW
{

class HltSelector: public ZJetGlobalMetaDataProducerBase
{
public:

	HltSelector(stringvector paths) : m_hltpaths(paths), m_verbose(false)
	{
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
			for (std::vector< std::string >::const_iterator it = data.GetDataLumiMetadata()->hltNames.begin();
				 it != data.GetDataLumiMetadata()->hltNames.end(); ++it)
			{
				std::cout << *it << std::endl;
			}
		}

		bool unprescaledPathFound = false;
		std::string bestHltName, curName;

		if (m_hltpaths.size() == 0)
			CALIB_LOG_FATAL("No Hlt Trigger path list configured");

		for (stringvector::const_iterator it = m_hltpaths.begin();
			 it != m_hltpaths.end(); ++it)
		{
			curName = metaData.m_hltInfo->getHLTName(*it);

			if (m_verbose)
				std::cout << *it << " becomes " << curName << std::endl;

			if (!curName.empty())
			{
				bestHltName = curName;
				if (!metaData.m_hltInfo->isPrescaled(curName))
				{
					unprescaledPathFound = true;
					break;
				}
			}
		}

		if (!unprescaledPathFound)
		{
			CALIB_LOG("Available Triggers:");

			for (std::vector< std::string >::const_iterator it = data.GetDataLumiMetadata()->hltNames.begin();
				 it != data.GetDataLumiMetadata()->hltNames.end(); ++ it)
			{
				CALIB_LOG((*it) << " prescale: " << metaData.m_hltInfo->getPrescale(*it));
			}

			CALIB_LOG_FATAL("No unprescaled trigger found for " << bestHltName << ", prescale: " << metaData.m_hltInfo->getPrescale(bestHltName) << ", event: " << data.m_eventmetadata->nRun);
		}

		if (m_verbose)
			CALIB_LOG("selected " << bestHltName << " as best HLT, prescale: " << metaData.m_hltInfo->getPrescale(bestHltName));

		if (bestHltName.empty())
			CALIB_LOG_FATAL("No HLT trigger path found at all!");

		metaData.SetSelectedHlt(bestHltName);
		return true;
	}

	static std::string Name()
	{
		return "hlt_selector";
	}

	stringvector m_hltpaths;
	bool m_verbose;

};

}
