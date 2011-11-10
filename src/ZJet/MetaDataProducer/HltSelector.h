#pragma once

#include "ZJet/MetaDataProducer/ZJetMetaDataProducer.h"

namespace CalibFW
{

class HltSelector: public ZJetMetaDataProducerBase
{
public:

	HltSelector( stringvector paths )
	 : m_hltpaths ( paths ), m_verbose ( false )
	{
	}

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const { }

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
			{
                bool unprescaledPathFound = false;
                std::string bestHltName, curName;

                if ( m_hltpaths.size() == 0)
                {
                	CALIB_LOG_FATAL("No Hlt Trigger path list configured")
                }

                for ( stringvector::const_iterator it = m_hltpaths.begin();
                        it != m_hltpaths.end();
                        ++ it )
                {
                    curName = metaData.m_hltInfo->getHLTName( *it );

                    if ( m_verbose )
                    	std::cout << *it << " becomes " << curName << std::endl;

                    if ( 	( curName != "" ) &&
                    		metaData.m_hltInfo->isAvailable(curName) )
                    {
                    	bestHltName = curName;
                    	if ( !metaData.m_hltInfo->isPrescaled(curName))
                    	{
                    		unprescaledPathFound = true;
                            break;
                    	}
                    }
                }

                if ( ! unprescaledPathFound )
                {
                    //CALIB_LOG_FATAL("No unprescaled trigger found" );
                	if ( m_verbose)
                	{
                		CALIB_LOG("Warning: No unprescaled trigger found for " << bestHltName )
                	}
                }

                metaData.SetSelectedHlt( bestHltName );

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
