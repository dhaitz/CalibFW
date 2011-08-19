#pragma once

#include <boost/noncopyable.hpp>

#include <map>

namespace CalibFW
{

// forward define to be able to use the event pipeline here
template<class TData, class TMetaData, class TSettings>
class EventPipeline;

class FilterResult
{
public:

	typedef std::map<std::string, bool> FilterDecisions;

	FilterResult():
		m_bHasPassed(false)
	{
	}

	FilterResult(bool bHasPassed):
		m_bHasPassed(bHasPassed)
	{
	}

	bool HasPassed()
	{
		return m_bHasPassed;
	}

	bool PassedIfExcludingFilter(std::string excludedFilter )
	{
		for ( std::map<std::string, bool>::iterator it = m_filterDecision.begin();
			it != m_filterDecision.end();
			it ++ )
		{
			if ( it->second == false)
			{
				if( it->first != excludedFilter )
					return false;
			}
		}

		return true;
	}

	FilterDecisions const&  GetFilterDecisions() const
	{
		return m_filterDecision;
	}

	// optimize this, without strings
	FilterDecisions m_filterDecision;
	bool m_bHasPassed;

private:

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

