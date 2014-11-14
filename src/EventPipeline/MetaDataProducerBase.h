#pragma once

#include <boost/noncopyable.hpp>

namespace Artus
{

/* This producer creates meta-data for a pipeline and event before the filter or the consumer are run
 * Meta data producer have to be stateless since they may used by multiple threads
 */

// Basis for producers attached to a pipeline and producing local results for this pipeline
template<class TData, class TProduct,  class TSettings>
class LocalProducerBase: public boost::noncopyable
{
public:

	virtual ~LocalProducerBase()
	{

	}

	virtual void PopulateLocal(TData const& data, TProduct const& product,
							   typename TProduct::LocalproductType& localproduct,
							   TSettings const& m_pipelineSettings) const = 0;
};

// Basis for producers which are run before the pipelines and produce
// results useful to all
template<class TData, class TGlobalProduct, class TSettings>
class GlobalProductProducerBase: public boost::noncopyable
{
public:
	virtual ~GlobalProductProducerBase()
	{
	}

	// if false is returned, the event is dropped as it does not meet the minimum requirements for the producer
	virtual bool PopulateGlobalProduct(TData const& data, TGlobalProduct& product,
									   TSettings const& globalSettings) const = 0;

	virtual stringvector GetListOfNeededProducers()
	{
		return stringvector();
	};
};


}
