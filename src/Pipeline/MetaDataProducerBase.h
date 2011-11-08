#pragma once

#include <boost/noncopyable.hpp>

namespace CalibFW
{

/*
 * This producer creates meta-data for a pipeline and event before the filter or the consumer are run
 * Meta data producer have to be stateless since they are used by multiple threads
 */
template<class TData, class TMetaData, class TSettings>
class MetaDataProducerBase: public boost::noncopyable
{
public:
	virtual ~MetaDataProducerBase()
	{

	}

	virtual void PopulateMetaData(TData const& data, TMetaData & metaData,
			TSettings const& m_pipelineSettings) const = 0;

    // if false is returned, the event is dropped as it does not meet the minimum requirements for the producer
	virtual bool PopulateGlobalMetaData(TData const& data, TMetaData & metaData,
			TSettings const& globalSettings) const
	{
		// optional
        return true;
	}

};

}
