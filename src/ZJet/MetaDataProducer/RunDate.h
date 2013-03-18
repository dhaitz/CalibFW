#pragma once

#include "../ZJetPipeline.h"
#include "RunMap.cpp"

namespace CalibFW
{


class RunDate: public ZJetGlobalMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	static std::string Name() { return "run_date_producer"; }

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
        if (runmap.find(data.m_eventmetadata->nRun) != runmap.end())
            metaData.SetRunDate(runmap.at(data.m_eventmetadata->nRun));
        else
            {
                CALIB_LOG("Rundate not found: " << data.m_eventmetadata->nRun)
                metaData.SetRunDate(0);
            }
   		return true;
	}

};


}
