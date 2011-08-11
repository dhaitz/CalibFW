#pragma once

#include <boost/noncopyable.hpp>

namespace CalibFW
{

// forward define to be able to use the event pipeline here
template<class TData, class TMetaData, class TSettings>
class EventPipeline;

class FilterResult
{
public:
	FilterResult(bool bHasPassed) :
		m_bHasPassed(bHasPassed)
	{
	}

	bool HasPassed()
	{
		return m_bHasPassed;
	}

private:
	bool m_bHasPassed;
};

template<class TData, class TMetaData, class TSettings>
class FilterBase: public boost::noncopyable
{
public:
	virtual ~FilterBase()
	{
	}

	// todo: do we event need this init this ?
	virtual void Init(EventPipeline<TData, TMetaData, TSettings> * pset)
	{
		//m_pipeline = pset;
	}
	virtual void Finish()
	{
	}

	virtual std::string GetFilterId() = 0;

	virtual bool DoesEventPass( TData const& event, TMetaData const& metaData, TSettings const& settings) = 0;

	virtual std::string ToString(bool bVerbose = false)
	{
		return GetFilterId();
	}
/*
	TSettings * GetPipelineSettings()
	{
		return m_pipeline->GetSettings();
	}
*/
	//EventPipeline<TData, TMetaData, TSettings> * m_pipeline;
};

}

