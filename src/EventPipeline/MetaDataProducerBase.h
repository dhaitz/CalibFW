#pragma once

#include <boost/noncopyable.hpp>

namespace CalibFW
{

/* This producer creates meta-data for a pipeline and event before the filter or the consumer are run
 * Meta data producer have to be stateless since they may used by multiple threads
 */

// Basis for producers attached to a pipeline and producing local results for this pipeline
template<class TData, class TMetaData,  class TSettings>
class LocalMetaDataProducerBase: public boost::noncopyable
{
public:

	virtual ~LocalMetaDataProducerBase()
	{

	}

	virtual void PopulateLocal(TData const& data, TMetaData const& metaData,
							   typename TMetaData::LocalMetaDataType& localMetaData,
							   TSettings const& m_pipelineSettings) const = 0;
};

// Basis for producers which are run before the pipelines and produce
// results useful to all
template<class TData, class TGlobalMetaData, class TSettings>
class GlobalMetaDataProducerBase: public boost::noncopyable
{
public:
	virtual ~GlobalMetaDataProducerBase()
	{
	}

	// if false is returned, the event is dropped as it does not meet the minimum requirements for the producer
	virtual bool PopulateGlobalMetaData(TData const& data, TGlobalMetaData& metaData,
										TSettings const& globalSettings) const = 0;
};


}
