#pragma once

#include <vector>

#include <boost/noncopyable.hpp>

#include "EventPipeline.h"

namespace CalibFW
{

template < class TPipeline >
class EventPipelineRunner : public boost::noncopyable
{
public:
	typedef boost::ptr_vector<TPipeline> Pipelines;
	typedef typename Pipelines::iterator ConsumerVectorIterator;

	void AddPipeline( TPipeline * pline );
	void AddPipelines( std::vector<TPipeline *> pVec );

	// EventData -> const for all Pipelines
	// EventMetadata -> different for each pipeline, generated for each event and pipeline ( filter results, cut results, etc )
	// apply cuts of pipeline
	// -> more generic pipeline modifiers


private:

	Pipelines m_pipelines;
};

}
