#pragma once

#include "ZJet/MetaDataProducer/ZJetMetaDataProducer.h"

namespace CalibFW
{

/*
 * Matches all jets in the event by the geomerty
 */

class HltSelector: public ZJetMetaDataProducerBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const { }

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
			{
/*
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
*/
			}

};


}
