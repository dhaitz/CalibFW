#pragma once

#include "ZJet/MetaDataProducer/ZJetMetaDataProducer.h"

namespace CalibFW
{

/*
 * Matches all jets in the event by the geomerty
 */

class JetMatcher: public ZJetMetaDataProducerBase
{
public:
	JetMatcher ( std::string input_jets1,
			std::string input_jets2,
			std::string matching_name) :
				m_jets1 ( input_jets1 ),
				m_jets2 ( input_jets2) ,
				m_matching_name( matching_name)	{ }

	virtual ~JetMatcher() {}

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const { }

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const;
private:
	std::string m_jets1, m_jets2, m_matching_name;
};


}
