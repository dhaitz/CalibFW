#pragma once

#include <vector>

#include <boost/noncopyable.hpp>

#include "EventPipeline.h"

namespace CalibFW
{

class EventMetaDataBase : public boost::noncopyable
{
	virtual ~EventMetaDataBase()
	{
	}
};

template < class TPipelines >
class EventPipelineRunner : public boost::noncopyable
{
public:
	typedef EventConsumerBase<TData, TMetaData,TSettings> ConsumerForThisPipeline;
	typedef boost::ptr_vector<EventConsumerBase<TData,TMetaData, TSettings> > ConsumerVector;
	typedef typename ConsumerVector::iterator ConsumerVectorIterator;

	typedef FilterBase<TData, TMetaData, TSettings> FilterForThisPipeline;
	typedef boost::ptr_vector<FilterBase<TData, TMetaData,TSettings> > FilterVector;
	typedef typename FilterVector::iterator FilterVectorIterator;


	// EventData -> const for all Pipelines
	// EventMetadata -> different for each pipeline, generated for each event and pipeline ( filter results, cut results, etc )
	// apply cuts of pipeline
	// -> more generic pipeline modifiers


private:

};

}


